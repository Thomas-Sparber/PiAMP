#pragma once

#include "drawcommand.h"

struct DrawCommandString : public DrawCommandHandler
{
  static DrawCommand* create(String str)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::String, 0, 0, 0, 0, 0, 0, 0, 0);
    DrawCommandString *commandString = new DrawCommandString();
    commandString->str = str;
    command->data = commandString;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    //nothing to do
  }
  
  String str;
};