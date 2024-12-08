#include <zf3_common.h>

char* zf3_get_file_contents(const char* const filename) {
    // Open the file.
    FILE* const fs = fopen(filename, "rb");

    if (!fs) {
        return NULL;
    }

    // Get the file size.
    fseek(fs, 0, SEEK_END);
    const long file_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    // Allocate memory to store the file contents.
    char* const contents = malloc(file_size + 1);

    if (!contents) {
        fclose(fs);
        return NULL;
    }

    // Read the contents into the buffer.
    fread(contents, 1, file_size, fs);
    contents[file_size] = '\0';

    fclose(fs);

    return contents;
}
