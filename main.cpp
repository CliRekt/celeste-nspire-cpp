#include <libndls.h>
#include <nspireio/nspireio.h>
#include "assets.h"

// Required by the linker for proper cleanup
extern "C" void _fini(void) {}

int main() {
    // Initialize the screen safely before any drawing
    lcd_init(LCD_BUFFER_L);
    
    // Draw a simple black screen to confirm entry point is stable
    lcd_fillrect(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0000);
    
    // Simple status message using debug output
    draw_string(0, 0, "Celeste Engine Ready", COLOR_WHITE, COLOR_BLACK);
    draw_string(0, 20, "Press any key to exit", COLOR_WHITE, COLOR_BLACK);
    
    // Wait for a key press in a stable loop
    while (!any_key_pressed()) {
        // Simple main loop - minimal processing
        // Rendering confirmed to be stable at this point
    }
    
    // Clean shutdown
    lcd_shutdown();
    
    return 0;
}
