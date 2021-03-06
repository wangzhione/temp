#include "socket_server.h"

#define SOCKET_TYPE_INVALID         0
#define SOCKET_TYPE_RESERVE         1
#define SOCKET_TYPE_PLISTEN         2
#define SOCKET_TYPE_LISTEN          3
#define SOCKET_TYPE_CONNECTING      4
#define SOCKET_TYPE_CONNECTED       5
#define SOCKET_TYPE_HALFCLOSE_READ  6
#define SOCKET_TYPE_HALFCLOSE_WRITE 7
#define SOCKET_TYPE_PACCEPT         8
#define SOCKET_TYPE_BIND            9

#define PRIORITY_HIGH 0
#define PRIORITY_LOW  1

#define PROTOCOL_TCP        0
#define PROTOCOL_UDP        1
#define PROTOCOL_UDPv6      2
#define PROTOCOL_UNKNOWN    255

#define MAX_INFO 128

#define MIN_READ_BUFFER 64

#define UDP_ADDRESS_SIZE 19 // ipv6 128bit + port 16bit + 1 byte type

#define MAX_UDP_PACKAGE 65535

#define WARNING_SIZE (1024*1024)

// MAX_SOCKET will be 2^MAX_SOCKET_P
#define MAX_SOCKET_P    16
#define MAX_SOCKET      (1<<MAX_SOCKET_P)
#define HASH_ID(id)     (((unsigned)(id)) % MAX_SOCKET)
#define ID_TAG16(id)    (((id)>>MAX_SOCKET_P) & 0xFFFF)

#if EAGAIN != EWOULDBLOCK
#define AGAIN_WOULDBLOCK EAGAIN : case EWOULDBLOCK:
#else
#define AGAIN_WOULDBLOCK EAGAIN
#endif

struct write_buffer {
    struct write_buffer * next;
    const void * buffer;
    char * ptr;
    int sz;
    bool userobject;
    uint8_t udp_address[UDP_ADDRESS_SIZE];
};

#define SIZEOF_TCPBUFFER (offsetof(struct write_buffer, udp_address[0]))
#define SIZEOF_UDPBUFFER (sizeof(struct write_buffer))

struct wb_list {
    struct write_buffer * head;
    struct write_buffer * tail;
};

struct socket_stat {
    uint64_t rtime;
    uint64_t wtime;
	uint64_t read;
	uint64_t write;
};

struct socket {
    uintptr_t opaque;
    struct wb_list high;
    struct wb_list low;
    int wb_size;
    struct socket_stat stat;
    atomic_ulong sending;
    socket_t fd;
    int id;
    atomic_int type;
    uint8_t protocol;
    bool reading;
    bool writing;
    bool closing;
    atomic_int udpconnecting;
    int warn_size;
    union {
        int size;
        uint8_t udp_address[UDP_ADDRESS_SIZE];
    } p;
    atomic_flag dw_lock;
    int dw_offset;
    const void * dw_buffer;
    int dw_size;
};

struct socket_server {
    volatile uint64_t time;
    socket_t recvctrl_fd;
    socket_t sendctrl_fd;
    int checkctrl;
    spoll_t event_fd;
    atomic_int alloc_id;
    int event_n;
    int event_index;
    struct socket_object_interface soi;
    spoll_event_t ev;
    struct socket slot[MAX_SOCKET];
    char buffer[MAX_INFO];
    uint8_t udpbuffer[MAX_UDP_PACKAGE];
    fd_set rfds;
};

struct request_open {
    int id;
    uint16_t port;
    uintptr_t opaque;
    char host[1];
};

struct request_send {
    int id;
    int sz;
    const void * buffer;
};

struct request_send_udp {
    struct request_send send;
    uint8_t address[UDP_ADDRESS_SIZE];
};

struct request_setudp {
    int id;
    uint8_t address[UDP_ADDRESS_SIZE];
};

struct request_close {
    int id;
    int shutdown;
    uintptr_t opaque;
};

struct request_listen {
    int id;
    socket_t fd;
    uintptr_t opaque;
    char host[1];
};

struct request_bind {
    int id;
    socket_t fd;
    uintptr_t opaque;
};

struct request_resumepause {
    int id;
    uintptr_t opaque;
};

struct request_setopt {
    int id;
    int what;
    int value;
};

struct request_udp {
    int id;
    socket_t fd;
    int family;
    uintptr_t opaque;
};

struct request_package {
    uint8_t header[8]; // 6 bytes dummy
    union {
        char buffer[256];
        struct request_open open;
        struct request_send send;
        struct request_send_udp send_udp;
        struct request_close close;
        struct request_listen listen;
        struct request_bind bind;
        struct request_resumepause resumepause;
        struct request_setopt setopt;
        struct request_udp udp;
        struct request_setudp set_udp;
    } u;
    uint8_t dummy[256];
};

struct send_object {
    const void * buffer;
    int sz;
    void (* free_func)(void * object);
};

struct socket_lock {
    atomic_flag * lock;
    int count;
};

static inline void socket_lock_init(struct socket_lock * sl, struct socket * s) {
    sl->lock = &s->dw_lock;
    sl->count = 0;
}

static inline void socket_lock(struct socket_lock * sl) {
    if (sl->count == 0) {
        atomic_flag_lock(sl->lock);
    }
    ++sl->count;
}

static inline int socket_trylock(struct socket_lock * sl) {
    if (sl->count == 0) {
        if (!atomic_flag_trylock(sl->lock))
            return 0; // lock failed
    }
    ++sl->count;
    return 1;
}

static inline void socket_unlock(struct socket_lock * sl) {
    --sl->count;
    if (sl->count <= 0) {
        assert(sl->count == 0);
        atomic_flag_unlock(sl->lock);
    }
}

static inline int socket_invalid(struct socket * s, int id) {
    return s->id != id || atomic_load(&s->type) == SOCKET_TYPE_INVALID;
}

static inline bool send_object_init(struct socket_server * ss, struct send_object * so, const void * obejct, int sz) {
    if (sz == USEROBJECT) {
        so->buffer = ss->soi.buffer(obejct);
        so->sz = ss->soi.size(obejct);
        so->free_func = ss->soi.free;
        return true;
    }
    so->buffer = obejct;
    so->sz = sz;
    so->free_func = free;
    return false;
}

static inline void dummy_free(void * ptr) {
    (void)ptr;
}

static inline void send_object_init_from_sendbuffer(struct socket_server * ss, struct send_object * so, struct socket_sendbuffer * buf) {
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        send_object_init(ss, so, buf->buffer, buf->sz);
        break;
    case SOCKET_BUFFER_OBJECT:
        send_object_init(ss, so, buf->buffer, USEROBJECT);
        break;
    case SOCKET_BUFFER_RAWPOINTER:
        so->buffer = buf->buffer;
        so->sz = buf->sz;
        so->free_func = dummy_free;
        break;
    default:
        // never get here
        so->buffer = NULL;
        so->sz = 0;
        so->free_func = NULL;
        break;   
    }
}

static inline void write_buffer_free(struct socket_server * ss, struct write_buffer * wb) {
    if (wb->userobject) {
        ss->soi.free((void *)wb->buffer);
    } else {
        free((void *)wb->buffer);
    }
    free(wb);
}

static int reserve_id(struct socket_server * ss) {
    for (int i = 0; i < MAX_SOCKET; i++) {
        int id = atomic_fetch_add(&ss->alloc_id, 1) + 1;
        if (id < 0) {
            id = atomic_fetch_and(&ss->alloc_id, 0x7FFFFFFF) & 0x7FFFFFFF;
        }
        struct socket * s = &ss->slot[HASH_ID(id)];
        int type_invalid = atomic_load(&s->type);
        if (type_invalid == SOCKET_TYPE_INVALID) {
            if (atomic_compare_exchange_weak(&s->type, &type_invalid, SOCKET_TYPE_RESERVE)) {
                s->id = id;
                s->protocol = PROTOCOL_UNKNOWN;
				// socket_server_udp_connect may inc s->udpconncting directly (from other thread, before new_fd), 
				// so reset it to 0 here rather than in new_fd.
                atomic_init(&s->udpconnecting, 0);
                s->fd = INVALID_SOCKET;
                return id;
            }

            // retry
            --i;
        }
    }
    return -1;
}

