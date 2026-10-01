#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests
${HOST_CC:-cc} -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -g \
  -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude \
  tests/test_core.c source/discovery.c source/playlist.c source/path.c source/queue.c source/spectrum.c source/text_encoding.c source/settings.c source/config.c \
  -lm -o build/tests/core
./build/tests/core
