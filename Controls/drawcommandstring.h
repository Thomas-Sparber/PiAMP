#pragma once

#include "drawcommand.h"

struct DrawCommandString : public DrawCommandHandler
{
  static DrawCommand* create(const char *str)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::String, 0, 0, 0, 0, 0, 0, 0, 0);
    DrawCommandString *commandString = (DrawCommandString*)ReusableDrawCommands::get(DrawCommandType::String);
    snprintf(commandString->str, 256, "%s", str);
    command->data = commandString;
    return command;
  }

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override
  {
    //nothing to do
  }
  
  char str[256];
};
