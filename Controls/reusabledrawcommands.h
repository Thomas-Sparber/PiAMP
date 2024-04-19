#pragma once

#include <LinkedList.h>

#include "drawcommandtype.h"

struct SavedDrawCommand
{
  DrawCommandType type;
  DrawCommandHandler *handler;
}; //end class SavedDrawCommand

class ReusableDrawCommands
{

public:
  static void* get();
  static void put(void *drawCommand);

  static DrawCommandHandler* get(DrawCommandType type);
  static void put(DrawCommandType type, DrawCommandHandler *handler);

private:
  static LinkedList<void*> drawCommands;
  static LinkedList<SavedDrawCommand> drawCommandHandlers;

}; //end class ReusableDrawCommands