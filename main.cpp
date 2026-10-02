#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

int main() {
    nio_console console;

    // Initialize console with all required parameters
    // Parameters: console pointer, width, height, offset_x, offset_y, background_color, foreground_color, drawing_enabled
    if (!nio_init(&console, 320, 240, 0, 0, 0x0000, 0xFFFF, false)) {
        return 1;
    }

    // Clear the screen
    nio_clear(&console);

    // Draw text using nio_printf_at or the correct API
    nio_printf("%s\n", GAME_TITLE);
    nio_printf("Engine: C++ & Python Pipeline\n");
    nio_printf("Press any key on calculator to exit.\n");

    // Wait for key press
    wait_key_pressed();

    return 0;
}