static inline void clear_wb_list(struct wb_list * list) {
    list->head = NULL;
    list->tail = NULL;
}

struct socket_server *
socket_server_create(uint64_t time) {
    spoll_t efd = spoll_create();
    if (spoll_invalid(efd)) {
        RETNUL("socket-server: create event pool failed.");
    }
    
    socket_t fd[2];
    if (pipe(fd)) {
        spoll_delete(efd);
        RETNUL("socket-server: create socket pair failed.");
    }
    if (spoll_add(efd, fd[0], NULL)) {
        // add recvctrl_fd to event poll
        socket_close(fd[0]);
        socket_close(fd[1]);
        spoll_delete(efd);
        RETNUL("socket-server: can't add server fd to event pool.");
    }

    struct socket_server * ss = malloc(sizeof(struct socket_server));
    ss->time = time;
    ss->event_fd = efd;
    ss->recvctrl_fd = fd[0];
    ss->sendctrl_fd = fd[1];
    ss->checkctrl = 1;

    for (int i = 0; i < MAX_SOCKET; i++) {
        struct socket * s = &ss->slot[i];
        atomic_init(&s->type, SOCKET_TYPE_INVALID);
        clear_wb_list(&s->high);
        clear_wb_list(&s->low);
        s->dw_lock = (atomic_flag)ATOMIC_FLAG_INIT;
    }
    
    atomic_init(&ss->alloc_id, 0);
    ss->event_n = 0;
    ss->event_index = 0;
    memset(&ss->soi, 0 , sizeof(ss->soi));
    FD_ZERO(&ss->rfds);
    assert(ss->recvctrl_fd < FD_SETSIZE);

    return ss;
}

inline void
socket_server_updatetime(struct socket_server * ss, uint64_t time) {
    ss->time = time;
}

static void free_wb_list(struct socket_server * ss, struct wb_list * list) {
    struct write_buffer * wb = list->head;
    while (wb) {
        struct write_buffer * temp = wb;
        wb = wb->next;
        write_buffer_free(ss, temp);
    }
    list->head = NULL;
    list->tail = NULL;
}

static void free_buffer(struct socket_server * ss, struct socket_sendbuffer * buf) {
    void * buffer = (void *)buf->buffer;
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        free(buffer);
        break;
    case SOCKET_BUFFER_OBJECT:
        ss->soi.free(buf);
        break;
    case SOCKET_BUFFER_RAWPOINTER:
        break;
    }
}

static void force_close(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    result->id = s->id;
    result->ud = 0;
    result->data = NULL;
    result->opaque = s->opaque;
    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_INVALID) {
        return;
    }
    
    assert(type != SOCKET_TYPE_RESERVE);
    free_wb_list(ss, &s->high);
    free_wb_list(ss, &s->low);
    spoll_del(ss->event_fd, s->fd);

    socket_lock(sl);
    if (type != SOCKET_TYPE_BIND) {
        if (socket_close(s->fd) == SOCKET_ERROR) {
            CERR("close socket");
        }
    }

    atomic_store(&s->type, SOCKET_TYPE_INVALID);
    if (s->dw_buffer) {
        struct socket_sendbuffer temp;
        temp.buffer = s->dw_buffer;
        temp.sz = s->dw_size;
        temp.id = s->id;
        temp.type = temp.sz == USEROBJECT ? SOCKET_BUFFER_OBJECT : SOCKET_BUFFER_MEMORY;
        free_buffer(ss, &temp);
        s->dw_buffer = NULL;
    }
    socket_unlock(sl);
}

void
socket_server_delete(struct socket_server * ss) {
    struct socket_message dummy;
    for (int i = 0; i < MAX_SOCKET; i++) {
        struct socket * s = &ss->slot[i];
        struct socket_lock sl;
        socket_lock_init(&sl, s);
        if (atomic_load(&s->type) != SOCKET_TYPE_RESERVE) {
            force_close(ss, s, &sl, &dummy);
        }
    }
    socket_close(ss->sendctrl_fd);
    socket_close(ss->sendctrl_fd);
    spoll_delete(ss->event_fd);
    free(ss);
}

static inline void check_wb_list(struct wb_list * list) {
    assert(list->head == NULL);
    assert(list->tail == NULL);
}

static inline int enable_write(struct socket_server * ss, struct socket * s, bool enable) {
    if (s->writing != enable) {
        s->writing = enable;
        return spoll_mod(ss->event_fd, s->fd, s, s->reading, enable);
    }
    return 0;
}

static inline int enable_read(struct socket_server * ss, struct socket * s, bool enable) {
    if (s->reading != enable) {
        s->reading = enable;
        return spoll_mod(ss->event_fd, s->fd, s, enable, s->writing);
    }
    return 0;
}

static struct socket * new_fd(struct socket_server * ss, int id, socket_t fd, int protocol, uintptr_t opaque, bool reading) {
    struct socket * s = &ss->slot[HASH_ID(id)];
    assert(atomic_load(&s->type) == SOCKET_TYPE_RESERVE);

    if (spoll_add(ss->event_fd, fd, s)) {
        atomic_store(&s->type, SOCKET_TYPE_INVALID);
        return NULL;
    }

    s->id = id;
    s->fd = fd;
    s->reading = true;
    s->writing = false;
    s->closing = false;
    atomic_init(&s->sending, ID_TAG16(id) << 16);
    s->protocol = protocol;
    s->p.size = MIN_READ_BUFFER;
    s->opaque = opaque;
    s->wb_size = 0;
    s->warn_size = 0;
    check_wb_list(&s->high);
    check_wb_list(&s->low);
    s->dw_buffer = NULL;
    s->dw_size = 0;
    memset(&s->stat, 0, sizeof(s->stat));
    if (enable_read(ss, s, reading)) {
        atomic_store(&s->type, SOCKET_TYPE_INVALID);
        return NULL;
    }
    return s;
}

static inline void stat_read(struct socket_server * ss, struct socket * s, int n) {
    s->stat.read += n;
    s->stat.rtime = ss->time;
}

static inline void stat_write(struct socket_server * ss, struct socket * s, int n) {
    s->stat.write += n;
    s->stat.wtime = ss->time;
}

// return -1 when conencting
static int open_socket(struct socket_server * ss, struct request_open * request, struct socket_message * result) {
    int id = request->id;

    result->opaque = request->opaque;
    result->id = id;
    result->ud = 0;
    result->data = NULL;

    int status;
    struct socket * s;
    struct addrinfo * ai_list = NULL;
    struct addrinfo * ai_ptr = NULL;
    struct addrinfo ai_hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
    };
    char port[sizeof "65535"];
    sprintf(port, "%hu", request->port);
    status = getaddrinfo(request->host, port, &ai_hints, &ai_list);
    if (status < 0) {
        result->data = (void *)gai_strerror(status);
        goto err_failed_getaddrinfo;
    }

    socket_t sock = INVALID_SOCKET;
    for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
        sock = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            continue;
        }

        socket_set_keepalive(sock);
        socket_set_nonblock(sock);
        status = connect(sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
        if (status < 0 && errno != EINPROGRESS) {
            close(sock);
            sock = INVALID_SOCKET;
            continue;
        }

        break;
    }
    if (sock == INVALID_SOCKET) {
        result->data = (void *)strerr(errno);
        goto err_failed;
    }

    s = new_fd(ss, id, sock, PROTOCOL_TCP, request->opaque, true);
	if (s == NULL) {
		close(sock);
		result->data = "reach skynet socket number limit";
		goto err_failed;
	}

	if(status == 0) {
		atomic_store(&s->type , SOCKET_TYPE_CONNECTED);
		struct sockaddr * addr = ai_ptr->ai_addr;
		void * sin_addr = (ai_ptr->ai_family == AF_INET) ? (void*)&((struct sockaddr_in *)addr)->sin_addr : (void*)&((struct sockaddr_in6 *)addr)->sin6_addr;
		if (inet_ntop(ai_ptr->ai_family, sin_addr, ss->buffer, sizeof(ss->buffer))) {
			result->data = ss->buffer;
		}
		freeaddrinfo( ai_list );
		return SOCKET_OPEN;
	} else {
		atomic_store(&s->type , SOCKET_TYPE_CONNECTING);
		if (enable_write(ss, s, true)) {
			result->data = "enable write failed";
			goto err_failed;
		}
	}

	freeaddrinfo( ai_list );
	return -1;

