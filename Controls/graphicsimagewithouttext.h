#pragma once

#include "drawcommandimage.h"
#include "drawcommandsleep.h"
#include "drawcommandstring.h"
#include "drawcommandtextwithimagebackground.h"
#include "drawcommandtext.h"
#include "font.h"
#include "graphicstextandimage.h"
#include "utils.h"

#define BLOCKSIZE_X 80
#define BLOCKSIZE_Y 60

class GraphicsImageWithoutText : public Graphics
{

public:
  GraphicsImageWithoutText(Font *f_font) :
    font(f_font),
    text(),
    imageStr(),
    offset_y(),
    margin()
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

  void setOffsetY(int16_t ui_offset_y)
  {
    if(offset_y != ui_offset_y)
    {
      offset_y = ui_offset_y;
      invalidate();
    }
  }

  void setMargin(uint16_t ui_margin)
  {
    if(margin != ui_margin)
    {
      margin = ui_margin;
      invalidate();
    }
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    if(text == "" && imageStr == "")return;

    int16_t draw_x = max(x, margin);
    int16_t draw_y = max(y, offset_y + margin);
    uint16_t draw_w = w - (draw_x - x);
    uint16_t draw_h = h - (draw_y - y);
    if(draw_x+draw_w > displayWidth - margin)draw_w -= (draw_x+draw_w) - (displayWidth - margin);
    if(draw_y+draw_h-offset_y > displayHeight/2 - margin)draw_h -= (draw_y+draw_h-offset_y) - (displayHeight/2 - margin);
    
  	int16_t cursor_x;
  	int16_t cursor_y;
  	int16_t text_x;
  	int16_t text_y;
  	uint16_t text_w;
  	uint16_t text_h;
  	uint16_t max_w;
  	uint16_t max_h;
  	
  	GraphicsTextAndImage::doPrecalculation(font, text, offset_y, displayWidth, displayHeight, cursor_x, cursor_y, text_x, text_y, text_w, text_h, max_w, max_h);

    DrawCommand *cmd_str = DrawCommandString::create(imageStr);

    const String *image = &(((DrawCommandString*)(cmd_str->data))->str);
  
    for(int16_t x = 0; x < IMAGE_WIDTH; x += BLOCKSIZE_X) {
      for(int16_t y = offset_y; y < offset_y+IMAGE_HEIGHT; y += BLOCKSIZE_Y) {
        if(text_x > x + BLOCKSIZE_X || x > text_x + static_cast<int32_t>(text_w) || y > text_y + static_cast<int32_t>(text_h) || text_y > y + BLOCKSIZE_Y) {
          if(x < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < y + BLOCKSIZE_Y && y < draw_y + static_cast<int32_t>(draw_h)) {
            drawCommands->add(DrawCommandImage::create(x, y, BLOCKSIZE_X, BLOCKSIZE_Y, offset_y, image, draw_x, draw_y, draw_w, draw_h));
          }
        } else {
        
          if(text_x > x){

            //x - text_x
          
            if(x < draw_x + static_cast<int32_t>(draw_w) && draw_x < text_x && draw_y < y + BLOCKSIZE_Y && y < draw_y + static_cast<int32_t>(draw_h)) {
              drawCommands->add(DrawCommandImage::create(x, y, text_x - x, BLOCKSIZE_Y, offset_y, image, draw_x, draw_y, draw_w, draw_h));
            }

            if(text_y > y){

              //y - text_y
              if(x < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < text_y && y < draw_y + static_cast<int32_t>(draw_h)) {
                drawCommands->add(DrawCommandImage::create(x, y, BLOCKSIZE_X, text_y - y, offset_y, image, draw_x, draw_y, draw_w, draw_h));
              }
            } else {

              //(text_y+text_h) - (y+BLOCKSIZE_Y)
              if(y + BLOCKSIZE_Y > text_y + static_cast<int32_t>(text_h) && x < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < y + BLOCKSIZE_Y && text_y + static_cast<int32_t>(text_h) < draw_y + static_cast<int32_t>(draw_h)) {
                drawCommands->add(DrawCommandImage::create(x, text_y + text_h, BLOCKSIZE_X, (y + BLOCKSIZE_Y) - (text_y + text_h), offset_y, image, draw_x, draw_y, draw_w, draw_h));
              }
            }
          } else {

            //(text_x+text_w) - x + BLOCKSIZE_X
            if(x + BLOCKSIZE_X > text_x + static_cast<int32_t>(text_w) && text_x + static_cast<int32_t>(text_w) < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < y + BLOCKSIZE_Y && y < draw_y + static_cast<int32_t>(draw_h)) {
              drawCommands->add(DrawCommandImage::create(text_x + text_w, y, (x + BLOCKSIZE_X) - (text_x + text_w), BLOCKSIZE_Y, offset_y, image, draw_x, draw_y, draw_w, draw_h));
            }

            if(text_y > y){

              //y - text_y
              if(x < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < text_y && y < draw_y + static_cast<int32_t>(draw_h)) {
                drawCommands->add(DrawCommandImage::create(x, y, BLOCKSIZE_X, text_y - y, offset_y, image, draw_x, draw_y, draw_w, draw_h));
              }
            } else {

              //(text_y+text_h) - (y+BLOCKSIZE_Y)
              if(y + BLOCKSIZE_Y > text_y + static_cast<int32_t>(text_h) && x < draw_x + static_cast<int32_t>(draw_w) && draw_x < x + BLOCKSIZE_X && draw_y < y + BLOCKSIZE_Y && text_y + static_cast<int32_t>(text_h) < draw_y + static_cast<int32_t>(draw_h)) {
                drawCommands->add(DrawCommandImage::create(x, text_y + text_h, BLOCKSIZE_X, (y + BLOCKSIZE_Y) - (text_y + text_h), offset_y, image, draw_x, draw_y, draw_w, draw_h));
              }
            }
          }
        }
      }
    }

    drawCommands->add(cmd_str);

    revalidateUsingDrawCommand(drawCommands);
  }

private:
  Font *font;
  String text;
  String imageStr;
  int16_t offset_y;
  uint16_t margin;

}; //end class GraphicsImageWithoutText
