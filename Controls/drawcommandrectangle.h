#pragma once

#include "drawcommand.h"

struct DrawCommandRectangle : public DrawCommandHandler
{
  static DrawCommand* create(uint16_t x, uint16_t y, uint16_t w, uint16_t h, unsigned char r, unsigned char g, unsigned char b, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::Rectangle, x, y, w, h, draw_x, draw_y, draw_w, draw_h);
    DrawCommandRectangle *commandRectangle = new DrawCommandRectangle();
    commandRectangle->r = r;
    commandRectangle->g = g;
    commandRectangle->b = b;
    command->data = commandRectangle;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    unsigned int color = mylcd->Color_To_565(r, g, b);
    mylcd->Set_Draw_color(color);
    mylcd->Fill_Rectangle(x, y, x+w, y+h);
  }

  unsigned char r;
  unsigned char g;
  unsigned char b;
};