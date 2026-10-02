#include <libndls.h>
#include <nspireio/nspireio.h>

// Required by the linker for proper cleanup
extern "C" void _fini(void) {}

int main() {
    assert_ndless();

    // Correct console initialization structure
    nio_console console;
    nio_init(&console);
    nio_clean(&console);

    // Standard nio_printf takes standard printf parameters
    nio_printf("\n\nCeleste Engine Ready\n");
    nio_printf("Press any key to exit\n");

    // Wait for a key press
    wait_key();

    nio_free(&console);
    return 0;
}
