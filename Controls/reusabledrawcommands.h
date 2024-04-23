#pragma once

#include <LinkedList.h>

#include "drawcommandtype.h"

#define MAX_DRAW_COMMANDS 1

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
  static void* drawCommands[MAX_DRAW_COMMANDS];
  static int drawCommandsSize;
  static LinkedList<SavedDrawCommand> drawCommandHandlers;

}; //end class ReusableDrawCommands