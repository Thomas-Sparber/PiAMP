#pragma once

#include "drawcommandrectangle.h"
#include "graphics.h"

class GraphicsBackground : public Graphics
{

public:
  GraphicsBackground() :
    background_r(),
    background_g(),
    background_b()
  {}

  void setBackgroundColor(unsigned char uc_background_r, unsigned char uc_background_g, unsigned char uc_background_b)
  {
    if(background_r != uc_background_r || background_g != uc_background_g || background_b != uc_background_b)
    {
      background_r = uc_background_r;
      background_g = uc_background_g;
      background_b = uc_background_b;
      invalidate();
    }
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    drawCommands->add(DrawCommandRectangle::create(0, 0, displayWidth, displayHeight, background_r, background_g, background_b, draw_x, draw_y, draw_w, draw_h));
	
    revalidateUsingDrawCommand(drawCommands);
  }

private:
  unsigned char background_r;
  unsigned char background_g;
  unsigned char background_b;

}; //end class GraphicsBackground
