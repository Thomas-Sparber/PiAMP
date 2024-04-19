#pragma once

#include "drawcommand.h"

class Graphics;

struct DrawCommandRevalidate : public DrawCommandHandler
{
  static DrawCommand* create(Graphics *graphics)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::Revalidate, 0, 0, 0, 0, 0, 0, 0, 0);
    DrawCommandRevalidate *commandRevalidate = (DrawCommandRevalidate*)ReusableDrawCommands::get(DrawCommandType::Revalidate);
    commandRevalidate->graphics = graphics;
    command->data = commandRevalidate;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
  
  Graphics *graphics;
};
