#pragma once

#include "font.h"
#include "graphics.h"
#include "graphicslistrow.h"

class GraphicsList : public Graphics
{

public:
  GraphicsList(Font *f_font) :
    font(f_font),
    x(),
    y(),
    w(),
    h(),
    selected_r(),
    selected_g(),
    selected_b(),
    background_r(),
    background_g(),
    background_b(),
    text_r(),
    text_g(),
    text_b(),
    currentSelection(0)
  {}

  void setX(uint16_t ui_x)
  {
    if(x != ui_x)
    {
      x = ui_x;
      Graphics::invalidate();
    }
  }

  void setY(uint16_t ui_y)
  {
    if(y != ui_y)
    {
      y = ui_y;
      Graphics::invalidate();
    }
  }

  void setW(uint16_t ui_w)
  {
    if(w != ui_w)
    {
      w = ui_w;
      Graphics::invalidate();
    }
  }

  void setH(uint16_t ui_h)
  {
    if(h != ui_h)
    {
      h = ui_h;
      Graphics::invalidate();
    }
  }

  void setSelectedColor(unsigned char uc_selected_r, unsigned char uc_selected_g, unsigned char uc_selected_b)
  {
    if(selected_r != uc_selected_r || selected_g != uc_selected_g || selected_b != uc_selected_b)
    {
      selected_r = uc_selected_r;
      selected_g = uc_selected_g;
      selected_b = uc_selected_b;
      Graphics::invalidate();
    }
  }

  void setBackgroundColor(unsigned char uc_background_r, unsigned char uc_background_g, unsigned char uc_background_b)
  {
    if(background_r != uc_background_r || background_g != uc_background_g || background_b != uc_background_b)
    {
      background_r = uc_background_r;
      background_g = uc_background_g;
      background_b = uc_background_b;
      Graphics::invalidate();
    }
  }

  void setTextColor(unsigned char uc_text_r, unsigned char uc_text_g, unsigned char uc_text_b)
  {
    if(text_r != uc_text_r || text_g != uc_text_g || text_b != uc_text_b)
    {
      text_r = uc_text_r;
	  text_g = uc_text_g;
      text_b = uc_text_b;
      Graphics::invalidate();
    }
  }

  void setCurrentSelection(int i_currentSelection)
  {
    if(currentSelection != i_currentSelection)
	{
      currentSelection = i_currentSelection;
      Graphics::invalidate();
	}
  }

  virtual void setCurrentId(const String &currentId) = 0;

  virtual const String& getCurrentId() = 0;

  virtual int getSize() = 0;

  virtual void incrementCurrentSelection(int diff)
  {
    if(diff != 0)
	{
      while(diff > 0)
      {
        diff--;
        if(++currentSelection >= getSize())setCurrentSelection(0);
      }
      while(diff < 0)
      {
        diff++;
        if(--currentSelection < 0)setCurrentSelection(max(0, getSize() - 1));
      }
      Graphics::invalidate();
    }
  }

protected:
  Font *font;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  unsigned char selected_r;
  unsigned char selected_g;
  unsigned char selected_b;
  unsigned char background_r;
  unsigned char background_g;
  unsigned char background_b;
  unsigned char text_r;
  unsigned char text_g;
  unsigned char text_b;
  int currentSelection;

}; //end class GraphicsList