err_failed:
    freeaddrinfo(ai_list);
err_failed_getaddrinfo:
    ss->slot[HASH_ID(id)].type = SOCKET_TYPE_INVALID;
    return SOCKET_ERR;
}

static int report_error(struct socket * s, struct socket_message * result, const char * err) {
    result->id = s->id;
    result->ud = 0;
    result->opaque = s->opaque;
    result->data = (void *)err;
    return SOCKET_ERR;
}

static int close_write(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    if (s->closing) {
        force_close(ss, s, sl, result);
        return SOCKET_RST;
    }
    int type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_HALFCLOSE_READ) {
        // recv 0 befroe, ignore the error and close fd
        force_close(ss, s, sl, result);
        return SOCKET_RST;
    }
    if (type == SOCKET_TYPE_HALFCLOSE_WRITE) {
        // already raise SOCKET_ERR
        return SOCKET_RST;
    }
    atomic_store(&s->type, SOCKET_TYPE_HALFCLOSE_WRITE);
    shutdown(s->fd, SHUT_WR);
    enable_write(ss, s, false);
    return report_error(s, result, strerr(errno));
}

static int send_list_tcp(struct socket_server * ss, struct socket * s, struct wb_list * list, struct socket_lock * sl, struct socket_message * result) {
    while (list->head) {
        struct write_buffer * temp = list->head;

        for (;;) {
            int sz = socket_send(s->fd, temp->ptr, temp->sz);
            if (sz < 0) {
                switch (errno) {
                case EINTR:
                    continue;
                case AGAIN_WOULDBLOCK:
                    return -1;        
                }

                return close_write(ss, s, sl, result);
            }

            stat_write(ss, s, sz);
            s->wb_size -= sz;
            if (sz != temp->sz) {
                temp->ptr += sz;
                temp->sz -= sz;
                return -1;
            }

            break;
        }

        list->head = temp->next;
        write_buffer_free(ss, temp);
    }
    list->tail = NULL;

    return -1;
}

static socklen_t udp_socket_address(struct socket * s, const uint8_t udp_address[UDP_ADDRESS_SIZE], union sockaddr_all * sa) {
    int type = (uint8_t)udp_address[0];
    if (type != s->protocol)
        return 0;

    uint16_t port = 0;
    memcpy(&port, udp_address+1, sizeof(uint16_t));

    switch (s->protocol) {
    case PROTOCOL_UDP:
        memset(&sa->v4, 0, sizeof(sa->v4));
        sa->v4.sin_family = AF_INET;
        sa->v4.sin_port = port;
        memcpy(&sa->v4.sin_addr, udp_address+1+sizeof(uint16_t), sizeof(sa->v4.sin_addr));
        return sizeof(sa->v4);
    case PROTOCOL_UDPv6:
        memset(&sa->v6, 0, sizeof(sa->v6));
        sa->v6.sin6_family = AF_INET6;
        sa->v6.sin6_port = port;
        memcpy(&sa->v6.sin6_addr, udp_address+1+sizeof(uint16_t), sizeof(sa->v6.sin6_addr));
        return sizeof(sa->v6);
    }

    return 0;
}

static void drop_udp(struct socket_server * ss, struct socket * s, struct wb_list * list, struct write_buffer * temp) {
    s->wb_size -= temp->sz;
    list->head = temp->next;
    if (list->head == NULL)
        list->tail = NULL;
    write_buffer_free(ss, temp);
}

static int send_list_udp(struct socket_server * ss, struct socket * s, struct wb_list * list, struct socket_message * result) {
    while (list->head) {
        struct write_buffer * temp = list->head;
        union sockaddr_all sa;
        socklen_t szlen = udp_socket_address(s, temp->udp_address, &sa);
        if (szlen == 0) {
            CERR("socket-server : udp (%d) type mismatch.", s->id);
            drop_udp(ss, s, list, temp);
            return -1;
        }

        int err = sendto(s->fd, temp->ptr, temp->sz, 0, &sa.s, szlen);
        if (err < 0) {
            switch (errno) {
            case EINTR:
            case AGAIN_WOULDBLOCK:
                return -1;    
            }

            CERR("socket-server : udp sendto error");
            drop_udp(ss, s, list, temp);
            return -1;
        }

        stat_write(ss, s, temp->sz);
        s->wb_size -= temp->sz;
        list->head = temp->next;
        write_buffer_free(ss, temp);
    }
    list->tail = NULL;

    (void) result;
    return -1;;
}

static int send_list(struct socket_server * ss, struct socket * s, struct wb_list * list, struct socket_lock * sl, struct socket_message * result) {
    if (s->protocol == PROTOCOL_TCP)
        return send_list_tcp(ss, s, list, sl, result);
    return send_list_udp(ss, s, list, result);
}

static inline bool list_uncomplete(struct wb_list * list) {
    struct write_buffer * temp = list->head;
    if (temp == NULL)
        return false;
    return (void *)temp->ptr != temp->buffer;
}

static void raise_uncomplete(struct socket * s) {
    struct wb_list * low = &s->low;
    struct write_buffer * temp = low->head;
    low->head = temp->next;
    if (low->head == NULL) {
        low->tail = NULL;
    }

    // move head of low list (temp) to the empty high list
    struct wb_list * high = &s->high;
    assert(high->head == NULL);

    temp->next = NULL;
    high->head = high->tail = temp;
}

static inline bool send_buffer_empty(struct socket * s) {
    return s->high.head == NULL && s->low.head == NULL;
}

/*
	Each socket has two write buffer list, high priority and low priority.

	1. send high list as far as possible.
	2. If high list is empty, try to send low list.
	3. If low list head is uncomplete (send a part before), move the head of low list to empty high list (call raise_uncomplete) .
	4. If two lists are both empty, turn off the event. (call check_close)
 */
static int send_buffer_partial(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    assert(!list_uncomplete(&s->low));
    // step 1
    int ret = send_list(ss, s, &s->high, sl, result);
    if (ret != -1) {
        if (ret == SOCKET_ERR) {
            // HALFCLOSE_WRITE
            return SOCKET_ERR;
        }
        // SOCKET_RST (ignore)
        return -1;
    }

    if (s->high.head == NULL) {
        // step 2
        if (s->low.head != NULL) {
            ret = send_list(ss, s, &s->low, sl, result);
            if (ret != -1) {
                if (ret == SOCKET_ERR) {
                    // HALFCLOSE_WRITE
                    return SOCKET_ERR;
                }
                // SOCKET_RST (ignore)
                return -1;
            }

            // step 3
            if (list_uncomplete(&s->low)) {
                raise_uncomplete(s);
                return -1;
            }
            if (s->low.head)
                return -1;
        }

        // step 4
        assert(send_buffer_empty(s) && s->wb_size == 0);

        if (s->closing) {
            // finish writing
            force_close(ss, s, sl, result);
            return -1;
        }

        int err = enable_write(ss, s, false);
        if (err) {
            return report_error(s, result, "disable write failed");
        }

        if (s->warn_size > 0) {
            s->warn_size = 0;
            result->opaque = s->opaque;
            result->id = s->id;
            result->ud = 0;
            result->data = NULL;
            return SOCKET_WAARNING;
        }
    }

    return -1;
}

static int send_buffer(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    if (!socket_trylock(sl))
        return -1; // blocked by direct write, send later.

    if (s->dw_buffer) {
        // add direct write buffer before high.head
        struct write_buffer * buf = malloc(SIZEOF_TCPBUFFER);
        struct send_object so;
        buf->userobject = send_object_init(ss, &so, s->dw_buffer, s->dw_size);
        buf->ptr = (char *)so.buffer + s->dw_offset;
        buf->sz = so.sz - s->dw_offset;
        buf->buffer = s->dw_buffer;
        s->wb_size += buf->sz;
        if (s->high.head == NULL) {
            s->high.head = s->high.tail = buf;
            buf->next = NULL;
        } else {
            buf->next = s->high.head;
            s->high.head = buf;
        }
        s->dw_buffer = NULL;
    }

    int r = send_buffer_partial(ss, s, sl, result);
    socket_unlock(sl);

    return r;
}

