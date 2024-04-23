#pragma once

#include "drawcommand.h"

struct DrawCommandString : public DrawCommandHandler
{
  static DrawCommand* create(const String &str)
  {
    DrawCommand *command = DrawCommand::create(DrawCommandType::String, 0, 0, 0, 0, 0, 0, 0, 0);
    DrawCommandString *commandString = (DrawCommandString*)ReusableDrawCommands::get(DrawCommandType::String);
    commandString->str = str;
    command->data = commandString;
    return command;
  }

  virtual void handle(LCDWIKI_KBV */*mylcd*/, Font */*font*/, int16_t /*x*/, int16_t /*y*/, uint16_t /*w*/, uint16_t /*h*/) override
  {
    //nothing to do
  }
  
  String str;
};
