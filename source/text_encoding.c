/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "text_encoding.h"
#include <stdint.h>
size_t text_decode_utf8(const char *input, wchar_t *output, size_t capacity) {
    if (!capacity)
        return 0;
    const unsigned char *p = (const unsigned char *)input;
    size_t length = 0;
    while (*p && length + 1 < capacity) {
        uint32_t code = *p;
        unsigned extra = 0;
        uint32_t minimum = 0;
        if (code >= 0xc2 && code <= 0xdf) {
            extra = 1;
            code &= 31;
            minimum = 0x80;
        } else if (code >= 0xe0 && code <= 0xef) {
            extra = 2;
            code &= 15;
            minimum = 0x800;
        } else if (code >= 0xf0 && code <= 0xf4) {
            extra = 3;
            code &= 7;
            minimum = 0x10000;
        } else if (code >= 0x80) {
            output[length++] = 0xfffd;
            p++;
            continue;
        }
        unsigned i;
        for (i = 0; i < extra; i++) {
            if (!p[i + 1] || (p[i + 1] & 0xc0) != 0x80)
                break;
            code = (code << 6) | (p[i + 1] & 63);
        }
        if (i != extra || code < minimum || code > 0x10ffff || (code >= 0xd800 && code <= 0xdfff)) {
            output[length++] = 0xfffd;
            p++;
            continue;
        }
        output[length++] = code < 32 ? ' ' : (wchar_t)code;
        p += extra + 1;
    }
    output[length] = 0;
    return length;
}
