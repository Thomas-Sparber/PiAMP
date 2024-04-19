#pragma once

#include "drawcommand.h"

struct DrawCommandTextWithImageBackground : public DrawCommandHandler
{
  static DrawCommand* create(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t cursor_x, uint16_t cursor_y, unsigned char text_r, unsigned char text_g, unsigned char text_b, const char *text, uint16_t image_offset_y, const char *image, unsigned char background_r, unsigned char background_g, unsigned char background_b, char border_size, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::TextWithImageBackground, x, y, w, h, draw_x, draw_y, draw_w, draw_h);
    DrawCommandTextWithImageBackground *commandImageText = (DrawCommandTextWithImageBackground*)ReusableDrawCommands::get(DrawCommandType::TextWithImageBackground);
    commandImageText->cursor_x = cursor_x;
    commandImageText->cursor_y = cursor_y;
    commandImageText->text_r = text_r;
    commandImageText->text_g = text_g;
    commandImageText->text_b = text_b;
    snprintf(commandImageText->text, 50, "%s", text);
    commandImageText->image_offset_y = image_offset_y;
    commandImageText->image = image;
    commandImageText->background_r = background_r;
    commandImageText->background_g = background_g;
    commandImageText->background_b = background_b;
    commandImageText->border_size = border_size;
    command->data = commandImageText;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    const int border_size_2 = border_size * 2;

    char command[256];
    snprintf(command, 256, "{\"action\":\"getImage\",\"image\":\"%s\",\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d}", image, x, y-image_offset_y, w, h);
    Serial.println(command);

    uint16_t pixels[w];
    
    for(int y_counter = y; y_counter < y+h; y_counter++) {
      const int minBorderY = min(
        y_counter + 1 - y,
        y+h - y_counter
      );
        
      for(int x_counter = x; x_counter < x+w; x_counter++) {
        int minBorderDistance = min(
          minBorderY,
          min(
            x_counter + 1 - x,
            x+w - x_counter
          )
        );

        minBorderDistance = min(minBorderDistance, border_size);

        int r = Utils::readSerialChar();
        int g = Utils::readSerialChar();
        int b = Utils::readSerialChar();
      
        if(r == -1 || g == -1 || b == -1) {
          Utils::log("canceling");
          y_counter = y+h;
          break;
        }
      
        r = ((border_size_2 - minBorderDistance) * r + minBorderDistance * background_r) / border_size_2;
        g = ((border_size_2 - minBorderDistance) * g + minBorderDistance * background_g) / border_size_2;
        b = ((border_size_2 - minBorderDistance) * b + minBorderDistance * background_b) / border_size_2;

        pixels[x_counter - x] = mylcd->Color_To_565(r, g, b);
      }

      mylcd->Set_Addr_Window(x, y_counter, x + w - 1, y_counter - 1);
      mylcd->Push_Any_Color(pixels, w, true, 0);
    }

    unsigned int color = mylcd->Color_To_565(text_r, text_g, text_b);
    font->setTextColor(color);
    font->setCursor(cursor_x, cursor_y);
    font->print(text);
  }
  
  uint16_t cursor_x;
  uint16_t cursor_y;
  unsigned char text_r;
  unsigned char text_g;
  unsigned char text_b;
  char text[50];
  uint16_t image_offset_y;
  const char *image;
  unsigned char background_r;
  unsigned char background_g;
  unsigned char background_b;
  char border_size;
};
