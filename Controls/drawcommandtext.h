#pragma once

#include "drawcommand.h"

struct DrawCommandText : public DrawCommandHandler
{
  static DrawCommand* create(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t cursor_x, uint16_t cursor_y, unsigned char r, unsigned char g, unsigned char b, String text, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::Text, x, y, w, h, draw_x, draw_y, draw_w, draw_h);
    DrawCommandText *commandText = new DrawCommandText();
    commandText->cursor_x = cursor_x;
    commandText->cursor_y = cursor_y;
    commandText->r = r;
    commandText->g = g;
    commandText->b = b;
    commandText->text = text;
    command->data = commandText;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    unsigned int color = mylcd->Color_To_565(r, g, b);
    font->setTextColor(color);
    font->setCursor(cursor_x, cursor_y);
    font->print(text);
  }
  
  uint16_t cursor_x;
  uint16_t cursor_y;
  unsigned char r;
  unsigned char g;
  unsigned char b;
  String text;
};