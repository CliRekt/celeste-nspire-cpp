#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

// Required by the linker for proper cleanup
extern "C" void _fini(void) {}

int main() {
    nio_console console;

    // Initialize console with all required parameters
    if (!nio_init(&console, 320, 240, 0, 0, 0x0000, 0xFFFF, false)) {
        return 1;
    }

    // Clear the screen
    nio_clear(&console);

    // Draw text
    nio_printf("%s\n", GAME_TITLE);
    nio_printf("Engine: C++ & Python Pipeline\n");
    nio_printf("Press any key on calculator to exit.\n");

    // Wait for key press
    wait_key_pressed();

    return 0;
}
