#include <zf3c_misc.h>

namespace zf3 {

bool init_mem_arena(MemArena* const memArena, const int size) {
    memset(memArena, 0, sizeof(*memArena));

    memArena->buf = static_cast<zf3::Byte*>(malloc(size));

    if (!memArena->buf) {
        return false;
    }

    memset(memArena->buf, 0, size);

    memArena->size = size;

    return true;
}

void clean_mem_arena(MemArena* const memArena) {
    free(memArena->buf);
    memset(memArena, 0, sizeof(*memArena));
}

void reset_mem_arena(MemArena* const memArena) {
    memset(memArena->buf, 0, memArena->used);
    memArena->used = 0;
}

char* get_file_contents(const char* const filename) {
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
    char* const contents = static_cast<char*>(malloc(file_size + 1));

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

}
