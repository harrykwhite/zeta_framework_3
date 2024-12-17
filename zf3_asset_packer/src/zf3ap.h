#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stb_image.h>
#include <zf3c.h>

constexpr int gk_srcAssetFilePathBufSize = 256;

cJSON* getCJAssetsArrayAndWriteAssetCnt(cJSON* const instrsCJ, FILE* const outputFileStream, const char* const packingInstrsArrayName);

bool complete_asset_file_path(char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const char* const relPath);

bool pack_textures(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen);
bool pack_fonts(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen);
bool pack_sounds(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen);
