#include "zf3ap.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct FontData {
    zf3::FontArrangementInfo arrangementInfo;
    zf3::Vec2DInt texSize;
    zf3::Byte texPxData[zf3::gk_texPxDataSizeLimit];
};

static int calc_largest_bitmap_width(const FT_Face ftFace) {
    int width = 0;

    for (int i = 0; i < zf3::gk_fontCharRangeSize; i++) {
        FT_Load_Glyph(ftFace, FT_Get_Char_Index(ftFace, zf3::gk_fontCharRangeBegin + i), FT_LOAD_DEFAULT);
        FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);

        if (ftFace->glyph->bitmap.width > width) {
            width = ftFace->glyph->bitmap.width;
        }
    }

    return width;
}

static inline int get_line_height(const FT_Face ftFace) {
    return ftFace->size->metrics.height >> 6;
}

static zf3::Vec2DInt calc_font_tex_size(const FT_Face ftFace) {
    const int largestGlyphBitmapWidth = calc_largest_bitmap_width(ftFace);
    const int idealTexWidth = largestGlyphBitmapWidth * zf3::gk_fontCharRangeSize;

    return {
        (idealTexWidth < zf3::gk_texSizeLimit.x) ? idealTexWidth : zf3::gk_texSizeLimit.x,
        get_line_height(ftFace) * ((idealTexWidth / zf3::gk_texSizeLimit.x) + 1)
    };
}

static bool load_font_data(FontData& fd, const FT_Library ftLib, const char* const filePath, const int ptSize) {
    memset(&fd, 0, sizeof(fd));

    FT_Face ftFace;

    if (FT_New_Face(ftLib, filePath, 0, &ftFace)) {
        zf3::log_error("Failed to create a FreeType face object for font with file path %s.", filePath);
        return false;
    }

    FT_Set_Char_Size(ftFace, ptSize << 6, 0, 96, 0);

    fd.arrangementInfo.lineHeight = get_line_height(ftFace);

    fd.texSize = calc_font_tex_size(ftFace);

    if (fd.texSize.y > zf3::gk_texSizeLimit.y) {
        zf3::log_error("Font texture size is too large!");
        FT_Done_Face(ftFace);
        return false;
    }

    const int texPxDataSize = zf3::gk_texChannelCnt * fd.texSize.x * fd.texSize.y;

    for (int i = 0; i < texPxDataSize; i += zf3::gk_texChannelCnt) {
        fd.texPxData[i + 0] = 255;
        fd.texPxData[i + 1] = 255;
        fd.texPxData[i + 2] = 255;
        fd.texPxData[i + 3] = 0;
    }

    int charDrawX = 0;
    int charDrawY = 0;

    for (int i = 0; i < zf3::gk_fontCharRangeSize; i++) {
        FT_UInt ftCharIndex = FT_Get_Char_Index(ftFace, zf3::gk_fontCharRangeBegin + i);

        FT_Load_Glyph(ftFace, ftCharIndex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);

        if (charDrawX + ftFace->glyph->bitmap.width > zf3::gk_texSizeLimit.x) {
            charDrawX = 0;
            charDrawY += fd.arrangementInfo.lineHeight;
        }

        fd.arrangementInfo.chars.horOffsets[i] = ftFace->glyph->metrics.horiBearingX >> 6;
        fd.arrangementInfo.chars.verOffsets[i] = (ftFace->size->metrics.ascender - ftFace->glyph->metrics.horiBearingY) >> 6;
        fd.arrangementInfo.chars.horAdvances[i] = ftFace->glyph->metrics.horiAdvance >> 6;

        fd.arrangementInfo.chars.srcRects[i].x = charDrawX;
        fd.arrangementInfo.chars.srcRects[i].y = charDrawY;
        fd.arrangementInfo.chars.srcRects[i].width = ftFace->glyph->bitmap.width;
        fd.arrangementInfo.chars.srcRects[i].height = ftFace->glyph->bitmap.rows;

        for (int j = 0; j < zf3::gk_fontCharRangeSize; j++) {
            FT_Vector ftKerning;
            FT_Get_Kerning(ftFace, FT_Get_Char_Index(ftFace, zf3::gk_fontCharRangeBegin + j), ftCharIndex, FT_KERNING_DEFAULT, &ftKerning);
            fd.arrangementInfo.chars.kernings[(zf3::gk_fontCharRangeSize * i) + j] = ftKerning.x >> 6;
        }

        for (int y = 0; y < fd.arrangementInfo.chars.srcRects[i].height; y++) {
            for (int x = 0; x < fd.arrangementInfo.chars.srcRects[i].width; x++) {
                unsigned char pxAlpha = ftFace->glyph->bitmap.buffer[(y * ftFace->glyph->bitmap.width) + x];

                if (pxAlpha > 0) {
                    int pxX = fd.arrangementInfo.chars.srcRects[i].x + x;
                    int pxY = fd.arrangementInfo.chars.srcRects[i].y + y;
                    int pxDataIndex = (pxY * fd.texSize.x * zf3::gk_texChannelCnt) + (pxX * zf3::gk_texChannelCnt);

                    fd.texPxData[pxDataIndex + 3] = pxAlpha;
                }
            }
        }

        charDrawX += fd.arrangementInfo.chars.srcRects[i].width;
    }

    FT_Done_Face(ftFace);

    return true;
}

