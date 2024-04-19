#include "drawcommand.h"
#include "drawcommandimage.h"
#include "drawcommandtext.h"
#include "drawcommandtextwithimagebackground.h"
#include "drawcommandstring.h"
#include "drawcommandsleep.h"
#include "drawcommandrectangle.h"
#include "drawcommandrevalidate.h"
#include "reusabledrawcommands.h"

LinkedList<void*> ReusableDrawCommands::drawCommands;
LinkedList<SavedDrawCommand> ReusableDrawCommands::drawCommandHandlers;

void* ReusableDrawCommands::get()
{
  if(drawCommands.size() != 0)
  {
    return drawCommands.shift();
  }

  return new DrawCommand();
}

void ReusableDrawCommands::put(void *drawCommand)
{
  delete drawCommand;
  return;
  drawCommands.add(drawCommand);
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
  delete handler;
  return;
  SavedDrawCommand d;
  d.type = type;
  d.handler = handler;
  drawCommandHandlers.add(d);
}