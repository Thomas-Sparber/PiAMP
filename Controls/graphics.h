#pragma once

#include "drawcommandrevalidate.h"

#include <LinkedList.h>

class Graphics
{

public:
  Graphics() :
    valid(false)
  {}

  virtual bool isValid() const
  {
    return valid;
  }

  virtual void revalidate()
  {
    valid = true;
  }

  void revalidateUsingDrawCommand(LinkedList<DrawCommand*> *drawCommands)
  {
    DrawCommand *cmd_revalidate = DrawCommandRevalidate::create(this);
	drawCommands->add(cmd_revalidate);
  }

  virtual void invalidate()
  {
    valid = false;
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) = 0;

private:
  bool valid;

}; //end class Graphics