#include "font.h"
#include "harfbuzz_schrift.h"

#include "esphome/core/log.h"
#include "esphome/core/application.h"


namespace esphome {
namespace advance_font {

Font::Font(const unsigned char* font_data, const unsigned long font_data_size, const unsigned int height):
    font_data_(font_data), font_data_size_(font_data_size), height_(height)
{
}

void Font::measure_internal(hb_font_t *font, unsigned int glyph_count, hb_glyph_info_t *glyph_info, hb_glyph_position_t *glyph_pos, int *width, int *baseline, int *height) {
    bool calculate_width = (width != nullptr);
    bool calculate_baseline = (baseline != nullptr);
    bool calculate_height = (height != nullptr);
    
    if (calculate_width) *width = 0;
    if (calculate_height) *height = 0;
    if (calculate_baseline) *baseline = 0;

    int y_min = 0;
    int y_max = 0;
    for (unsigned int i = 0; i < glyph_count; i++) {
        hb_codepoint_t glyphid  = glyph_info[i].codepoint;

        hb_glyph_extents_t extents;
        if (hb_font_get_glyph_extents(font, glyphid, &extents)) {
            if (calculate_height) {
                y_max = std::max(y_max, extents.y_bearing);
                y_min = std::min(y_min, extents.y_bearing + extents.height);
            }

            if (calculate_baseline) {
                *baseline = std::max(*baseline, extents.y_bearing);
            }
        }

        if (calculate_width) {
            *width += glyph_pos[i].x_advance;
        }
    }

    if (calculate_height) {
        *height = y_max - y_min;
    }
}

void Font::measure(const char *text, int *width, int *x_offset, int *baseline, int *height) {
    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    hb_blob_t *blob = hb_blob_create((const char*)font_data_, font_data_size_, HB_MEMORY_MODE_READONLY, NULL, NULL); /* or hb_blob_create_from_file_or_fail() */
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);

    hb_font_set_scale(font, height_, height_);

    hb_shape(font, buf, NULL, 0);

    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    *x_offset = 0;
    measure_internal(font, glyph_count, glyph_info, glyph_pos, width, baseline, height);

    hb_buffer_destroy(buf);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
}

void Font::print(int x_start, int y_start, display::Display *display, Color color, const char *text, Color background) {
    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    hb_blob_t *blob = hb_blob_create((const char*)font_data_, font_data_size_, HB_MEMORY_MODE_READONLY, NULL, NULL); /* or hb_blob_create_from_file_or_fail() */
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);

    hb_font_set_scale(font, height_, height_);

    hb_shape(font, buf, NULL, 0);

    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    int baseline = 0;
    measure_internal(font, glyph_count, glyph_info, glyph_pos, nullptr, &baseline, nullptr);

    hb_position_t cursor_x = x_start;
    hb_position_t cursor_y = y_start + baseline;
    for (unsigned int i = 0; i < glyph_count; i++) {
        hb_codepoint_t glyphid  = glyph_info[i].codepoint;

        hb_glyph_extents_t extents;
        if (!hb_font_get_glyph_extents(font, glyphid, &extents)) {
            // Error?
            continue;
        }

        // App.feed_wdt();

        unsigned int glyph_width = extents.width;
        unsigned int glyph_height = -1 * extents.height;

        int glyph_x_offset = glyph_pos[i].x_offset + extents.x_bearing;
        int glyph_y_offset = glyph_pos[i].y_offset + extents.y_bearing;

        uint8_t* pixels = (uint8_t*)malloc(glyph_width * glyph_height);
        memset(pixels, 0, glyph_width * glyph_height);

        draw_glyph(font, glyphid, extents, pixels);

        for (int y = 0; y < glyph_height; ++y) {
            for (int x = 0; x < glyph_width; ++x) {
                // Todo: support non monochrome displays
                if (pixels[y * glyph_width + x] > 128) {
                    display->draw_pixel_at(
                        cursor_x + x + glyph_x_offset,
                        cursor_y + y - glyph_y_offset,
                        color
                    );
                }
            }
        }

        free(pixels);

        cursor_x += glyph_pos[i].x_advance;
        cursor_y += glyph_pos[i].y_advance;
    }

    hb_buffer_destroy(buf);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
}

}  // namespace advance_font
}  // namespace esphome
