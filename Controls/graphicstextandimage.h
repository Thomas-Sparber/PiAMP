#pragma once

#include "drawcommandimage.h"
#include "drawcommandsleep.h"
#include "drawcommandstring.h"
#include "drawcommandtextwithimagebackground.h"
#include "drawcommandtext.h"
#include "font.h"
#include "graphics.h"
#include "utils.h"

#define BORDERSIZE 4
#define MAXLINES 3
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

class GraphicsTextAndImage : public Graphics
{

public:
  GraphicsTextAndImage(Font *f_font) :
    font(f_font),
    text(),
    imageStr(),
    offset_y(),
    background_r(),
    background_g(),
    background_b(),
    text_r(),
    text_g(),
    text_b(),
    textToClear()
  {}

  const String& getText() const
  {
    return text;
  }

  void setText(const String &s_text)
  {
    if(text != s_text)
	  {
      text = s_text;
      invalidate();
    }
  }

  void setImageStr(const String &s_imageStr)
  {
    if(imageStr != s_imageStr)
    {
      imageStr = s_imageStr;
      invalidate();
    }
  }

  void setOffsetY(uint16_t ui_offset_y)
  {
    if(offset_y != ui_offset_y)
    {
      offset_y = ui_offset_y;
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

  static LinkedList<Utils::TextPart> doPrecalculation(Font *font, const String &text, int16_t offset_y, uint16_t displayWidth, uint16_t displayHeight, int16_t &cursor_x, int16_t &cursor_y, int16_t &text_x, int16_t &text_y, uint16_t &text_w, uint16_t &text_h, uint16_t &max_w, uint16_t &max_h)
  {
    cursor_x = 100;
    cursor_y = offset_y + 100;
  
    LinkedList<Utils::TextPart> texts = Utils::splitText(font, text.c_str(), cursor_x, cursor_y, MAXLINES, displayWidth);

    max_w = 0;
    max_h = 0;
    for(int i = 0; i < texts.size(); i++)
    {
      Utils::TextPart str = texts.get(i);
      font->getTextBounds(str.text, cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
  
      max_w = max(max_w, text_w);
      max_h = max(max_h, text_h);
    }

    text_x -= BORDERSIZE;
    text_y = text_y - max_h * (texts.size()-1) /  (MAXLINES-1) - BORDERSIZE;
    text_w = max_w + BORDERSIZE * 2;
    text_h = (max_h + BORDERSIZE * 2) * texts.size();

    text_w = min(text_w, displayWidth - text_x);
    text_h = min(text_h, displayHeight - text_y);

    int diff_x = text_x - (IMAGE_WIDTH - text_w) / 2;
    int diff_y = (text_y-offset_y) - (IMAGE_HEIGHT - text_h) / 2;
    
    cursor_x -= diff_x;
    cursor_y -= diff_y;
    text_x -= diff_x;
    text_y -= diff_y;
  
	  return texts;
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    if(text == "" && imageStr == "")return;

    int16_t cursor_x;
    int16_t cursor_y;
    int16_t text_x;
    int16_t text_y;
    uint16_t text_w;
    uint16_t text_h;
    uint16_t max_w;
    uint16_t max_h;
  
    LinkedList<Utils::TextPart> texts = GraphicsTextAndImage::doPrecalculation(font, text, offset_y, displayWidth, displayHeight, cursor_x, cursor_y, text_x, text_y, text_w, text_h, max_w, max_h);

    DrawCommand *cmd_str = DrawCommandString::create(imageStr);

    const String *image = &(((DrawCommandString*)(cmd_str->data))->str);

    if(text_x < draw_x + static_cast<int32_t>(draw_w) &&
        draw_x < text_x + static_cast<int32_t>(text_w) &&
        draw_y < text_y + static_cast<int32_t>(text_h) &&
        text_y < draw_y + static_cast<int32_t>(draw_h))
    {
      if(textToClear != "" && textToClear != text)
      {
        int16_t old_cursor_x;
        int16_t old_cursor_y;
        int16_t old_text_x;
        int16_t old_text_y;
        uint16_t old_text_w;
        uint16_t old_text_h;
        uint16_t old_max_w;
        uint16_t old_max_h;
      
        LinkedList<Utils::TextPart> oldTexts = GraphicsTextAndImage::doPrecalculation(font, textToClear, offset_y, displayWidth, displayHeight, old_cursor_x, old_cursor_y, old_text_x, old_text_y, old_text_w, old_text_h, old_max_w, old_max_h);

        for(int i = 0; i < oldTexts.size(); i++)
        {
          Utils::TextPart str = oldTexts.get(i);
          const uint16_t cx = old_cursor_x;
          const uint16_t cy = old_cursor_y - old_max_h * (oldTexts.size()-1) /  (MAXLINES-1) + (old_max_h + BORDERSIZE * 2) * i;
          drawCommands->add(DrawCommandText::create(0, 0, 0, 0, cx, cy, background_r, background_g, background_b, str.text, draw_x, draw_y, draw_w, draw_h));
        }

        for(int i = 0; i < texts.size(); i++)
        {
          Utils::TextPart str = texts.get(i);
          const uint16_t x = text_x;
          const uint16_t y = text_y + (max_h + BORDERSIZE * 2) * i;
          const uint16_t w = max_w + BORDERSIZE * 2;
          const uint16_t h = max_h + BORDERSIZE * 2;
          const uint16_t cx = cursor_x;
          const uint16_t cy = cursor_y - max_h * (texts.size()-1) /  (MAXLINES-1) + (max_h + BORDERSIZE * 2) * i;
          drawCommands->add(DrawCommandText::create(x, y, w, h, cx, cy, text_r, text_g, text_b, str.text, draw_x, draw_y, draw_w, draw_h));
        }
    
        drawCommands->add(DrawCommandSleep::create(2000));
      }
    
      for(int i = 0; i < texts.size(); i++)
      {
        Utils::TextPart str = texts.get(i);
        const uint16_t x = text_x;
        const uint16_t y = text_y + (max_h + BORDERSIZE * 2) * i;
        const uint16_t w = max_w + BORDERSIZE * 2;
        const uint16_t h = max_h + BORDERSIZE * 2;
        const uint16_t cx = cursor_x;
        const uint16_t cy = cursor_y - max_h * (texts.size()-1) /  (MAXLINES-1) + (max_h + BORDERSIZE * 2) * i;
        drawCommands->add(DrawCommandTextWithImageBackground::create(x, y, w, h, cx, cy, text_r, text_g, text_b, str.text, offset_y, image, background_r, background_g, background_b, BORDERSIZE, draw_x, draw_y, draw_w, draw_h));
      }
    }

    drawCommands->add(cmd_str);

    textToClear = text;

    revalidateUsingDrawCommand(drawCommands);
  }

private:
  Font *font;
  String text;
  String imageStr;
  uint16_t offset_y;
  unsigned char background_r;
  unsigned char background_g;
  unsigned char background_b;
  unsigned char text_r;
  unsigned char text_g;
  unsigned char text_b;
  String textToClear;

}; //end class GraphicsTextAndImage
