#pragma once

#include "drawcommandrectangle.h"
#include "graphics.h"

class GraphicsBorder : public Graphics
{

public:
  GraphicsBorder() :
    outset(),
    x(),
    y(),
    w(),
    h(),
    border_thickness()
  {}

  void setOutset(bool b_outset)
  {
    if(outset != b_outset)
    {
      outset = b_outset;
      invalidate();
    }
  }

  void setX(uint16_t ui_x)
  {
    if(x != ui_x)
    {
      x = ui_x;
      invalidate();
    }
  }

  void setY(uint16_t ui_y)
  {
    if(y != ui_y)
    {
      y = ui_y;
      invalidate();
    }
  }

  void setW(uint16_t ui_w)
  {
    if(w != ui_w)
    {
      w = ui_w;
      invalidate();
    }
  }

  void setH(uint16_t ui_h)
  {
    if(h != ui_h)
    {
      h = ui_h;
      invalidate();
    }
  }

  void setBorderThickness(unsigned char uc_border_thickness)
  {
    if(border_thickness != uc_border_thickness)
    {
      border_thickness = uc_border_thickness;
      invalidate();
    }
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t /*displayWidth*/, uint16_t /*displayHeight*/) override
  {
    const unsigned char outset_color_1_r = 250;
    const unsigned char outset_color_1_g = 180;
    const unsigned char outset_color_1_b = 180;
  
    const unsigned char outset_color_2_r = 150;
    const unsigned char outset_color_2_g = 50;
    const unsigned char outset_color_2_b = 50;
  
    const unsigned char inset_color_1_r = 200;
    const unsigned char inset_color_1_g = 200;
    const unsigned char inset_color_1_b = 200;
  
    const unsigned char inset_color_2_r = 80;
    const unsigned char inset_color_2_g = 80;
    const unsigned char inset_color_2_b = 80;
   
    uint16_t r_x = x;
    uint16_t r_y = y;
    uint16_t r_w = w;
    uint16_t r_h = border_thickness;
    unsigned char r = outset ? outset_color_1_r : inset_color_1_r;
    unsigned char g = outset ? outset_color_1_g : inset_color_1_g;
    unsigned char b = outset ? outset_color_1_b : inset_color_1_b;

    //top
    if(r_x < draw_x + draw_w && draw_x < r_x + r_w && draw_y < r_y + r_h && r_y < draw_y + draw_h) {
      drawCommands->add(DrawCommandRectangle::create(r_x, r_y, r_w, r_h, r, g, b, draw_x, draw_y, draw_w, draw_h));
    }

    r_x = x;
    r_y = y + border_thickness;
    r_w = border_thickness;
    r_h = y + h - border_thickness*2;
    r = outset ? outset_color_1_r : inset_color_1_r;
    g = outset ? outset_color_1_g : inset_color_1_g;
    b = outset ? outset_color_1_b : inset_color_1_b;

    //left
    if(r_x < draw_x + draw_w && draw_x < r_x + r_w && draw_y < r_y + r_h && r_y < draw_y + draw_h) {
      drawCommands->add(DrawCommandRectangle::create(r_x, r_y, r_w, r_h, r, g, b, draw_x, draw_y, draw_w, draw_h));
    }

    r_x = x;
    r_y = y + h - border_thickness;
    r_w = w;
    r_h = border_thickness;
    r = outset ? outset_color_2_r : inset_color_2_r;
    g = outset ? outset_color_2_g : inset_color_2_g;
    b = outset ? outset_color_2_b : inset_color_2_b;

    //bottom
    if(r_x < draw_x + draw_w && draw_x < r_x + r_w && draw_y < r_y + r_h && r_y < draw_y + draw_h) {
      drawCommands->add(DrawCommandRectangle::create(r_x, r_y, r_w, r_h, r, g, b, draw_x, draw_y, draw_w, draw_h));
    }

    r_x = x + w - border_thickness;
    r_y = y + border_thickness;
    r_w = border_thickness;
    r_h = y + h - border_thickness*2;
    r = outset ? outset_color_2_r : inset_color_2_r;
    g = outset ? outset_color_2_g : inset_color_2_g;
    b = outset ? outset_color_2_b : inset_color_2_b;

    //right
    if(r_x < draw_x + draw_w && draw_x < r_x + r_w && draw_y < r_y + r_h && r_y < draw_y + draw_h) {
      drawCommands->add(DrawCommandRectangle::create(r_x, r_y, r_w, r_h, r, g, b, draw_x, draw_y, draw_w, draw_h));
    }

    revalidateUsingDrawCommand(drawCommands);
  }

private:
  bool outset;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  unsigned char border_thickness;

}; //end class GraphicsBorder
