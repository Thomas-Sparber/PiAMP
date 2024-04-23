#pragma once

#include "graphicslist.h"

#include <ArduinoJson.h>
#include <LinkedList.h>

class GraphicsListStreaming : public GraphicsList
{

public:
  GraphicsListStreaming(Font *f_font, const String &s_action) :
    GraphicsList(f_font),
    action(s_action),
    idToSet(),
    size(0),
    startIndex(0),
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

  void readAndFillList(int rowsPerPage)
  {
    char command[256];
    
    if(idToSet != "")
    {
      snprintf(command, 256, "{\"action\":\"%s\",\"rowsPerPage\":%d,\"id\":\"%s\"}", action.c_str(), rowsPerPage, idToSet.c_str());
      idToSet[0] = '\0';
    }
    else
    {
      snprintf(command, 256, "{\"action\":\"%s\",\"rowsPerPage\":%d,\"currentSelection\":%d}", action.c_str(), rowsPerPage, currentSelection);
    }

    Serial.println(command);

    Utils::readSerialLine(command, 256);

    JsonDocument doc;
    deserializeJson(doc, command);

    int tempSize = doc["size"];
    size = tempSize;

    int tempCurrentSelection = doc["currentSelection"];
    currentSelection = tempCurrentSelection;

    int tempStartIndex = doc["startIndex"];
    startIndex = tempStartIndex;

    int numModels = doc["numModels"];

    for(int i = 0; i < numModels; ++i)
    {
      char id[100];
      char name[100];
      Utils::readSerialLine(id, 100);
       Utils::readSerialLine(name, 100);

      if(list.size() > i)
      {
        GraphicsListRow *row = list.get(i);
        row->setId(id);
        row->setName(name);
      }
      else
      {
        GraphicsListRow *row = new GraphicsListRow(font, id, name);
        list.add(row);
      }
    }

    for(int i = numModels; i < list.size(); ++i)
    {
      GraphicsListRow *row = list.get(i);
      row->setId("");
      row->setName("");
    }

    Graphics::invalidate();
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
      idToSet = currentId;
    }
  }

  int getListValidSize()
  {
    for(int i = 0; i < list.size(); ++i)
    {
      if(list.get(i)->getId() == "")
      {
        return i;
      }
    }

    return list.size();
  }

  virtual const String& getCurrentId() override
  {
    if(idToSet != "")return idToSet;
    if(startIndex > currentSelection || currentSelection - startIndex >= getListValidSize())return Utils::emptyString();

    const GraphicsListRow *row = list.get(currentSelection - startIndex);
    return row->getId();
  }

  virtual int getSize() override
  {
    return size;
  }

  virtual void createDrawCommands(LinkedList<DrawCommand*> *drawCommands, uint16_t draw_x, uint16_t draw_y, uint16_t draw_w, uint16_t draw_h, uint16_t displayWidth, uint16_t displayHeight) override
  {
    Utils::log("Current selection: ", currentSelection);
    int rowsPerPage = displayHeight / ROW_HEIGHT;
    if(startIndex > currentSelection || currentSelection - startIndex >= getListValidSize() || idToSet != "")
    {
      Utils::log("Streaming list data");
      readAndFillList(rowsPerPage);
    }

    int listValidSize = getListValidSize();
    for(int i = 0; i < listValidSize; ++i)
	  {
      uint16_t x = 0;
      uint16_t y = ROW_HEIGHT * i;
      uint16_t w = displayWidth;
      uint16_t h = ROW_HEIGHT;

      if(x < draw_x + draw_w && draw_x < x + w && draw_y < y + h && y < draw_y + draw_h)
      {
        GraphicsListRow *row = list.get(i);
        row->setIndex(i);

        if(startIndex + i == currentSelection)
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

    if(!isValid())
    {
      uint16_t last_y = listValidSize * ROW_HEIGHT;
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
  String action;
  String idToSet;
  int size;
  int startIndex;
  LinkedList<GraphicsListRow*> list;

}; //end class GraphicsListStreaming
