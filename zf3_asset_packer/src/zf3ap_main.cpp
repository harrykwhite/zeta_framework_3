#include "zf3ap.h"

const char* const ik_packingInstrsFileName = "packing_instrs.json";

struct AssetPacker {
    FILE* outputFS;
    char* instrsFileChars;
    cJSON* instrsCJ;
};

static FILE* open_output_file(const char* const outputDir, char* const errorMsgBuf) {
    // Determine the output file path.
    char filePath[256];
    const int filePathLen = snprintf(filePath, sizeof(filePath), "%s/%s", outputDir, zf3::gk_assetsFileName);

    if (filePathLen >= sizeof(filePath)) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "The provided output directory \"%s\" is too long!", outputDir);
        return nullptr;
    }

    // Create or replace the output file.
    FILE* const outputFS = fopen(filePath, "wb");

    if (!outputFS) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to open the output file \"%s\"!", filePath);
    }

    return outputFS;
}

static char* get_packing_instrs_file_chars(char* const srcAssetFilePathBuf, char* const errorMsgBuf, const int srcAssetFilePathBufStartLen) {
    if (!complete_asset_file_path(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathBufStartLen, ik_packingInstrsFileName)) {
        return nullptr;
    }

    return zf3::get_file_contents(srcAssetFilePathBuf);
}

static bool run_asset_packer(AssetPacker& packer, char* const errorMsgBuf, const char* const srcDir, const char* const outputDir) {
    assert(zf3::is_zero(packer));

    // Open the output file.
    packer.outputFS = open_output_file(outputDir, errorMsgBuf);

    if (!packer.outputFS) {
        return false;
    }

    // Initialise the source asset file path buffer with the source directory.
    char srcAssetFilePathBuf[gk_srcAssetFilePathBufSize] = {};
    const int srcAssetFilePathStartLen = snprintf(srcAssetFilePathBuf, gk_srcAssetFilePathBufSize, "%s/", srcDir);

    if (srcAssetFilePathStartLen >= gk_srcAssetFilePathBufSize) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "The provided source directory \"%s\" is too long!", srcDir);
        return false;
    }

    // Get the contents of the packing instructions JSON file.
    packer.instrsFileChars = get_packing_instrs_file_chars(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathStartLen);

    if (!packer.instrsFileChars) {
        return false;
    }

    // Parse the packing instructions file contents.
    packer.instrsCJ = cJSON_Parse(packer.instrsFileChars);

    if (!packer.instrsCJ) {
        zf3::log_error("cJSON failed to parse the packing instructions file contents!");
        return false;
    }

    // Perform packing for each asset type using the packing instructions file.
    if (!pack_textures(packer.outputFS, packer.instrsCJ, srcAssetFilePathBuf, srcAssetFilePathStartLen, errorMsgBuf)
        || !pack_fonts(packer.outputFS, packer.instrsCJ, srcAssetFilePathBuf, srcAssetFilePathStartLen, errorMsgBuf)
        || !pack_audio(packer.outputFS, packer.instrsCJ, srcAssetFilePathBuf, srcAssetFilePathStartLen, errorMsgBuf)) {
        return false;
    }

    return true;
}

static void clean_asset_packer(AssetPacker& packer, const bool packingSuccessful) {
    cJSON_Delete(packer.instrsCJ);

    free(packer.instrsFileChars);

    if (packer.outputFS) {
        fclose(packer.outputFS);

        if (!packingSuccessful) {
            remove(zf3::gk_assetsFileName);
        }
    }

    zf3::zero_out(packer);
}

int main(const int argCnt, const char* const* args) {
    if (argCnt != 3) {
        zf3::log_error("Invalid number of command-line arguments! Expected a source directory and an output directory.");
        return EXIT_FAILURE;
    }

    const char* const srcDir = args[1];    // The directory containing the assets to pack.
    const char* const outputDir = args[2]; // The directory to output the packed assets file to.

    char errorMsgBuf[gk_errorMsgBufSize] = {};

    AssetPacker packer;
    const bool packingSuccessful = run_asset_packer(packer, errorMsgBuf, srcDir, outputDir);
    clean_asset_packer(packer, packingSuccessful);

    if (!packingSuccessful) {
        zf3::log_error(errorMsgBuf);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

cJSON* get_cj_assets_array(const cJSON* const instrsCJObj, const char* const arrayName) {
    cJSON* const assetsCJ = cJSON_GetObjectItemCaseSensitive(instrsCJObj, arrayName);

    if (!cJSON_IsArray(assetsCJ)) {
        return nullptr;
    }

    return assetsCJ;
}

bool complete_asset_file_path(char* const srcAssetFilePathBuf, char* const errorMsgBuf, const int srcAssetFilePathStartLen, const char* const relPath) {
    strncpy(srcAssetFilePathBuf + srcAssetFilePathStartLen, relPath, gk_srcAssetFilePathBufSize - srcAssetFilePathStartLen);

    if (srcAssetFilePathBuf[gk_srcAssetFilePathBufSize - 1]) {
        const int lenLimit = gk_srcAssetFilePathBufSize - 1 - srcAssetFilePathStartLen;
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "The asset relative file path of \"%s\" exceeds the length limit of %d characters!", relPath, lenLimit);
        return false;
    }

    return true;
}
