#pragma once

#include "drawcommand.h"
#include "utils.h"

struct DrawCommandImage : public DrawCommandHandler
{
  static DrawCommand* create(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t offset_y, const char *image, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::Image, x, y, w, h, draw_x, draw_y, draw_w, draw_h);
    DrawCommandImage *commandImage = (DrawCommandImage*)ReusableDrawCommands::get(DrawCommandType::Image);
    commandImage->offset_y = offset_y;
    commandImage->image = image;
    command->data = commandImage;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    char command[256];
    snprintf(command, 256, "{\"action\":\"getImage\",\"image\":\"%s\",\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d}", image, x, y-offset_y, w, h);
    Serial.println(command);

    uint16_t pixels[w];
      
    for(int y_counter = y; y_counter < y+h; y_counter++) {
      for(int x_counter = x; x_counter < x+w; x_counter++) {
        int r = Utils::readSerialChar();
        int g = Utils::readSerialChar();
        int b = Utils::readSerialChar();
      
        if(r == -1 || g == -1 || b == -1) {
          Utils::log("canceling");
          y_counter = y+h;
          break;
        }

        pixels[x_counter - x] = mylcd->Color_To_565(r, g, b);
      }

      mylcd->Set_Addr_Window(x, y_counter, x + w - 1, y_counter - 1);
      mylcd->Push_Any_Color(pixels, w, true, 0);
    }
  }
  
  uint16_t offset_y;
  const char *image;
};
