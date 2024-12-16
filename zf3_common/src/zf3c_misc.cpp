#include <zf3c_misc.h>

namespace zf3 {
    char* get_file_contents(const char* const filename) {
        // Open the file.
        FILE* const fs = fopen(filename, "rb");

        if (!fs) {
            return nullptr;
        }

        // Get the file size.
        fseek(fs, 0, SEEK_END);
        const long file_size = ftell(fs);
        fseek(fs, 0, SEEK_SET);

        // Allocate memory to store the file contents.
        char* const contents = static_cast<char*>(malloc(file_size + 1));

        if (!contents) {
            fclose(fs);
            return nullptr;
        }

        // Read the contents into the buffer.
        fread(contents, 1, file_size, fs);
        contents[file_size] = '\0';

        fclose(fs);

        return contents;
    }
}
