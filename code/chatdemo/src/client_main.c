#include "chat_client.h"

static void
usage(const char *prog) {
    fprintf(stderr, "usage: %s <server_host> <server_port> <name> [local_port]\n", prog);
}

int
main(int argc, char **argv) {
#if defined(_WIN32)
    char ** utf8_argv = nullptr;
    int utf8_argc = 0;
    int exit_code;
    int rc = platform_get_utf8_argv(&utf8_argc, &utf8_argv);
#endif
    chat_client_config_t cfg;

#if defined(_WIN32)
    if (rc < 0) {
        return 2;
    }
    argc = utf8_argc;
    argv = utf8_argv;
#endif

    if (argc != 4 && argc != 5) {
        usage(argv[0]);
#if defined(_WIN32)
        platform_free_utf8_argv(utf8_argc, utf8_argv);
#endif
        return 1;
    }

    cfg.server_host = argv[1];
    cfg.server_port = (uint16_t)strtoul(argv[2], nullptr, 10);
    cfg.name = argv[3];
    cfg.local_port = 0;

    if (cfg.server_port == 0) {
        usage(argv[0]);
#if defined(_WIN32)
        platform_free_utf8_argv(utf8_argc, utf8_argv);
#endif
        return 1;
    }
    if (argc == 5) {
        cfg.local_port = (uint16_t)strtoul(argv[4], nullptr, 10);
    }

#if defined(_WIN32)
    exit_code = chat_client_run(&cfg) == 0 ? 0 : 2;
    platform_free_utf8_argv(utf8_argc, utf8_argv);
    return exit_code;
#else
    return chat_client_run(&cfg) == 0 ? 0 : 2;
#endif
}

