#include "chat_server.h"

static void
usage(const char *prog) {
    fprintf(stderr, "usage: %s [bind_ip] [port] [max_clients]\n", prog);
}

int
main(int argc, char **argv) {
#if defined(_WIN32)
    char ** utf8_argv = nullptr;
    int utf8_argc = 0;
    int exit_code;
    int rc = platform_get_utf8_argv(&utf8_argc, &utf8_argv);
#endif
    chat_server_config_t cfg;

#if defined(_WIN32)
    if (rc < 0) {
        return 2;
    }
    argc = utf8_argc;
    argv = utf8_argv;
#endif

    cfg.bind_ip = "0.0.0.0";
    cfg.bind_port = 9000;
    cfg.max_clients = 128;

    if (argc > 4) {
        usage(argv[0]);
#if defined(_WIN32)
        platform_free_utf8_argv(utf8_argc, utf8_argv);
#endif
        return 1;
    }
    if (argc >= 2) {
        cfg.bind_ip = argv[1];
    }
    if (argc >= 3) {
        long port = strtol(argv[2], nullptr, 10);
        if (port <= 0 || port > 65535) {
            usage(argv[0]);
#if defined(_WIN32)
            platform_free_utf8_argv(utf8_argc, utf8_argv);
#endif
            return 1;
        }
        cfg.bind_port = (uint16_t)port;
    }
    if (argc >= 4) {
        long max_clients = strtol(argv[3], nullptr, 10);
        if (max_clients <= 0 || max_clients > CLIENT_MAX) {
            usage(argv[0]);
#if defined(_WIN32)
            platform_free_utf8_argv(utf8_argc, utf8_argv);
#endif
            return 1;
        }
        cfg.max_clients = (size_t)max_clients;
    }

#if defined(_WIN32)
    exit_code = chat_server_run(&cfg) == 0 ? 0 : 2;
    platform_free_utf8_argv(utf8_argc, utf8_argv);
    return exit_code;
#else
    return chat_server_run(&cfg) == 0 ? 0 : 2;
#endif
}