bool pack_fonts(FILE* const outputFS, const cJSON* const instrsCJ, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, char* const errorMsgBuf) {
    FT_Library ftLib;

    if (FT_Init_FreeType(&ftLib)) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to initialise FreeType!");
        return false;
    }

    const auto fontData = static_cast<FontData*>(malloc(sizeof(FontData))); // Reused for each font.

    if (!fontData) {
        FT_Done_FreeType(ftLib);
        return false;
    }

    const cJSON* const cjFonts = get_cj_assets_array(instrsCJ, "fonts");

    if (!cjFonts) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to get the fonts array from the packing instructions JSON file!");
        free(fontData);
        FT_Done_FreeType(ftLib);
        return false;
    }

    const int fontCnt = cJSON_GetArraySize(cjFonts);

    if (fontCnt > zf3::gk_fontLimit) {
        snprintf(errorMsgBuf, gk_errorMsgBufSize, "Font count exceeds the limit of %d!", zf3::gk_fontLimit);
        free(fontData);
        FT_Done_FreeType(ftLib);
        return false;
    }

    fwrite(&fontCnt, sizeof(fontCnt), 1, outputFS);

    bool success = true;

    const cJSON* cjFont = nullptr;

    cJSON_ArrayForEach(cjFont, cjFonts) {
        const cJSON* const cjRelFilePath = cJSON_GetObjectItem(cjFont, "relFilePath");
        const cJSON* const cjPtSize = cJSON_GetObjectItem(cjFont, "ptSize");

        if (!cJSON_IsString(cjRelFilePath) || !cJSON_IsNumber(cjPtSize)) {
            snprintf(errorMsgBuf, gk_errorMsgBufSize, "Invalid font entry in packing instructions JSON file!");
            success = false;
            break;
        }

        if (!complete_asset_file_path(srcAssetFilePathBuf, errorMsgBuf, srcAssetFilePathStartLen, cjRelFilePath->valuestring)) {
            success = false;
            break;
        }

        if (!load_font_data(*fontData, ftLib, srcAssetFilePathBuf, cjPtSize->valueint)) {
            snprintf(errorMsgBuf, gk_errorMsgBufSize, "Failed to load data for font with relative file path %s and point size %d!", cjRelFilePath->valuestring, cjPtSize->valueint);
            success = false;
            break;
        }

        fwrite(fontData, sizeof(*fontData), 1, outputFS);

        zf3::log("Packed font with file path \"%s\" and point size %d.", srcAssetFilePathBuf, cjPtSize->valueint);
    }

    free(fontData);
    FT_Done_FreeType(ftLib);

    return success;
}
