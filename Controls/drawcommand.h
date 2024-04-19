#pragma once

#include "drawcommandhandler.h"
#include "drawcommandtype.h"
#include "reusabledrawcommands.h"


struct DrawCommand
{
  static DrawCommand* create(DrawCommandType type, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h)
  {
    DrawCommand *command = (DrawCommand*)ReusableDrawCommands::get();
    command->type = type;
    command->x = max(x, draw_x);
    command->y = max(y, draw_y);
    command->w = w - (command->x - x);
    command->h = h - (command->y - y);
    if(command->x+command->w > draw_x+draw_w)command->w -= (command->x+command->w) - (draw_x+draw_w);
    if(command->y+command->h > draw_y+draw_h)command->h -= (command->y+command->h) - (draw_y+draw_h);
    command->data = NULL;
    return command;
  }

  DrawCommandType type;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  DrawCommandHandler *data;
};
