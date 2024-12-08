#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include <zf3_common.h>

#define PACKING_INSTRS_FILE_NAME "packing_instrs.json"

#define OUTPUT_FILE_PATH_BUF_SIZE 256
#define SRC_ASSET_FILE_PATH_BUF_SIZE 256

static char *get_packing_instrs_file_chars(char* const srcAssetFilePathBuf, const int srcAssetFilePathBufStartLen)
{
    // Append the packing instructions file name to the source asset file path buffer.
    strncpy(srcAssetFilePathBuf + srcAssetFilePathBufStartLen, PACKING_INSTRS_FILE_NAME, SRC_ASSET_FILE_PATH_BUF_SIZE - srcAssetFilePathBufStartLen);

    if (srcAssetFilePathBuf[SRC_ASSET_FILE_PATH_BUF_SIZE - 1])
    {
        zf3_log_error("The packing instructions file name is too long!");
        return NULL;
    }

    return zf3_get_file_contents(srcAssetFilePathBuf);
}

int main(const int argCnt, const char* const* args) {
    // Get the source directory and output directory if provided.
    if (argCnt != 3) {
        zf3_log_error("Invalid number of command-line arguments! Expected a source directory and an output directory to be provided.");
        return EXIT_FAILURE;
    }

    const char* const srcDir = args[1]; // The directory containing the assets to pack.
    const char* const outputDir = args[2]; // The directory to output the packed assets file to.

    // Determine the output file path using the directory.
    char outputFilePath[OUTPUT_FILE_PATH_BUF_SIZE];
    const int outputFilePathLen = snprintf(outputFilePath, OUTPUT_FILE_PATH_BUF_SIZE, "%s\\%s", outputDir, ZF3_ASSETS_FILE_NAME);

    if (outputFilePathLen >= OUTPUT_FILE_PATH_BUF_SIZE) {
        zf3_log_error("The provided output directory \"%s\" is too long!", outputDir);
        return EXIT_FAILURE;
    }

    // Initialise the source asset file path buffer with the source directory.
    char srcAssetFilePathBuf[SRC_ASSET_FILE_PATH_BUF_SIZE] = {0};
    const int srcAssetFilePathStartLen = snprintf(srcAssetFilePathBuf, SRC_ASSET_FILE_PATH_BUF_SIZE, "%s\\", srcDir);

    if (srcAssetFilePathStartLen >= SRC_ASSET_FILE_PATH_BUF_SIZE) {
        zf3_log_error("The provided source directory of \"%s\" is too long!", srcDir);
        return EXIT_FAILURE;
    }

    // Get the contents of the packing instructions JSON file.
    char* const packingInstrsFileChars = get_packing_instrs_file_chars(srcAssetFilePathBuf, srcAssetFilePathStartLen);

    if (!packingInstrsFileChars) {
        return EXIT_FAILURE;
    }

    free(packingInstrsFileChars);

    return EXIT_SUCCESS;
}