static struct write_buffer * append_sendbuffer_partial(struct socket_server * ss, struct wb_list * s, struct request_send * request, int size) {
    struct write_buffer * buf = malloc(size);
    struct send_object so;
    buf->userobject = send_object_init(ss, &so, request->buffer, request->sz);
    buf->ptr = (char *)so.buffer;
    buf->sz = so.sz;
    buf->buffer = request->buffer;
    buf->next = NULL;
    if (s->head == NULL) {
        s->head = s->tail = buf;
    } else {
        assert(s->tail != NULL);
        assert(s->tail->next == NULL);
        s->tail->next = buf;
        s->tail = buf;
    }
    return buf;
}

static inline void append_sendbuffer_udp(struct socket_server * ss, struct socket * s, int priority, struct request_send * request, const uint8_t udp_address[UDP_ADDRESS_SIZE]) {
    struct wb_list * list = priority == PRIORITY_HIGH ? &s->high : &s->low;
    struct write_buffer * buf = append_sendbuffer_partial(ss, list, request, SIZEOF_UDPBUFFER);
    memcpy(buf->udp_address, udp_address, UDP_ADDRESS_SIZE);
    s->wb_size += buf->sz;
}

static inline void append_sendbuffer(struct socket_server * ss, struct socket * s, struct request_send * request) {
    struct write_buffer * buf = append_sendbuffer_partial(ss, &s->high, request, SIZEOF_TCPBUFFER);
    s->wb_size += buf->sz;
}

static inline void append_sendbuffer_low(struct socket_server * ss, struct socket * s, struct request_send * request) {
    struct write_buffer * buf = append_sendbuffer_partial(ss, &s->low, request, SIZEOF_TCPBUFFER);
    s->wb_size += buf->sz;
}

static int trigger_write(struct socket_server * ss, struct request_send * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id))
        return -1;
    if (enable_write(ss, s, true)) {
        return report_error(s, result, "enable write failed");
    }
    return -1;
}

/*
	When send a package , we can assign the priority : PRIORITY_HIGH or PRIORITY_LOW

	If socket buffer is empty, write to fd directly.
		If write a part, append the rest part to high list. (Even priority is PRIORITY_LOW)
	Else append package to high (PRIORITY_HIGH) or low (PRIORITY_LOW) list.
 */
static int send_socket(struct socket_server * ss, struct request_send * request, struct socket_message * result, int priority, const uint8_t * udp_address) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    struct send_object so;
    send_object_init(ss, &so, request->buffer, request->sz);
    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_INVALID || s->id != id 
        || type == SOCKET_TYPE_HALFCLOSE_WRITE
        || type == SOCKET_TYPE_PACCEPT
        || s->closing) {
        so.free_func((void *)request->buffer);
        return -1;
    }
    if (type == SOCKET_TYPE_PLISTEN || type == SOCKET_TYPE_LISTEN) {
        CERR("socket-server: write to listen fd %d.", id);
        so.free_func((void *)request->buffer);
        return -1;
    }
    if (send_buffer_empty(s)) {
        if (s->protocol == PROTOCOL_TCP) {
            append_sendbuffer(ss, s, request); // add to high priority list, even priority == PRIORITY_LOW
        } else {
            // udp
            if (udp_address == NULL) {
                udp_address = s->p.udp_address;
            }
            union sockaddr_all sa;
            socklen_t sasz = udp_socket_address(s, udp_address, &sa);
            if (sasz == 0) {
                // udp type mismatch, just drop it.
                CERR("socket-server: udp socket (%d) type mismatch.", id);
                so.free_func((void *)request->buffer);
                return -1;
            }
            int n = sendto(s->fd, so.buffer, so.sz, 0, &sa.s, sasz);
            if (n != so.sz) {
                append_sendbuffer_udp(ss, s, priority, request, udp_address);
            } else {
                stat_write(ss, s, n);
                so.free_func((void *)request->buffer);
                return -1;
            }
        }

        if (enable_write(ss, s, true)) {
            return report_error(s, result, "enable write failed");
        }
    } else {
        if (s->protocol == PROTOCOL_TCP) {
            if (priority == PRIORITY_LOW) {
                append_sendbuffer_low(ss, s, request);
            } else {
                append_sendbuffer(ss, s, request);
            }
        } else {
            if (udp_address == NULL) {
                udp_address = s->p.udp_address;
            }
            append_sendbuffer_udp(ss, s, priority, request, udp_address);
        }
    }

    if (s->wb_size >= WARNING_SIZE && s->wb_size >= s->warn_size) {
        s->warn_size = s->warn_size == 0 ? WARNING_SIZE * 2 : s->warn_size * 2;
        result->opaque = s->opaque;
        result->id = s->id;
        result->ud = s->wb_size % 1024 == 0 ? s->wb_size/1024 : s->wb_size/1024 + 1;
        result->data = NULL;
        return SOCKET_WAARNING;
    }

    return -1;
}

static int listen_socket(struct socket_server * ss, struct request_listen * request, struct socket_message * result) {
    int id = request->id;
    socket_t listen_fd = request->fd;
    struct socket * s = new_fd(ss, id, listen_fd, PROTOCOL_TCP, request->opaque, false);
    if (s == NULL) {
        goto err_failed;
    }

    atomic_store(&s->type, SOCKET_TYPE_PLISTEN);
    return -1;

err_failed:
    close(listen_fd);
    request->opaque = request->opaque;
    result->id = id;
    result->ud = 0;
    result->data = "reach skynet socket number limit";
    ss->slot[HASH_ID(id)].type = SOCKET_TYPE_INVALID;
    return SOCKET_ERR;
}

static inline bool nomore_sending_data(struct socket * s) {
    return (send_buffer_empty(s) && s->dw_buffer == NULL && (atomic_load(&s->sending) & 0xFFFF) == 0)
        || (atomic_load(&s->type) == SOCKET_TYPE_HALFCLOSE_WRITE);
}

static void close_read(struct socket_server * ss, struct socket * s, struct socket_message * result) {
    // Don't read socket later
    atomic_store(&s->type, SOCKET_TYPE_HALFCLOSE_READ);
    enable_read(ss, s, false);
    shutdown(s->fd, SHUT_RD);
    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = 0;
    result->data = NULL;
}

static inline bool halfclose_read(struct socket * s) {
    return atomic_load(&s->type) == SOCKET_TYPE_HALFCLOSE_READ;
}

// SOCKET_CLOSE can be raised (only once) in one of two conditions.
// See https://github.com/cloudwu/skynet/issues/1346 for more discussion.
// 1. close socket by self, See close_socket()
// 2. recv 0 or eof event (close socket by remote), See forward_message_tcp()
// It's able to write data after SOCKET_CLOSE (In condition 2), but if remote is closed, SOCKET_ERR may raised.
static int close_socket(struct socket_server * ss, struct request_close * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        // The socket is closed, ignore
        return -1;
    }

    struct socket_lock sl;
    socket_lock_init(&sl, s);

    int shutdown_read = halfclose_read(s);

    if (request->shutdown || nomore_sending_data(s)) {
        // If socket is SOCKET_TYPE_HALFCLOSE_READ, Do not raise SOCKET_CLOSE again.
        int r = shutdown_read ? -1 : SOCKET_CLOSE;
        force_close(ss, s, &sl, result);
        return r;
    }

    s->closing = true;
    if (!shutdown_read) {
        // don't read socket after socket.close()
        close_read(ss, s, result);
        return SOCKET_CLOSE;
    }

    // recv 0 before (socket is SOCKET_TYPE_HALFCLOSE_READ) and waiting for sending data out.
    return -1;
}

static int bind_socket(struct socket_server * ss, struct request_bind * request, struct socket_message * result) {
    int id = request->id;
    result->id = id;
    result->opaque = request->opaque;
    result->ud = 0;
    struct socket * s = new_fd(ss, id, request->fd, PROTOCOL_TCP, request->opaque, true);
    if (s == NULL) {
        result->data = "reach skynet socket number limit";
        return SOCKET_ERR;
    }
    
    socket_set_nonblock(request->fd);
    atomic_store(&s->type, SOCKET_TYPE_BIND);
    result->data = "binding";
    return SOCKET_OPEN;
}

