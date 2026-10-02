#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

int main() {
    nio_console console;

    // Initialize console with screen dimensions and colors
    // Parameters: console pointer, width, height, background color, foreground color
    if (!nio_init(&console, 320, 240, 0x0000, 0xFFFF)) {
        return 1;
    }

    // Clear the screen
    nio_clear(&console);

    // Draw text
    nio_printf(&console, "%s\n", GAME_TITLE);
    nio_printf(&console, "Engine: C++ & Python Pipeline\n");
    nio_printf(&console, "Press any key on calculator to exit.\n");

    // Wait for key press
    wait_key_pressed();

    return 0;
}
