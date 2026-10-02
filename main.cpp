#include <libndls.h>
#include <nspireio/nspireio.h>

// Required by linker for bare-metal cleanup
extern "C" void _fini(void) {}

int main() {
    assert_ndless_rev(45);

    nio_console console;

    // nio_init expects: (&console, cols, rows, x_offset, y_offset, bg_color, fg_color, show_screen)
    // 0x0000 = Black, 0xFFFF = White (565 RGB format)
    nio_init(&console, 40, 30, 0, 0, 0x0000, 0xFFFF, true);
    nio_clear(&console);

    nio_printf("\n\n  Celeste Engine Ready!\n");
    nio_printf("  Press any key to exit...\n");

    wait_key_pressed();

    nio_free(&console);
    return 0;
}
