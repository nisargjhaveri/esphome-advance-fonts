#include "font.h"
#include "tiny_text_renderer.h"

#include "esphome/core/log.h"
#include "esphome/core/application.h"


namespace esphome {
namespace advance_font {

Font::Font(const unsigned char* font_data, const unsigned long font_data_size, const unsigned int height):
    font_data_(font_data), font_data_size_(font_data_size), height_(height)
{
}

void Font::measure(const char *text, int *width, int *x_offset, int *baseline, int *height) {
    hb_font_t* font = ttr_create_font((const char*)font_data_, font_data_size_, height_);

    *x_offset = 0;
    ttr_measure_text(font, text, (unsigned int*)width, (unsigned int*)height, (unsigned int*)baseline);

    ttr_destroy_font(font);
}

void Font::print(int x_start, int y_start, display::Display *display, Color color, const char *text, Color background) {
    hb_font_t* font = ttr_create_font((const char*)font_data_, font_data_size_, height_);

    struct pixel_callback_data_t {
        display::Display* display;
        Color color;
    } pixel_callback_data = {display, color};

    ttr_draw_text_with_callback(font, text, x_start, y_start, 0, 0, [](unsigned int x, unsigned int y, uint8_t pixel, void* user_data) {
        pixel_callback_data_t* data = (pixel_callback_data_t*)user_data;

        if (pixel > 128) {
            data->display->draw_pixel_at(x, y, data->color);
        }
    }, &pixel_callback_data);

    ttr_destroy_font(font);
}

}  // namespace advance_font
}  // namespace esphome
