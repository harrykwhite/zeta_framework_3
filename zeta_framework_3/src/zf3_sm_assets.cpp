#include <zf3_assets.h>

namespace zf3 {

static Assets* i_assets;

bool load_assets() {
    assert(!i_assets);

    FILE* const fs = fopen(gk_assetsFileName, "rb");

    if (!fs) {
        return false;
    }

    i_assets = static_cast<Assets*>(calloc(1, sizeof(*i_assets)));

    if (!i_assets) {
        return false;
    }

    const bool loadSucceeded = load_textures(i_assets->textures, fs)
        && load_fonts(i_assets->fonts, fs);

    fclose(fs);

    return loadSucceeded;
}

void unload_assets() {
    if (!i_assets) {
        return;
    }

    unload_textures(i_assets->textures);
    unload_fonts(i_assets->fonts);

    free(i_assets);
    i_assets = nullptr;
}

const Assets& get_assets() {
    return *i_assets;
}

}