static int resume_socket(struct socket_server * ss, struct request_resumepause * request, struct socket_message * result) {
    int id = request->id;
    result->id = id;
    result->opaque = request->opaque;
    result->ud = 0;
    result->data = NULL;
    
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        result->data = "invalid socket";
        return SOCKET_ERR;
    }

    if (halfclose_read(s)) {
        // The closing socket may be in transit, so raise an error. See https://github.com/cloudwu/skynet/issues/1374
        result->data = "socket closed";
        return SOCKET_ERR; 
    }

    struct socket_lock sl;
    socket_lock_init(&sl, s);
    if (enable_read(ss, s, true)) {
        result->data = "enabel read failed";
        return SOCKET_ERR;
    }

    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_PACCEPT || type == SOCKET_TYPE_PLISTEN) {
        atomic_store(&s->type, type == SOCKET_TYPE_PACCEPT ? SOCKET_TYPE_CONNECTED : SOCKET_TYPE_LISTEN);
        s->opaque = request->opaque;
        result->data = "start";
        return SOCKET_OPEN;
    }
    if (type == SOCKET_TYPE_CONNECTED) {
        // todo: maybe we should send a message SOCKET_TRANSFER to s->opaque
        s->opaque = request->opaque;
        result->data = "transfer";
        return SOCKET_OPEN;
    }
    // if s->type == SOCKET_TYPE_HALFCLOSE_WRITE, SOCKET_CLOSE message will send later
    return -1;
}

static int pause_socket(struct socket_server * ss, struct request_resumepause * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        return -1;
    }
    if (enable_read(ss, s, false)) {
        return report_error(s, result, "enable read failed");
    }
    return -1;
}

static void setopt_socket(struct socket_server * ss, struct request_setopt * request) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        return;
    }

    int v = request->value;
    setsockopt(s->fd, IPPROTO_TCP, request->what, &v, sizeof v);
}

static void block_readpipe(socket_t pipefd, void * buffer, int sz) {
    for (;;) {
        int n = socket_recv(pipefd, buffer, sz);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            RETNIL("socket-server : read pipe error.");
        }

        // must atomic read from a pipe
        assert(n == sz);
        return;
    }
}

static bool has_cmd(struct socket_server * ss) {
    struct timeval tv = {0,0};

    FD_SET(ss->recvctrl_fd, &ss->rfds);
    
    int retval = select(ss->recvctrl_fd+1, &ss->rfds, NULL, NULL, &tv);
    return retval == 1;
}

static void add_udp_socket(struct socket_server * ss, struct request_udp * udp) {
    int id = udp->id;
    int protocol = udp->family == AF_INET6 ? PROTOCOL_UDPv6 : PROTOCOL_UDP;
    struct socket * s = new_fd(ss, id, udp->fd, protocol, udp->opaque, true);
    if (s == NULL) {
        close(udp->fd);
        ss->slot[HASH_ID(id)].type = SOCKET_TYPE_INVALID;
        return;
    }
    atomic_store(&s->type, SOCKET_TYPE_CONNECTED);
    memset(s->p.udp_address, 0, sizeof(s->p.udp_address));
}

static int set_udp_address(struct socket_server * ss, struct request_setudp * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        return -1;
    }

    int type = request->address[0];
    if (type != s->protocol) {
        // protocol mismatch
        return report_error(s, result, "protocol mismatch");
    }
    if (type == PROTOCOL_UDP) {
        memcpy(s->p.udp_address, request->address, 1+2+4); // 1 type, 2 port, 4 ipv4
    } else {
        memcpy(s->p.udp_address, request->address, 1+2+16); // 1 type, 2 port, 16 ipv6
    }
    atomic_fetch_add(&s->udpconnecting, 1);
    return -1;
}

static inline void inc_sending_ref(struct socket * s, int id) {
    if (s->protocol != PROTOCOL_TCP)
        return;

    for (;;) {
        unsigned long sending = atomic_load(&s->sending);
        if ((sending >> 16) == (unsigned long) ID_TAG16(id)) {
            if ((sending & 0xFFFF) == 0xFFFF) {
                // s->sending may overflow (rarely), so busy waiting here for socket thread dec it. see issue #794
                continue;
            }
            // inc sending only matching the same socket id
            if (atomic_compare_exchange_weak(&s->sending, &sending, sending + 1))
                return;
            // atom inc filed, retry
        } else {
            // socket id changed, just return
            return;
        }
    }
}

static inline void dec_sending_ref(struct socket_server * ss, int id) {
    struct socket * s = &ss->slot[HASH_ID(id)];
    // Notice: udp may inc sending while type == SOCKET_TYPE_RESERVE
    if (s->id == id && s->protocol == PROTOCOL_TCP) {
        assert((atomic_load(&s->sending) & 0xFFFF) != 0);
        atomic_fetch_sub(&s->sending, 1);
    }
}

// return type
static int ctrl_cmd(struct socket_server * ss, struct socket_message * result) {
    socket_t fd = ss->recvctrl_fd;
    // the length of message is one byte, so 256+8 buffer size is enough
    uint8_t buffer[256];
    uint8_t header[2];
    block_readpipe(fd, header, sizeof(header));
    int type = header[0];
    int len = header[1];
    block_readpipe(fd, buffer, len);
    // ctel command only exist in local fd, so don't worry about endian.
    switch (type) {
    case 'R':
        return resume_socket(ss, (struct request_resumepause *)buffer, result);
    case 'S':
        return pause_socket(ss, (struct request_resumepause *)buffer, result);
    case 'B':
        return bind_socket(ss, (struct request_bind *)buffer, result);
    case 'L':
        return listen_socket(ss, (struct request_listen *)buffer, result);
    case 'K':
        return close_socket(ss, (struct request_close *)buffer, result);
    case 'O':
        return open_socket(ss, (struct request_open *)buffer, result);
    case 'X':
        result->opaque = 0;
        result->id = 0;
        result->ud = 0;
        result->data = NULL;
        return SOCKET_EXIT;
    case 'W':
        return trigger_write(ss, (struct request_send *)buffer, result);
    case 'D':
    case 'P': {
        int priority = type == 'D' ? PRIORITY_HIGH : PRIORITY_LOW;
        struct request_send * request = (struct request_send *)buffer;
        int ret = send_socket(ss, request, result, priority, NULL);
        dec_sending_ref(ss, request->id);
        return ret;
    }
    case 'A': {
        struct request_send_udp * request = (struct request_send_udp *)buffer;
        return send_socket(ss, &request->send, result, PRIORITY_HIGH, request->address);
    }
    case 'C':
        return set_udp_address(ss, (struct request_setudp *)buffer, result);
    case 'T':
        setopt_socket(ss, (struct request_setopt *)buffer);
        return -1;
    case 'U':
        add_udp_socket(ss, (struct request_udp *)buffer);
        return -1;
    default:
        CERR("socket-server: Unknown ctrl %c.",type);
        return -1;
    }

    return -1;
}

// return -1 (ignore) when error
static int forward_message_tcp(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    int sz = s->p.size;
    char * buffer = malloc(sz);
    int n = socket_recv(s->fd, buffer, sz);
    if (n < 0) {
        free(buffer);
        switch (errno) {
        case EINTR:
        case AGAIN_WOULDBLOCK:
            break;
        default:
            return report_error(s, result, strerr(errno));
        }
        return -1;
    }

    if (n == 0) {
        free(buffer);
        if (s->closing) {
            // Rare case : if s->closing is true, reading event is disable, and SOCKET_CLOSE is raised.
            if (nomore_sending_data(s)) {
                force_close(ss, s, sl, result);
            }
            return -1;
        }

        int type = atomic_load(&s->type);
        if (type == SOCKET_TYPE_HALFCLOSE_READ) {
            // Rare case : Already shutdwon read.
            return -1;
        }
        if (type == SOCKET_TYPE_HALFCLOSE_WRITE) {
            // Remote shutdown read (write error) before.
            force_close(ss, s, sl, result);
        } else {
            close_read(ss, s, result);
        }

        return SOCKET_CLOSE;
    }

    if (halfclose_read(s)) {
        // discard recv data (Rare case : if socket is HALFCLOSE_READ, reading event is disable.)
        free(buffer);
        return -1;
    }

    stat_read(ss, s, n);

    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = n;
    result->data = buffer;

    if (n == sz) {
        s->p.size *= 2;
        return SOCKET_MORE;
    }
    if (sz > MIN_READ_BUFFER && n * 2 < sz) {
        s->p.size /= 2;
    }

    return SOCKET_DATA;
}

