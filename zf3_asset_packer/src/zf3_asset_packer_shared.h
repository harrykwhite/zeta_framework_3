#ifndef ZF3_ASSET_PACKER_SHARED_H
#define ZF3_ASSET_PACKER_SHARED_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stb_image.h>
#include <zf3_common.h>

#define SRC_ASSET_FILE_PATH_BUF_SIZE 256

cJSON* getCJSONAssetsArrayAndWriteAssetCnt(cJSON* const instrsCJ, FILE* const outputFileStream, const char* const packingInstrsArrayName);

bool pack_textures(cJSON* const instrsCJ, FILE* const outputFileStream, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen);

#endif
