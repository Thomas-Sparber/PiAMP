#include "drawcommand.h"
#include "drawcommandimage.h"
#include "drawcommandtext.h"
#include "drawcommandtextwithimagebackground.h"
#include "drawcommandstring.h"
#include "drawcommandsleep.h"
#include "drawcommandrectangle.h"
#include "drawcommandrevalidate.h"
#include "reusabledrawcommands.h"

void* ReusableDrawCommands::drawCommands[MAX_DRAW_COMMANDS];
LinkedList<SavedDrawCommand> ReusableDrawCommands::drawCommandHandlers;
int ReusableDrawCommands::drawCommandsSize = 0;

void* ReusableDrawCommands::get()
{
  if(drawCommandsSize != 0)
  {
    drawCommandsSize--;
    return drawCommands[drawCommandsSize];
  }

  return new DrawCommand();
}

void ReusableDrawCommands::put(void *drawCommand)
{
  delete (DrawCommand*)drawCommand;
  return;

  if(drawCommandsSize >= MAX_DRAW_COMMANDS)
  {
    delete (DrawCommand*)drawCommand;
    return;
  }
  drawCommands[drawCommandsSize] = drawCommand;
  drawCommandsSize++;
}

DrawCommandHandler* ReusableDrawCommands::get(DrawCommandType type)
{
  for(int i = 0; i < drawCommandHandlers.size(); ++i)
  {
    SavedDrawCommand d = drawCommandHandlers.get(i);
    if(d.type == type)
    {
      drawCommandHandlers.remove(i);
      return d.handler;
    }
  }

  switch(type)
  {
    case DrawCommandType::Image:
      return new DrawCommandImage();
    case DrawCommandType::Text:
      return new DrawCommandText();
    case DrawCommandType::TextWithImageBackground:
      return new DrawCommandTextWithImageBackground();
    case DrawCommandType::String:
      return new DrawCommandString();
    case DrawCommandType::Sleep:
      return new DrawCommandSleep();
    case DrawCommandType::Rectangle:
      return new DrawCommandRectangle();
    case DrawCommandType::Revalidate:
      return new DrawCommandRevalidate();
  }

  return NULL;
}

void ReusableDrawCommands::put(DrawCommandType type, DrawCommandHandler *handler)
{
  switch(type)
  {
    case DrawCommandType::Image:
      delete (DrawCommandImage*)handler;
	  break;
    case DrawCommandType::Text:
      delete (DrawCommandText*)handler;
	  break;
    case DrawCommandType::TextWithImageBackground:
      delete (DrawCommandTextWithImageBackground*)handler;
	  break;
    case DrawCommandType::String:
      delete (DrawCommandString*)handler;
	  break;
    case DrawCommandType::Sleep:
      delete (DrawCommandSleep*)handler;
	  break;
    case DrawCommandType::Rectangle:
      delete (DrawCommandRectangle*)handler;
	  break;
    case DrawCommandType::Revalidate:
      delete (DrawCommandRevalidate*)handler;
	  break;
    default:
      Utils::log("Received invalid draw command to delete - memory leak!");
      break;
  }
  return;
  SavedDrawCommand d;
  d.type = type;
  d.handler = handler;
  drawCommandHandlers.add(d);
}