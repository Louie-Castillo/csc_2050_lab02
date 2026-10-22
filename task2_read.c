#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#if BUFFER_SIZE < 1
#error "BUFFER_SIZE must be at least 1"
#endif

int main(int argc, char *argv[])
{
    unsigned char buffer[BUFFER_SIZE];
    int file_descriptor;
    int status = EXIT_SUCCESS;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    file_descriptor = open(argv[1], O_RDONLY);
    if (file_descriptor == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    for (;;) {
        ssize_t bytes_read = read(file_descriptor, buffer, BUFFER_SIZE);

        if (bytes_read > 0) {
            continue;
        }

        if (bytes_read == 0) {
            break;
        }

        if (errno == EINTR) {
            continue;
        }

        perror("read");
        status = EXIT_FAILURE;
        break;
    }

    if (close(file_descriptor) == -1) {
        perror("close");
        status = EXIT_FAILURE;
    }

    return status;
}
