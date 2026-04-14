#include "utf8.h"

int
utf8_validate_and_count(const char * text, size_t text_len, size_t * char_count) {
    size_t idx = 0;
    size_t count = 0;

    if (text == nullptr) {
        return -EINVAL;
    }

    while (idx < text_len) {
        unsigned char ch = (unsigned char)text[idx];
        size_t need = 0;
        uint32_t code = 0;

        if (ch <= 0x7F) {
            need = 1;
            code = ch;
        } else if (ch >= 0xC2 && ch <= 0xDF) {
            need = 2;
            code = (uint32_t)(ch & 0x1F);
        } else if (ch >= 0xE0 && ch <= 0xEF) {
            need = 3;
            code = (uint32_t)(ch & 0x0F);
        } else if (ch >= 0xF0 && ch <= 0xF4) {
            need = 4;
            code = (uint32_t)(ch & 0x07);
        } else {
            return -EILSEQ;
        }

        if (idx + need > text_len) {
            return -EILSEQ;
        }

        if (need >= 2) {
            unsigned char b1 = (unsigned char)text[idx + 1];
            if ((b1 & 0xC0) != 0x80) {
                return -EILSEQ;
            }
            if (ch == 0xE0 && b1 < 0xA0) {
                return -EILSEQ;
            }
            if (ch == 0xED && b1 >= 0xA0) {
                return -EILSEQ;
            }
            if (ch == 0xF0 && b1 < 0x90) {
                return -EILSEQ;
            }
            if (ch == 0xF4 && b1 >= 0x90) {
                return -EILSEQ;
            }
            code = (code << 6U) | (uint32_t)(b1 & 0x3F);
        }

        if (need >= 3) {
            unsigned char b2 = (unsigned char)text[idx + 2];
            if ((b2 & 0xC0) != 0x80) {
                return -EILSEQ;
            }
            code = (code << 6U) | (uint32_t)(b2 & 0x3F);
        }

        if (need >= 4) {
            unsigned char b3 = (unsigned char)text[idx + 3];
            if ((b3 & 0xC0) != 0x80) {
                return -EILSEQ;
            }
            code = (code << 6U) | (uint32_t)(b3 & 0x3F);
        }

        if (code > 0x10FFFFU || (code >= 0xD800U && code <= 0xDFFFU)) {
            return -EILSEQ;
        }

        idx += need;
        ++count;
    }

    if (char_count != nullptr) {
        *char_count = count;
    }
    return 0;
}

