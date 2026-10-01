/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "font_render.h"
#include <ft2build.h>
#include <grrlib.h>
#include FT_FREETYPE_H
#include <string.h>
#define GLYPH_LIMIT 512
/* Immutable textures stay alive until shutdown: queued GX draws never see freed glyphs. */
typedef struct {
    wchar_t character;
    unsigned size;
    FT_UInt index;
    GRRLIB_texImg *texture;
    int left, top, advance;
} Glyph;
static FT_Library library;
static FT_Face face;
static Glyph glyphs[GLYPH_LIMIT];
static size_t glyph_count;
static Glyph *glyph(wchar_t character, unsigned size) {
    for (size_t i = 0; i < glyph_count; i++)
        if (glyphs[i].character == character && glyphs[i].size == size)
            return &glyphs[i];
    if (glyph_count == GLYPH_LIMIT) {
        for (size_t i = 0; i < glyph_count; i++)
            if (glyphs[i].character == '?' && glyphs[i].size == size)
                return &glyphs[i];
        return NULL;
    }
    if (FT_Set_Pixel_Sizes(face, 0, size))
        return NULL;
    FT_UInt index = FT_Get_Char_Index(face, (FT_ULong)character);
    if (FT_Load_Glyph(face, index, FT_LOAD_RENDER))
        return NULL;
    FT_GlyphSlot slot = face->glyph;
    Glyph item = {.character = character,
                  .size = size,
                  .index = index,
                  .left = slot->bitmap_left,
                  .top = slot->bitmap_top,
                  .advance = (int)(slot->advance.x >> 6)};
    FT_Bitmap *bitmap = &slot->bitmap;
    if (bitmap->width && bitmap->rows) {
        unsigned w = (bitmap->width + 3) & ~3u, h = (bitmap->rows + 3) & ~3u;
        item.texture = GRRLIB_CreateEmptyTexture(w, h);
        if (!item.texture)
            return NULL;
        for (unsigned y = 0; y < bitmap->rows; y++)
            for (unsigned x = 0; x < bitmap->width; x++) {
                const unsigned char *row =
                    bitmap->pitch >= 0 ? bitmap->buffer + y * bitmap->pitch
                                       : bitmap->buffer + (bitmap->rows - 1 - y) * (-bitmap->pitch);
                GRRLIB_SetPixelTotexImg(x, y, item.texture, 0xffffff00u | row[x]);
            }
        GRRLIB_FlushTex(item.texture);
    }
    glyphs[glyph_count] = item;
    return &glyphs[glyph_count++];
}
bool font_render_init(const unsigned char *data, size_t bytes) {
    if (FT_Init_FreeType(&library))
        return false;
    if (FT_New_Memory_Face(library, data, (FT_Long)bytes, 0, &face)) {
        FT_Done_FreeType(library);
        library = NULL;
        return false;
    }
    const unsigned sizes[] = {12, 16, 32};
    for (unsigned i = 0; i < 3; i++)
        glyph('?', sizes[i]);
    return true;
}
void font_render_shutdown(void) {
    GX_DrawDone();
    for (size_t i = 0; i < glyph_count; i++)
        if (glyphs[i].texture)
            GRRLIB_FreeTexture(glyphs[i].texture);
    glyph_count = 0;
    if (face)
        FT_Done_Face(face);
    if (library)
        FT_Done_FreeType(library);
    face = NULL;
    library = NULL;
}
unsigned font_render_width(const wchar_t *text, unsigned size) {
    unsigned width = 0;
    while (*text) {
        Glyph *g = glyph(*text++, size);
        if (g)
            width += (unsigned)g->advance;
    }
    return width;
}
void font_render_draw(int x, int y, const wchar_t *text, unsigned size, uint32_t rgba) {
    while (*text) {
        Glyph *g = glyph(*text++, size);
        if (!g)
            continue;
        if (g->texture)
            GRRLIB_DrawImg(x + g->left, y + (int)size - g->top, g->texture, 0, 1, 1, rgba);
        x += g->advance;
    }
}
