#pragma once

#include "esphome/core/datatypes.h"
#include "esphome/core/color.h"
#include "esphome/components/display/display_buffer.h"

#include <tiny_text_renderer.h>

namespace esphome {
namespace advance_font {

class Font;

class Font : public display::BaseFont {
public:
  Font(const unsigned char* font_data, const unsigned long font_data_size, const unsigned int height);

  void print(int x_start, int y_start, display::Display *display, Color color, const char *text,
             Color background) override;
  void measure(const char *text, int *width, int *x_offset, int *baseline, int *height) override;

private:
  const unsigned char* font_data_;
  const unsigned long font_data_size_;
  const unsigned int height_;
};

}  // namespace advance_font
}  // namespace esphome
