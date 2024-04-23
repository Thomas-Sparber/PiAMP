#pragma once

#include "font.h"

#include <Arduino.h>
#include <LinkedList.h>

namespace Utils
{

struct TextPart
{
  TextPart()
  {}
  
  TextPart(const char *t)
  {
    snprintf(text, 100, "%s", t);
  }
  
  char text[100];
};

inline static char *ltrim(char *s)
{
    while(isspace(*s))s++;
    return s;
}

inline static char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

inline static char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

inline static void log(const char *a) {
  char text[256];
  snprintf(text, 256, "{\"action\":\"log\",\"message\":\"%s\"}", a);
  Serial.println(text);
}

inline static void log(const char *a, const char *b) {
  char text[256];
  snprintf(text, 256, "{\"action\":\"log\",\"message\":\"%s%s\"}", a, b);
  Serial.println(text);
}

inline static void log(int a, const char *b) {
  char text[256];
  snprintf(text, 256, "{\"action\":\"log\",\"message\":\"%d%s\"}", a, b);
  Serial.println(text);
}

inline static void log(const char *a, int b) {
  char text[256];
  snprintf(text, 256, "{\"action\":\"log\",\"message\":\"%s%d\"}", a, b);
  Serial.println(text);
}

inline static void log(const char *a, const char *b, const char *c) {
  char text[256];
  snprintf(text, 256, "{\"action\":\"log\",\"message\":\"%s%s%s\"}", a, b, c);
  Serial.println(text);
}

inline static int lastIndexOf(const char *text, char p, int index)
{
  while(index >= 0)
  {
    if(text[index] == p)
    {
      break;
    }

    index--;
  }

  return index;
}

inline static LinkedList<TextPart> splitText(Font *font, const char *toSplit, uint16_t cursor_x, uint16_t cursor_y, int maxLines, uint16_t maxWidth)
{
  LinkedList<TextPart> parts;

  char tempText[256];
  snprintf(tempText, 256, "%s", toSplit);
  char *text = tempText;

  while(strlen(text) != 0)
  {
    char tempPart[256];
    snprintf(tempPart, 256, "%s", text);
    char *part = trim(tempPart);
    text += strlen(text);
    
    int16_t text_x;
    int16_t text_y;
    uint16_t text_w;
    uint16_t text_h;
    font->getTextBounds(part, cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);

    if(text_x + text_w <= maxWidth)
    {
      parts.add(part);
      continue;
    }

    if(parts.size() >= maxLines - 1)
    {
      char withDots[100];
      
      do
      {
        part[strlen(part) - 1] = '\0';
        
        snprintf(withDots, 100, "%s...", part);
  
        font->getTextBounds(withDots, cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
      }
      while(text_x + text_w > maxWidth);
      
      parts.add(withDots);
      continue;
    }

    int index = strlen(part);
    while((index = lastIndexOf(part, ' ', index - 1)) != -1)
    {
      char sub[100];
      snprintf(sub, 100, "%s", part);
      sub[index] = '\0';
      font->getTextBounds(sub, cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
  
      if(text_x + text_w <= maxWidth)
      {
        snprintf(tempText, 256, "%s", part + index);
        text = tempText;
      
        parts.add(sub);
        break;
      }
    }

    if(index != -1)continue;

    while(text_x + text_w > maxWidth)
    {
      text--;
      part[strlen(part) - 1] = '\0';

      font->getTextBounds(part, cursor_x, cursor_y, &text_x, &text_y, &text_w, &text_h);
    }

    parts.add(part);
  }

  return parts;
}

inline static int readSerialChar(int timeout=1000) {
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

inline static void readSerialLine(char *currentCommand, int size, int timeout=1000) {
  int currentPosition = 0;
  
  unsigned long startTime = millis();
  while(true) {
    while(Serial.available() == 0) {
      delay(1);
  
      if(millis() >= startTime + timeout)
      {
        Utils::log("Read timeout");
        return;
      }
    }
  
    while(Serial.available() > 0) {
      int b = Serial.read();

      if(b == '\n') {
        currentCommand[currentPosition] = '\0';
        return;
      }

      if(currentPosition < size)
      {
        currentCommand[currentPosition++] = (char)b;
      }
    }
  }
}

inline static const String& emptyString()
{
  static String empty = "";

  return empty;
}

} //end namespace Utils
