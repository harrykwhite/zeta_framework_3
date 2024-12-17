#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stb_image.h>
#include <zf3c.h>

constexpr int gk_errorMsgBufSize = 512;
constexpr int gk_srcAssetFilePathBufSize = 256;

cJSON* get_cj_assets_array(const cJSON* const instrsCJObj, const char* const arrayName);
bool complete_asset_file_path(char* const srcAssetFilePathBuf, char* const errorMsgBuf, const int srcAssetFilePathStartLen, const char* const relPath);

bool pack_textures(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf);
bool pack_fonts(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf);
bool pack_audio(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf);
