#pragma once

#include "graphicslist.h"

#include <LinkedList.h>

class GraphicsListStatic : public GraphicsList
{

public:
  GraphicsListStatic(Font *f_font) :
    GraphicsList(f_font),
    pageStart(0),
    list()
  {}

  virtual bool isValid()
  {
    if(!Graphics::isValid())
    {
      return false;
    }

    for(int i = 0; i < list.size(); ++i)
	  {
      const GraphicsListRow *row = list.get(i);
      if(!row->isValid())
  	  {
          return false;
  	  }
    }

    return false;
  }

  virtual void invalidate()
  {
    Graphics::invalidate();

    for(int i = 0; i < list.size(); ++i)
	  {
      GraphicsListRow *row = list.get(i);
      row->invalidate();
    }
  }

  virtual void setCurrentId(const String &currentId) override
  {
    bool found = false;

    for(int i = 0; i < list.size(); ++i)
	  {
      const GraphicsListRow *row = list.get(i);
      if(row->getId() == currentId)
  	  {
          found = true;
          setCurrentSelection(i);
          break;
  	  }
  	}

    if(!found)
    {
      Utils::log("New id ", currentId.c_str(), " not found");
    }
  }

  virtual const String& getCurrentId() override
  {
    if(currentSelection >= list.size())return Utils::emptyString();

    const GraphicsListRow *row = list.get(currentSelection);
    return row->getId();
  }

  void clearList()
  {
    while(list.size() > 0)
	  {
      GraphicsListRow *row = list.shift();
      delete row;
	  }

    list.clear();
  }

  void addEntry(const String &id, const String &name)
  {
    GraphicsListRow *row = new GraphicsListRow(font, id, name);
    list.add(row);
  }

  virtual int getSize() override
  {
    return list.size();
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    int rowsPerPage = displayHeight / ROW_HEIGHT;
    int newPageStart = (currentSelection / rowsPerPage) * rowsPerPage;
    if(pageStart != newPageStart)
	  {
      pageStart = newPageStart;
      invalidate();
	  }

    for(int i = 0; i < rowsPerPage; ++i)
	  {
      int currentIndex = pageStart + i;
  	  if(currentIndex < list.size())
  	  {
        uint16_t x = 0;
        uint16_t y = ROW_HEIGHT * i;
        uint16_t w = displayWidth;
        uint16_t h = ROW_HEIGHT;
	  
        if(x < draw_x + draw_w && draw_x < x + w && draw_y < y + h && y < draw_y + draw_h)
        {
          GraphicsListRow *row = list.get(currentIndex);
          row->setIndex(i);

          if(currentIndex == currentSelection)
            row->setBackgroundColor(selected_r, selected_g, selected_b);
          else
            row->setBackgroundColor(background_r, background_g, background_b);

          row->setTextColor(text_r, text_g, text_b);

          if(!row->isValid())
          {
            row->createDrawCommands(drawCommands, draw_x, draw_y, draw_w, draw_h, displayWidth, displayHeight);
          }
        }
      }
	}

    if(!isValid())
    {
      uint16_t last_y = min(rowsPerPage, list.size() - pageStart) * ROW_HEIGHT;
      if(last_y < displayHeight)
      {
        uint16_t x = 0;
        uint16_t y = last_y;
        uint16_t w = displayWidth;
        uint16_t h = displayHeight - y;

        if(x < draw_x + draw_w && draw_x < x + w && draw_y < y + h && y < draw_y + draw_h)
        {
          drawCommands->add(DrawCommandRectangle::create(x, y, w, h, background_r, background_g, background_b, draw_x, draw_y, draw_w, draw_h));
	    }
      }
    }

    revalidateUsingDrawCommand(drawCommands);
  }

private:
  int pageStart;
  LinkedList<GraphicsListRow*> list;

}; //end class GraphicsListStatic