static int gen_udp_address(int protocol, union sockaddr_all * sa, uint8_t * udp_address) {
    int addrsz = 1;
    udp_address[0] = (uint8_t)protocol;
    if (protocol == PROTOCOL_UDP) {
        memcpy(udp_address+addrsz, &sa->v4.sin_port, sizeof(sa->v4.sin_port));
        addrsz += sizeof(sa->v4.sin_port);
        memcpy(udp_address+addrsz, &sa->v4.sin_addr, sizeof(sa->v4.sin_addr));
    } else {
        memcpy(udp_address+addrsz, &sa->v6.sin6_port, sizeof(sa->v6.sin6_port));
        addrsz += sizeof(sa->v6.sin6_port);
        memcpy(udp_address+addrsz, &sa->v6.sin6_addr, sizeof(sa->v6.sin6_addr));
        addrsz += sizeof(sa->v6.sin6_addr);
    }
    return addrsz;
}

static int forward_message_udp(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    union sockaddr_all sa;
    socklen_t sasz = sizeof(sa);
    int n = recvfrom(s->fd, ss->udpbuffer, MAX_UDP_PACKAGE, 0, &sa.s, &sasz);
    if (n < 0) {
        switch (errno) {
        case EINTR:
        case AGAIN_WOULDBLOCK:
            return -1;
        }
        int err = errno;
        // close when error
        force_close(ss, s, sl, result);
        result->data = (char *)strerr(err);
        return SOCKET_ERR;
    }

    stat_read(ss, s, n);

    uint8_t * data;
    if (sasz == sizeof(sa.v4)) {
        if (s->protocol != PROTOCOL_UDP)
            return -1;
        
        data = malloc(n + 1 + 2 + 4);
        gen_udp_address(PROTOCOL_UDP, &sa, data + n);
    } else {
        if (s->protocol != PROTOCOL_UDPv6)
            return -1;

        data = malloc(n + 1 + 2 + 4);
        gen_udp_address(PROTOCOL_UDPv6, &sa, data + n);
    }
    memcpy(data, ss->udpbuffer, n);

    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = n;
    result->data = (char *)data;

    return SOCKET_UDP;
}

static int report_connect(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    int err;
    socklen_t len = sizeof (err);
    int code = getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (code < 0 || err) {
        err = code >= 0 ? err : errno;
        force_close(ss, s, sl, result);
        result->data = (char *)strerr(err);
        return SOCKET_ERR;
    }

    atomic_store(&s->type, SOCKET_TYPE_CONNECTED);
    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = 0;
    if (nomore_sending_data(s)) {
        if (enable_write(ss, s, false)) {
            force_close(ss, s, sl, result);
            result->data = "disable write failed";
            return SOCKET_ERR;
        }
    }
    union sockaddr_all sa;
    socklen_t sasz = sizeof(sa);
    if (getpeername(s->fd, &sa.s, &sasz) == 0) {
        void * sin_addr = sa.s.sa_family == AF_INET ? (void*)&sa.v4.sin_addr : (void *)&sa.v6.sin6_addr;
        if (inet_ntop(sa.s.sa_family, sin_addr, ss->buffer, sizeof(ss->buffer))) {
            result->data = ss->buffer;
            return SOCKET_OPEN;
        }
    }
    result->data = NULL;
    return SOCKET_OPEN;
}

static int getname(union sockaddr_all * sa, char * buffer, size_t sz) {
    char temp[INET6_ADDRSTRLEN];
    void * sin_addr = sa->s.sa_family == AF_INET ? (void*)&sa->v4.sin_addr : (void *)&sa->v6.sin6_addr;
    if (inet_ntop(sa->s.sa_family, sin_addr, temp, sizeof(temp))) {
        int16_t sin_port = ntohs(sa->s.sa_family == AF_INET ? sa->v4.sin_port : sa->v6.sin6_port);
        snprintf(buffer, sz, "%s:%hu", temp, sin_port);
        return 1;
    }
    buffer[0] = '\0';
    return 0;
}

// return 0 when failed, or -1 when file limit
static int report_accept(struct socket_server * ss, struct socket * s, struct socket_message * result) {
    union sockaddr_all sa;
    socklen_t sasz = sizeof(union sockaddr_all);
    int client_fd = accept(s->fd, &sa.s, &sasz);
    if (client_fd < 0) {
        switch (errno) {
        case EMFILE:
        case ENFILE:
            result->opaque = s->opaque;
            result->id = s->id;
            result->ud = 0;
            result->data = (char *)strerr(errno);
            return -1;
        }
        return 0;
    }

    int id = reserve_id(ss);
    if (id < 0) {
        socket_close(client_fd);
        return 0;
    }

    socket_set_keepalive(client_fd);
    socket_set_nonblock(client_fd);

    struct socket * ns = new_fd(ss, id, client_fd, PROTOCOL_TCP, s->opaque, false);
    if (s == NULL) {
        close(client_fd);
        return 0;
    }

    // accept new one connection
    stat_read(ss, s, 1);

    atomic_store(&ns->type, SOCKET_TYPE_PACCEPT);
    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = id;
    result->data = NULL;

    if (getname(&sa, ss->buffer, sizeof(ss->buffer))) {
        result->data = ss->buffer;
    }

    return 1;
}

static void clear_closed_event(struct socket_server * ss, struct socket_message * result, int type) {
    if (type == SOCKET_CLOSE || type == SOCKET_ERR) {
        int id = result->id;
        for (int i = ss->event_index; i < ss->event_n; i++) {
            struct spoll_event * e = &ss->ev[i];
            struct socket * s = e->u;
            if (s) {
                if (socket_invalid(s, id) && s->id == id) {
                    e->u = NULL;
                    break;
                }
            }
        }
    }
}

// return type
int 
socket_server_poll(struct socket_server * ss, struct socket_message * result, int * more) {
    for (;;) {
        if (ss->checkctrl) {
            if (has_cmd(ss)) {
                int type = ctrl_cmd(ss, result);
                if (type != -1) {
                    clear_closed_event(ss, result, type);
                    return type;
                }
                continue;
            } else {
                ss->checkctrl = 0;
            }
        }

        if (ss->event_index == ss->event_n) {
            ss->event_n = spoll_wait(ss->event_fd, ss->ev);
            ss->checkctrl = 1;
            if (more) {
                *more = 0;
            }
            ss->event_index = 0;
            if (ss->event_n <= 0) {
                ss->event_n = 0;
                if (errno != EINTR) {
                    CERR("socket-server error");
                }
                continue;
            }
        }

        struct spoll_event * e = &ss->ev[ss->event_index++];
        struct socket * s = e->u;
        if (s == NULL) {
            // dispath pipe message at beginning
            continue;
        }

        struct socket_lock sl;
        socket_lock_init(&sl, s);
        switch (atomic_load(&s->type)) {
        case SOCKET_TYPE_CONNECTING:
            return report_connect(ss, s, &sl, result);
        case SOCKET_TYPE_LISTEN: {
            int ok = report_accept(ss, s, result);
            if (ok > 0) {
                return SOCKET_ACCEPT;
            }
            if (ok < 0) {
                return SOCKET_ERR;
            }
            // when ok == 0, retry
            break;
        }
        case SOCKET_TYPE_INVALID:
            CERR("socket-server: invalid socket");
            break;
        default:
            if (e->read) {
                int type;
                if (s->protocol == PROTOCOL_TCP) {
                    type = forward_message_tcp(ss, s, &sl, result);
                    if (type == SOCKET_MORE) {
                        --ss->event_index;
                        return SOCKET_DATA;
                    }
                } else {
                    type = forward_message_udp(ss, s, &sl, result);
                    if (type == SOCKET_UDP) {
                        // try read again
                        --ss->event_index;
                        return SOCKET_UDP;
                    }
                }

                if (e->write && type != SOCKET_CLOSE && type != SOCKET_ERR) {
                    // Try to dispath write message next step if write flag set.
                    e->read = false;
                    --ss->event_index;
                }

                if (type == -1)
                    break;
                return type;
            }

            if (e->write) {
                int type = send_buffer(ss, s, &sl, result);
                if (type == -1)
                    break;
                return type;
            }

            if (e->error) {
                int error;
                socklen_t len = sizeof(error);
                int code = getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &error, &len);
                const char * err = NULL;
                if (code < 0) {
                    err = strerr(errno);
                } else if (error != 0) {
                    err = strerr(error);
                } else {
                    err = "Unknown error";
                }
                return report_error(s, result, err);
            }

            if (e->eof) {
                // For epoll (at least), FIN packets are exchanged both ways.
                // See: https://stackoverflow.com/questions/52976152/tcp-when-is-epollhup-generated
                int halfclose = halfclose_read(s);
                force_close(ss, s, &sl, result);
                if (!halfclose) {
                    return SOCKET_CLOSE;
                }
            }

            break;
        }
    }
}

