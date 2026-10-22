#include <stdio.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#if BUFFER_SIZE < 1
#error "BUFFER_SIZE must be at least 1"
#endif

int main(int argc, char *argv[])
{
    unsigned char buffer[BUFFER_SIZE];
    FILE *input;
    int status = EXIT_SUCCESS;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "rb");
    if (input == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    while (fread(buffer, 1, BUFFER_SIZE, input) > 0) {
        /* Reading is the work being measured; successful runs print nothing. */
    }

    if (ferror(input)) {
        fprintf(stderr, "fread: failed while reading '%s'\n", argv[1]);
        status = EXIT_FAILURE;
    }

    if (fclose(input) == EOF) {
        perror("fclose");
        status = EXIT_FAILURE;
    }

    return status;
}
