/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "input.h"
#include <ogc/lwp_watchdog.h>
#include <wiiuse/wpad.h>
static u64 next_repeat;
void input_init(void) {
    WPAD_Init();
}
Input input_read(void) {
    WPAD_ScanPads();
    u32 down = WPAD_ButtonsDown(0), held = WPAD_ButtonsHeld(0);
    Input i = {0};
    u64 now = ticks_to_millisecs(gettime());
    u32 nav = WPAD_BUTTON_UP | WPAD_BUTTON_DOWN;
    u32 repeat = down;
    if (down & nav)
        next_repeat = now + 350;
    else if ((held & nav) && now >= next_repeat) {
        repeat |= held & nav;
        next_repeat = now + 100;
    }
    i.selection = (repeat & WPAD_BUTTON_DOWN ? 1 : 0) - (repeat & WPAD_BUTTON_UP ? 1 : 0);
    i.skip = (down & WPAD_BUTTON_RIGHT ? 1 : 0) - (down & WPAD_BUTTON_LEFT ? 1 : 0);
    i.volume = (down & WPAD_BUTTON_PLUS ? 15 : 0) - (down & WPAD_BUTTON_MINUS ? 15 : 0);
    i.activate = down & WPAD_BUTTON_A;
    i.library = down & WPAD_BUTTON_1;
    i.loop = down & WPAD_BUTTON_2;
    i.visualizer = down & WPAD_BUTTON_B;
    i.exit = down & WPAD_BUTTON_HOME;
    return i;
}