static void send_request(struct socket_server * ss, struct request_package * request, char type, int len) {
    request->header[6] = (uint8_t)type;
    request->header[7] = (uint8_t)len;
    for (;;) {
        int n = socket_send(ss->sendctrl_fd, &request->header[6], len+2);
        if (n < 0) {
            if (errno != EINTR) {
                CERR("socket-server : send ctrl command error");
            }
            continue;
        }
        assert(n == len+2);
        return;
    }
}

static int open_request(struct socket_server * ss, struct request_package * request, uintptr_t opaque, const char * addr, uint16_t port) {
    int len = strlen(addr);
    if (len + sizeof(request->u.open) >= 256) {
        CERR("socket-server : Invalid addr %s.", addr);
        return -1;
    }

    int id = reserve_id(ss);
    if (id < 0)
        return -1;
    request->u.open.opaque = opaque;
    request->u.open.id = id;
    request->u.open.port = port;
    memcpy(request->u.open.host, addr, len);
    request->u.open.host[len] = '\0';

    return len;
}

int 
socket_server_connect(struct socket_server * ss, uintptr_t opaque, const char * addr, uint16_t port) {
    struct request_package request;
    int len = open_request(ss, &request, opaque, addr, port);
    if (len < 0)
        return -1;
    send_request(ss, &request, '0', sizeof(request.u.open) + len);
    return request.u.open.id;
}

static inline int can_direct_write(struct socket * s, int id) {
    return s->id == id && nomore_sending_data(s) && atomic_load(&s->type) == SOCKET_TYPE_CONNECTED && atomic_load(&s->udpconnecting) == 0;
}

// return -1 when error, 0 when success
int
socket_server_send(struct socket_server * ss, struct socket_sendbuffer * buf) {
    int id = buf->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id) || s->closing) {
        free_buffer(ss, buf);
        return -1;
    }

    struct socket_lock sl;
    socket_lock_init(&sl, s);

    if (can_direct_write(s, id) && socket_trylock(&sl)) {
        // may be we can send directly, double check
        if (can_direct_write(s, id)) {
            // send directly
            struct send_object so;
            send_object_init_from_sendbuffer(ss, &so, buf);
            int n;
            if (s->protocol == PROTOCOL_TCP) {
                n = socket_send(s->fd, so.buffer, so.sz);
            } else {
                union sockaddr_all sa;
                socklen_t sasz = udp_socket_address(s, s->p.udp_address, &sa);
                if (sasz == 0) {
                    CERR("socket-server : set udp (%d) address first.", id);
                    socket_unlock(&sl);
                    so.free_func((void *)buf->buffer);
                    return -1;
                }
                n = sendto(s->fd, so.buffer, so.sz, 0, &sa.s, sasz);
            }
            if (n < 0) {
                // ignore error, let socket thread try again
                n = 0;
            }
            stat_write(ss, s, n);
            if (n == so.sz) {
                //  write done
                socket_unlock(&sl);
                so.free_func((void *)buf->buffer);
                return 0;
            }
            // write failed, put buffer into s->dw_*, and let socket thread send it. see send_buffer()
            s->dw_buffer = socket_sendbuffer_clone(buf, &s->dw_size);
            s->dw_offset = n;

            socket_unlock(&sl);

            inc_sending_ref(s, id);

            struct request_package request;
            request.u.send.id = id;
            request.u.send.sz = 0;
            request.u.send.buffer = NULL;

            // let socket thread enable write event
            send_request(ss, &request, 'W', sizeof(request.u.send));

            return 0;
        }

        socket_unlock(&sl);
    }

    inc_sending_ref(s, id);

    struct request_package request;
    request.u.send.id = id;
    request.u.send.buffer = socket_sendbuffer_clone(buf, &request.u.send.sz);

    send_request(ss, &request, 'D', sizeof(request.u.send));
    return 0;
}

// return -1 when error, 0 when success
int socket_server_send_lowpriority(struct socket_server * ss, struct socket_sendbuffer * buf) {
    int id = buf->id;

    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        free_buffer(ss, buf);
        return -1;
    }

    inc_sending_ref(s, id);

    struct request_package request;
    request.u.send.id = id;
    request.u.send.buffer = socket_sendbuffer_clone(buf, &request.u.send.sz);

    send_request(ss, &request, 'P', sizeof(request.u.send));
    return 0;
}

void
socket_server_exit(struct socket_server * ss) {
    struct request_package request;
    send_request(ss, &request, 'X', 0);
}

void
socket_server_close(struct socket_server * ss, uintptr_t opaque, int id) {
    struct request_package request;
    request.u.close.id = id;
    request.u.close.shutdown = 0;
    request.u.close.opaque = opaque;
    send_request(ss, &request, 'K', sizeof(request.u.close));
}

void
socket_server_shutdown(struct socket_server * ss, uintptr_t opaque, int id) {
    struct request_package request;
    request.u.close.id = id;
    request.u.close.shutdown = 1;
    request.u.close.opaque = opaque;
    send_request(ss, &request, 'K', sizeof(request.u.close));
}

// return -1 means failed
// or return AF_INET or AF_INET6
static socket_t do_bind(const char * host, uint16_t port, int protocol, int * family) {
    socket_t fd;
    int status;
    struct addrinfo ai_hints;
    struct addrinfo * ai_list = NULL;
    char portstr[6];
    if (host == NULL || *host == 0) {
        host = NULL; // INADDR_ANY
    }
    sprintf(portstr, "%hu", port);
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_flags = AI_PASSIVE;
    ai_hints.ai_family = AF_UNSPEC;
    if (protocol == IPPROTO_TCP) {
        ai_hints.ai_socktype = SOCK_STREAM;
    } else {
        assert(protocol == IPPROTO_UDP);
        ai_hints.ai_socktype = SOCK_DGRAM;
    }
    ai_hints.ai_protocol = protocol;

    status = getaddrinfo(host, portstr, &ai_hints, &ai_list);
    if (status != 0) {
        return INVALID_SOCKET;
    }

    *family = ai_list->ai_family;
    fd = socket(ai_list->ai_family, ai_list->ai_socktype, ai_list->ai_protocol);
    if (fd == INVALID_SOCKET) {
        goto err_failed_fd;
    }

    if (socket_set_reuse(fd)) {
        goto err_failed;
    }

    status = bind(fd, ai_list->ai_addr, ai_list->ai_addrlen);
    if (status != 0) {
        goto err_failed;
    }

    freeaddrinfo(ai_list);
    return fd;

err_failed:
    close(fd);
err_failed_fd:
    freeaddrinfo(ai_list);
    return INVALID_SOCKET;
}

static socket_t do_listen(const char * host, uint16_t port, int backlog) {
    int family = 0;
    socket_t listen_fd = do_bind(host, port, IPPROTO_TCP, &family);
    if (listen_fd == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }
    if (listen(listen_fd, backlog) == -1) {
        close(listen_fd);
        return INVALID_SOCKET;
    }
    return listen_fd;
}

int
socket_server_listen(struct socket_server * ss, uintptr_t opaque, const char * addr, uint16_t port, int backlog) {
    socket_t fd = do_listen(addr, port, backlog);
    if (fd == INVALID_SOCKET) {
        return -1;
    }

    int id = reserve_id(ss);
    if (id < 0) {
        socket_close(fd);
        return id;
    }

    struct request_package request;
    request.u.listen.opaque = opaque;
    request.u.listen.fd = fd;
    send_request(ss, &request, 'L', sizeof(request.u.listen));

    return id;
}

