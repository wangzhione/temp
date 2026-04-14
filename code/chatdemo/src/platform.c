#include "platform.h"

#if defined(_WIN32)
int
platform_get_utf8_argv(int * argc_out, char *** argv_out) {
    LPWSTR * wide_argv;
    char ** utf8_argv;
    int argc;
    int idx;

    if (argc_out == nullptr || argv_out == nullptr) {
        return -EINVAL;
    }

    wide_argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (wide_argv == nullptr) {
        return -EINVAL;
    }

    utf8_argv = (char **)calloc((size_t)argc, sizeof(*utf8_argv));
    if (utf8_argv == nullptr) {
        LocalFree(wide_argv);
        return -ENOMEM;
    }

    for (idx = 0; idx < argc; ++idx) {
        int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_argv[idx], -1, nullptr, 0, nullptr, nullptr);

        if (utf8_len <= 0) {
            platform_free_utf8_argv(idx, utf8_argv);
            LocalFree(wide_argv);
            return -EINVAL;
        }

        utf8_argv[idx] = (char *)calloc((size_t)utf8_len, sizeof(*utf8_argv[idx]));
        if (utf8_argv[idx] == nullptr) {
            platform_free_utf8_argv(idx, utf8_argv);
            LocalFree(wide_argv);
            return -ENOMEM;
        }

        if (WideCharToMultiByte(CP_UTF8, 0, wide_argv[idx], -1, utf8_argv[idx], utf8_len, nullptr, nullptr) != utf8_len) {
            platform_free_utf8_argv(idx + 1, utf8_argv);
            LocalFree(wide_argv);
            return -EINVAL;
        }
    }

    LocalFree(wide_argv);
    *argc_out = argc;
    *argv_out = utf8_argv;
    return 0;
}

void
platform_free_utf8_argv(int argc, char ** argv) {
    int idx;

    if (argv == nullptr) {
        return;
    }

    for (idx = 0; idx < argc; ++idx) {
        free(argv[idx]);
    }
    free(argv);
}
#endif
