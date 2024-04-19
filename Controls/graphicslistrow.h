#pragma once

#include "drawcommandrectangle.h"
#include "font.h"
#include "graphics.h"

#define ROW_HEIGHT 60

class GraphicsListRow : public Graphics
{

public:
  GraphicsListRow(Font *f_font, const char *s_id, const char *s_name) :
    font(f_font),
    id(),
    name(),
    index(),
    background_r(),
    background_g(),
    background_b(),
    text_r(),
    text_g(),
    text_b()
  {
    setId(s_id);
    setName(s_name);
  }

  void setId(const char *s_id)
  {
    if(strncmp(id, s_id, 100) != 0)
    {
      snprintf(id, 100, "%s", s_id);
      invalidate();
    }
  }

  const char* getId() const
  {
    return id;
  }

  void setName(const char *s_name)
  {
    if(strncmp(name, s_name, 100) != 0)
    {
      snprintf(name, 100, "%s", s_name);
      invalidate();
    }
  }

  const char* getName() const
  {
    return name;
  }

  void setIndex(uint16_t ui_index)
  {
    if(index != ui_index)
    {
      index = ui_index;
      invalidate();
    }
  }

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

  void setTextColor(unsigned char uc_text_r, unsigned char uc_text_g, unsigned char uc_text_b)
  {
    if(text_r != uc_text_r || text_g != uc_text_g || text_b != uc_text_b)
    {
      text_r = uc_text_r;
	  text_g = uc_text_g;
      text_b = uc_text_b;
      invalidate();
    }
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    uint16_t x = 0;
    uint16_t y = ROW_HEIGHT * index;
    uint16_t w = displayWidth;
    uint16_t h = ROW_HEIGHT;
	  
    if(x < draw_x + draw_w && draw_x < x + w && draw_y < y + h && y < draw_y + draw_h)
    {
      uint16_t cursor_x = 20;
      uint16_t cursor_y = ROW_HEIGHT * (index+1) - 15;
      cursor_x -= 10;
      LinkedList<Utils::TextPart> texts = Utils::splitText(font, name, cursor_x, cursor_y, 1, displayWidth);

      Utils::TextPart p = texts.get(0);
      drawCommands->add(DrawCommandRectangle::create(x, y, w, h, background_r, background_g, background_b, draw_x, draw_y, draw_w, draw_h));
      drawCommands->add(DrawCommandText::create(x, y, w, h, cursor_x, cursor_y, text_r, text_g, text_b, p.text, draw_x, draw_y, draw_w, draw_h));
	  
      uint16_t r_x = x;
      uint16_t r_y = y + h - 3;
      uint16_t r_w = w;
      uint16_t r_h = 2;

      drawCommands->add(DrawCommandRectangle::create(r_x, r_y, r_w, r_h, text_r, text_g, text_b, draw_x, draw_y, draw_w, draw_h));
    }

    revalidateUsingDrawCommand(drawCommands);
  }

private:
  Font *font;
  char id[100];
  char name[100];
  uint16_t index;
  bool isSelected;
  unsigned char selected_r;
  unsigned char selected_g;
  unsigned char selected_b;
  unsigned char background_r;
  unsigned char background_g;
  unsigned char background_b;
  unsigned char text_r;
  unsigned char text_g;
  unsigned char text_b;

}; //end class GraphicsListRow