int
socket_server_bind(struct socket_server * ss, uintptr_t opaque, socket_t fd) {
    struct request_package request;
    int id = reserve_id(ss);
    if (id < 0)
        return -1;
    request.u.bind.opaque = opaque;
    request.u.bind.id = id;
    request.u.bind.fd = fd;
    send_request(ss, &request, 'B', sizeof(request.u.bind));
    return id;
}

void
socket_server_start(struct socket_server * ss, uintptr_t opaque, int id) {
    struct request_package request;
    request.u.resumepause.id = id;
    request.u.resumepause.opaque = opaque;
    send_request(ss, &request, 'R', sizeof(request.u.resumepause));
}

void
socket_server_pause(struct socket_server * ss, uintptr_t opaque, int id) {
    struct request_package request;
    request.u.resumepause.id = id;
    request.u.resumepause.opaque = opaque;
    send_request(ss, &request, 'S', sizeof(request.u.resumepause));
}

void
socket_server_userobject(struct socket_server * ss, struct socket_object_interface * soi) {
    ss->soi = *soi;
}

// UDP

int
socket_server_udp(struct socket_server * ss, uintptr_t opaque, const char * addr, uint16_t port) {
    socket_t fd;
    int family;
    if (port != 0 || addr != NULL) {
        // bind
        fd = do_bind(addr, port, IPPROTO_UDP, &family);
    } else {
        family = AF_INET;
        fd = socket(family, SOCK_DGRAM, 0);
    }
    if (fd == INVALID_SOCKET) {
        return -1;
    }
    socket_set_nonblock(fd);

    int id = reserve_id(ss);
    if (id < 0) {
        socket_close(fd);
        return -1;
    }

    struct request_package request;
    request.u.udp.id = id;
    request.u.udp.fd = fd;
    request.u.udp.opaque = opaque;
    request.u.udp.family = family;

    send_request(ss, &request, 'U', sizeof(request.u.udp));
    return id;
}

int
socket_server_udp_send(struct socket_server * ss, const struct socket_udp_address * addr, struct socket_sendbuffer * buf) {
    int id = buf->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        free_buffer(ss, buf);
        return -1;
    }

    const uint8_t * udp_address = (const uint8_t *)addr;
    int addrsz;
    switch (udp_address[0]) {
    case PROTOCOL_UDP:
        addrsz = 1+2+4;     // 1 type, 2 port, 4 ipv4
        break;
    case PROTOCOL_UDPv6:
        addrsz = 1+2+16;    // 1 type, 2 port, 16 ipv6
        break;
    default:
        free_buffer(ss, buf);
        return -1;
    }

    struct socket_lock sl;
    socket_lock_init(&sl, s);

    if (can_direct_write(s, id) && socket_trylock(&sl)) {
        // may be we can send directly, double check
        if (can_direct_write(s, id)) {
            // send directly
            struct send_object so;
            send_object_init_from_sendbuffer(ss, &so, buf);
            union sockaddr_all sa;
            socklen_t sasz = udp_socket_address(s, udp_address, &sa);
            if (sasz == 0) {
                socket_unlock(&sl);
                so.free_func((void *)buf->buffer);
                return -1;
            }
            int n = sendto(s->fd, so.buffer, so.sz, 0, &sa.s, sasz);
            if (n >= 0) {
                // sendto succ
                stat_write(ss, s, n);
                socket_unlock(&sl);
                so.free_func((void *)buf->buffer);
                return 0;
            }
        }
        socket_unlock(&sl);
        // let socket thread try again, udp doesn't care the order
    }

    struct request_package request;
    request.u.send_udp.send.id = id;
    request.u.send_udp.send.buffer = socket_sendbuffer_clone(buf, &request.u.send_udp.send.sz);
    memcpy(request.u.send_udp.address, udp_address, addrsz);
    send_request(ss, &request, 'A', sizeof(request.u.send_udp.send)+addrsz);
    return 0;
}

int
socket_server_udp_connect(struct socket_server * ss, int id, const char * addr, uint16_t port) {
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id)) {
        return -1;
    }

    struct socket_lock sl;
    socket_lock_init(&sl, s);
    socket_lock(&sl);
    if (socket_invalid(s, id)) {
        socket_unlock(&sl);
        return -1;
    }

    atomic_fetch_add(&s->udpconnecting, 1);
    socket_unlock(&sl);

    int status;
    struct addrinfo ai_hints;
    struct addrinfo * ai_list = NULL;
    char portstr[6];
    sprintf(portstr, "%hu", port);
    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_DGRAM;
    ai_hints.ai_protocol = IPPROTO_UDP;

    status = getaddrinfo(addr, portstr, &ai_hints, &ai_list);
    if (status != 0) {
        return -1;
    }

    struct request_package request;
    request.u.set_udp.id = id;
    int protocol;

    if (ai_list->ai_family == AF_INET) {
        protocol = PROTOCOL_UDP;
    } else if (ai_list->ai_family == AF_INET6) {
        protocol = PROTOCOL_UDPv6;
    } else {
        freeaddrinfo(ai_list);
        return -1;
    }

    int addrsz = gen_udp_address(protocol, (union sockaddr_all *)ai_list->ai_addr, request.u.send_udp.address);
    
    freeaddrinfo(ai_list);

    send_request(ss, &request, 'C', sizeof(request.u.set_udp) - sizeof(request.u.set_udp.address) + addrsz);

    return 0;
}

const struct socket_udp_address *
socket_server_udp_address(struct socket_server * ss, struct socket_message * msg, int * addrsz) {
    (void) ss;
    uint8_t * address = (uint8_t *)(msg->data + msg->ud);
    int type = address[0];
    switch (type) {
    case PROTOCOL_UDP:
        *addrsz = 1+2+4;
        break;
    case PROTOCOL_UDPv6:
        *addrsz = 1+2+16;
        break;
    default:
        return NULL;
    }
    return (const struct socket_udp_address *)address;
}

static bool query_info(struct socket * s, struct socket_info * si) {
    union sockaddr_all sa;
    socklen_t sasz = sizeof(union sockaddr_all);
    int type = atomic_load(&s->type);
    switch (type) {
    case SOCKET_TYPE_BIND:
        si->type = SOCKET_INFO_BIND;
        si->name[0] = 0;
        break;
    case SOCKET_TYPE_LISTEN:
        si->type = SOCKET_INFO_LISTEN;
        if (getsockname(s->fd, &sa.s, &sasz) == 0) {
            getname(&sa, si->name, sizeof(si->name));
        }
        break;
    case SOCKET_TYPE_HALFCLOSE_READ:
    case SOCKET_TYPE_HALFCLOSE_WRITE:
    case SOCKET_TYPE_CONNECTED:
        if (s->protocol == PROTOCOL_TCP) {
            si->type = type == SOCKET_TYPE_CONNECTED ? SOCKET_INFO_TCP : SOCKET_INFO_CLOSEING;
            if (getpeername(s->fd, &sa.s, &sasz) == 0) {
                getname(&sa, si->name, sizeof(si->name));
            }
        } else {
            si->type = SOCKET_INFO_UDP;
            if (udp_socket_address(s, s->p.udp_address, &sa)) {
                getname(&sa, si->name, sizeof(si->name));
            }
        }
        break;
    default:
        return false;
    }

    si->id = s->id;
    si->opaque = s->opaque;
    si->read = s->stat.read;
    si->write = s->stat.write;
    si->rtime = s->stat.rtime;
    si->wtime = s->stat.wtime;
    si->wbuffer = s->wb_size;
    si->reading = s->reading;
    si->writing = s->writing;

    return true;
}

struct socket_info *
socket_server_info(struct socket_server * ss) {
    struct socket_info * si = NULL;
    for (int i = 0; i < MAX_SOCKET; i++) {
        struct socket * s = &ss->slot[i];
        int id = s->id;
        
        struct socket_info temp;
        if (query_info(s, &temp) && s->id == id) {
            // socket_server_info may call in different thread, so check socket id again
            si = socket_info_create(si);
            temp.next = si->next;
            *si = temp;
        }
    }
    return si;
}
