#pragma once

#include "font.h"

#include <Arduino.h>
#include <LinkedList.h>

namespace Utils
{

void log(String l) {
  Serial.println("{\"action\":\"log\",\"message\":\""+l+"\"}");
}

LinkedList<String> splitText(const Font *font, String text, uint16_t cursor_x, uint16_t cursor_y, int maxLines, int maxWidth)
{
  LinkedList<String> parts;

  while(text != "")
  {
    String part = text;
    part.trim();
    text = "";
    
    uint16_t text_x;
    uint16_t text_y;
    uint16_t text_w;
    uint16_t text_h;
    font->getTextBounds(part.c_str(), cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);

    if(text_x + text_w <= maxWidth)
    {
      parts.add(part);
      continue;
    }

    if(parts.size() >= maxLines - 1)
    {
      do
      {
        part = part.substring(0, part.length() - 1);
        
        String withDots = part + "...";
  
        font->getTextBounds(withDots.c_str(), cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
      }
      while(text_x + text_w > maxWidth);
      
      parts.add(part + "...");
      continue;
    }

    int index = part.length();
    while((index = part.lastIndexOf(' ', index - 1)) != -1)
    {
      font->getTextBounds(part.c_str(), cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
  
      if(text_x + text_w <= maxWidth)
      {
        text = part.substring(index);
        part = part.substring(0, index);
      
        parts.add(part);
        break;
      }
    }

    if(index != -1)continue;

    while(text_x + text_w > maxWidth)
    {
      text = part.substring(part.length() - 1) + text;
      part = part.substring(0, part.length() - 1);

      font->getTextBounds(part.c_str(), cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
    }

    parts.add(part);
  }

  return parts;
}

int readSerialChar(int timeout=1000) {
  unsigned long startTime = millis();
  while(Serial.available() == 0) {
    delay(1);

    if(millis() >= startTime + timeout)break;
  }

  if(Serial.available() > 0)
  {
    return (unsigned)Serial.read();
  }
  else
  {
    Utils::log("Read timeout");
    return -1;
  }
}

String readSerialLine(int timeout=1000) {
  String temp;
  unsigned long startTime = millis();
  while(true) {
    while(Serial.available() == 0) {
      delay(1);
  
      if(millis() >= startTime + timeout)break;
    }
  
    while(Serial.available() > 0) {
      int b = Serial.read();
      temp += (char)b;
  
      if(b == '\n') {
        return temp;
      }
    }
  }
}

} //end namespace Utils