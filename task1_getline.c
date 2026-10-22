#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static void report_read_error(const char *program_name, const char *path)
{
    if (errno != 0) {
        fprintf(stderr, "%s: could not read '%s': %s\n",
                program_name, path, strerror(errno));
    } else {
        fprintf(stderr, "%s: could not read '%s'\n", program_name, path);
    }
}

static void report_output_error(const char *program_name)
{
    if (errno != 0) {
        fprintf(stderr, "%s: could not write output: %s\n",
                program_name, strerror(errno));
    } else {
        fprintf(stderr, "%s: could not write output\n", program_name);
    }
}

int main(int argc, char *argv[])
{
    FILE *input = NULL;
    char *line = NULL;
    size_t capacity = 0;
    size_t line_count = 0;
    size_t line_index = 0;
    size_t first_line_to_print;
    ssize_t length;
    int status = EXIT_FAILURE;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "%s: could not open '%s': %s\n",
                argv[0], argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    /* First pass: count every line while getline manages the buffer. */
    for (;;) {
        errno = 0;
        length = getline(&line, &capacity, input);

        if (length == -1) {
            break;
        }

        ++line_count;
    }

    if (!feof(input)) {
        report_read_error(argv[0], argv[1]);
        goto cleanup;
    }

    /* The lab explicitly permits rewind for a second pass. */
    errno = 0;
    rewind(input);
    if (errno != 0) {
        fprintf(stderr, "%s: could not rewind '%s': %s\n",
                argv[0], argv[1], strerror(errno));
        goto cleanup;
    }

    first_line_to_print = line_count > 2 ? line_count - 2 : 0;

    /* Second pass: reproduce the final two available lines exactly. */
    for (;;) {
        errno = 0;
        length = getline(&line, &capacity, input);

        if (length == -1) {
            break;
        }

        if (line_index >= first_line_to_print) {
            size_t bytes_to_write = (size_t)length;

            if (fwrite(line, 1, bytes_to_write, stdout) != bytes_to_write) {
                report_output_error(argv[0]);
                goto cleanup;
            }
        }

        ++line_index;
    }

    if (!feof(input)) {
        report_read_error(argv[0], argv[1]);
        goto cleanup;
    }

    errno = 0;
    if (fflush(stdout) == EOF) {
        report_output_error(argv[0]);
        goto cleanup;
    }

    status = EXIT_SUCCESS;

cleanup:
    free(line);

    if (fclose(input) == EOF && status == EXIT_SUCCESS) {
        fprintf(stderr, "%s: could not close '%s': %s\n",
                argv[0], argv[1], strerror(errno));
        status = EXIT_FAILURE;
    }

    return status;
}
