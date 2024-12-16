#include <zf3c_debugging.h>

namespace zf3 {
    void log(const char* const format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        printf("\n");
        va_end(args);
    }

    void log_error(const char* const format, ...) {
        va_list args;
        va_start(args, format);

        fprintf(stderr, "ERROR: ");
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");

        va_end(args);
    }
}
