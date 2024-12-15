#include "zf3ap.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct FontDisplayInfoWithPixels {
    zf3::FontDisplayInfo displayInfo;
    zf3::Byte pxData[zf3::gk_texChannelCnt * zf3::gk_texSizeLimit.x * zf3::gk_texSizeLimit.y];
};

static inline int get_line_height(const FT_Face ftFace) {
    return ftFace->size->metrics.height >> 6;
}

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

static zf3::Vec2DInt calc_font_tex_size(const FT_Face ftFace) {
    const int largestGlyphBitmapWidth = calc_largest_bitmap_width(ftFace);
    const int idealTexWidth = largestGlyphBitmapWidth * zf3::gk_fontCharRangeSize;

    return {
        (idealTexWidth < zf3::gk_texSizeLimit.x) ? idealTexWidth : zf3::gk_texSizeLimit.x,
        get_line_height(ftFace) * ((idealTexWidth / zf3::gk_texSizeLimit.x) + 1)
    };
}

static bool load_font_display_info_with_pixels(FontDisplayInfoWithPixels* const buf, FT_Library ftLib, const char* const filePath, const int ptSize) {
    FT_Face ftFace;

    if (FT_New_Face(ftLib, filePath, 0, &ftFace)) {
        zf3::log_error("Failed to create a FreeType face object for font with file path %s.", filePath);
        return false;
    }

    FT_Set_Char_Size(ftFace, ptSize << 6, 0, 96, 0);

    buf->displayInfo.lineHeight = get_line_height(ftFace);

    buf->displayInfo.texSize = calc_font_tex_size(ftFace);

    if (buf->displayInfo.texSize.y > zf3::gk_texSizeLimit.y) {
        zf3::log_error("Font texture size is too large!");
        FT_Done_Face(ftFace);
        return false;
    }

    int pxDataSize = zf3::gk_texChannelCnt * buf->displayInfo.texSize.x * buf->displayInfo.texSize.y;

    for (int i = 0; i < pxDataSize; i += zf3::gk_texChannelCnt) {
        buf->pxData[i + 0] = 255;
        buf->pxData[i + 1] = 255;
        buf->pxData[i + 2] = 255;
        buf->pxData[i + 3] = 0;
    }

    int charDrawX = 0;
    int charDrawY = 0;

    for (int i = 0; i < zf3::gk_fontCharRangeSize; i++) {
        FT_UInt ftCharIndex = FT_Get_Char_Index(ftFace, zf3::gk_fontCharRangeBegin + i);

        FT_Load_Glyph(ftFace, ftCharIndex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);

        if (charDrawX + ftFace->glyph->bitmap.width > zf3::gk_texSizeLimit.x) {
            charDrawX = 0;
            charDrawY += buf->displayInfo.lineHeight;
        }

        buf->displayInfo.chars.horOffsets[i] = ftFace->glyph->metrics.horiBearingX >> 6;
        buf->displayInfo.chars.verOffsets[i] = (ftFace->size->metrics.ascender - ftFace->glyph->metrics.horiBearingY) >> 6;
        buf->displayInfo.chars.horAdvances[i] = ftFace->glyph->metrics.horiAdvance >> 6;

        buf->displayInfo.chars.srcRects[i].x = charDrawX;
        buf->displayInfo.chars.srcRects[i].y = charDrawY;
        buf->displayInfo.chars.srcRects[i].width = ftFace->glyph->bitmap.width;
        buf->displayInfo.chars.srcRects[i].height = ftFace->glyph->bitmap.rows;

        for (int j = 0; j < zf3::gk_fontCharRangeSize; j++) {
            FT_Vector ftKerning;
            FT_Get_Kerning(ftFace, FT_Get_Char_Index(ftFace, zf3::gk_fontCharRangeBegin + j), ftCharIndex, FT_KERNING_DEFAULT, &ftKerning);
            buf->displayInfo.chars.kernings[(zf3::gk_fontCharRangeSize * i) + j] = ftKerning.x >> 6;
        }

        for (int y = 0; y < buf->displayInfo.chars.srcRects[i].height; y++) {
            for (int x = 0; x < buf->displayInfo.chars.srcRects[i].width; x++) {
                unsigned char pxAlpha = ftFace->glyph->bitmap.buffer[(y * ftFace->glyph->bitmap.width) + x];

                if (pxAlpha > 0) {
                    int pxX = buf->displayInfo.chars.srcRects[i].x + x;
                    int pxY = buf->displayInfo.chars.srcRects[i].y + y;
                    int pxDataIndex = (pxY * buf->displayInfo.texSize.x * zf3::gk_texChannelCnt) + (pxX * zf3::gk_texChannelCnt);

                    buf->pxData[pxDataIndex + 3] = pxAlpha;
                }
            }
        }

        charDrawX += buf->displayInfo.chars.srcRects[i].width;
    }

    FT_Done_Face(ftFace);

    return true;
}

bool pack_fonts(cJSON* const instrsCJ, FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen) {
    FT_Library ftLib;

    if (FT_Init_FreeType(&ftLib)) {
        zf3::log_error("Failed to initialise FreeType!");
        return false;
    }

    const auto fontDisplayInfoWithPixelsBuf = static_cast<FontDisplayInfoWithPixels*>(malloc(sizeof(FontDisplayInfoWithPixels))); // Reused for each font.

    if (!fontDisplayInfoWithPixelsBuf) {
        zf3::log_error("Failed to allocate memory for font data!");
        FT_Done_FreeType(ftLib);
        return false;
    }

    bool success = true;

    const cJSON* const cjFonts = getCJAssetsArrayAndWriteAssetCnt(instrsCJ, outputFS, "fonts");

    if (cjFonts) {
        const cJSON* cjFont = NULL;

        cJSON_ArrayForEach(cjFont, cjFonts) {
            cJSON* cjRelFilePath = cJSON_GetObjectItem(cjFont, "relFilePath");
            cJSON* cjPtSize = cJSON_GetObjectItem(cjFont, "ptSize");

            if (!cJSON_IsString(cjRelFilePath) || !cJSON_IsNumber(cjPtSize)) {
                zf3::log_error("Invalid font entry in packing instructions JSON file!");
                success = false;
                break;
            }

            if (!complete_asset_file_path(srcAssetFilePathBuf, srcAssetFilePathStartLen, cjRelFilePath->valuestring)) {
                success = false;
                break;
            }

            if (!load_font_display_info_with_pixels(fontDisplayInfoWithPixelsBuf, ftLib, srcAssetFilePathBuf, cjPtSize->valueint)) {
                zf3::log_error("Failed to load data for font with relative file path %s and point size %d!", cjRelFilePath->valuestring, cjPtSize->valueint);
                success = false;
                break;
            }

            fwrite(&fontDisplayInfoWithPixelsBuf->displayInfo, sizeof(fontDisplayInfoWithPixelsBuf->displayInfo), 1, outputFS);
            fwrite(fontDisplayInfoWithPixelsBuf->pxData, zf3::gk_texChannelCnt * fontDisplayInfoWithPixelsBuf->displayInfo.texSize.x * fontDisplayInfoWithPixelsBuf->displayInfo.texSize.y, 1, outputFS);

            zf3::log("Packed font with file path \"%s\" and point size %d.", srcAssetFilePathBuf, cjPtSize->valueint);
        }
    }

    free(fontDisplayInfoWithPixelsBuf);
    FT_Done_FreeType(ftLib);

    return success;
}
