/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_TEXT_ENCODING_H
#define MPII3_TEXT_ENCODING_H
#include <stddef.h>
#include <wchar.h>
/* Converts filenames safely without depending on the console's C locale. */
size_t text_decode_utf8(const char *input, wchar_t *output, size_t capacity);
#endif
