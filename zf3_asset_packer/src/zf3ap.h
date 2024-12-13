#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stb_image.h>
#include <zf3c.h>

constexpr int gk_srcAssetFilePathBufSize = 256;

cJSON* getCJSONAssetsArrayAndWriteAssetCnt(cJSON* const instrsCJ, FILE* const outputFileStream, const char* const packingInstrsArrayName);

bool pack_textures(cJSON* const instrsCJ, FILE* const outputFileStream, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen);
