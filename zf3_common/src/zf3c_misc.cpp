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
        const long fileSize = ftell(fs);
        fseek(fs, 0, SEEK_SET);

        // Allocate memory to store the file contents.
        const int contentsSize = fileSize + 1; // Accounts for '\0'.
        const auto contents = alloc<char>(contentsSize);

        if (!contents) {
            fclose(fs);
            return nullptr;
        }

        // Read the contents into the buffer.
        fread(contents, 1, contentsSize - 1, fs);
        contents[contentsSize - 1] = '\0';

        fclose(fs);

        return contents;
    }
}
