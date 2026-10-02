#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

// Required by the linker for proper cleanup
extern "C" void _fini(void) {}

int main() {
    // Initialize nspireio for console output
    nio_console console;
    nio_init_console(&console);
    
    // Clear the console/screen
    nio_printf(&console, "\n\nCeleste Engine Ready\n");
    nio_printf(&console, "Press any key to exit\n");
    
    // Wait for a key press
    while (!any_key_pressed()) {
        // Simple main loop - minimal processing
    }
    
    // Close console
    nio_close_console(&console);
    
    return 0;
}
