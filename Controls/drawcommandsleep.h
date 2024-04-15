#pragma once

#include "drawcommand.h"

struct DrawCommandSleep : public DrawCommandHandler
{
  static DrawCommand* create(int milliseconds)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::Sleep, 0, 0, 0, 0, 0, 0, 0, 0);
    DrawCommandSleep *commandSleep = new DrawCommandSleep();
    commandSleep->milliseconds = milliseconds;
    command->data = commandSleep;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    //nothing to do - needs to be handled in main program
  }
  
  int milliseconds;
};