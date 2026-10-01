/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "queue.h"
int queue_next(size_t count, int current, int direction, LoopMode loop, int automatic) {
    if (!count || current < 0 || (size_t)current >= count)
        return -1;
    if (automatic && loop == LOOP_ONE)
        return current;
    int next = current + (direction < 0 ? -1 : 1);
    if (next >= 0 && (size_t)next < count)
        return next;
    if (loop == LOOP_ALL)
        return next < 0 ? (int)count - 1 : 0;
    return -1;
}
