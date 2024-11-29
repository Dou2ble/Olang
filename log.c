#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <term.h>

#ifdef _WIN32
#include <io.h> // For _isatty()
#define isatty _isatty
#else
#include <unistd.h> // For isatty()
#endif

/* int main() {
    // Initialize the terminal
    int result = setupterm(NULL, fileno(stdout), (int *)0);
    if (result == ERR) {
        fprintf(stderr, "Failed to initialize terminal.\n");
        return EXIT_FAILURE;
    }

    // Check if the terminal supports colors
    int colors = tigetnum("colors");
    if (colors > 0) {
        printf("This terminal supports %d colors.\n", colors);
    } else {
        printf("This terminal does not support colors.\n");
    }

    return EXIT_SUCCESS;
} */

/* int main() {
    // Check if stdout is a terminal
    if (isatty(fileno(stdout))) {
        printf("Standard output is a terminal.\n");
    } else {
        printf("Standard output is NOT a terminal.\n");
    }

    return 0;
} */
