/***********************************************************************************
*                  LCD_CS  LCD_CD  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK 
*     Arduino Uno    A3      A2      A1      A0      A4      10     11     12      13                            
*
*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*     Arduino Uno    8       9       2       3       4       5       6       7
**********************************************************************************/

#include "drawcommand.h"
#include "font.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "graphicsborder.h"
#include "graphicsimagewithouttext.h"
#include "graphicstextandimage.h"
#include "utils.h"

#include <RotaryEncoder.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <base64.hpp>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#define SWITCH_CLK 20
#define SWITCH_DT 19
#define SWITCH_SW 21
#define LOADING_CIRCLE_RADIUS 30
#define BORDER_THICKNESS 4

enum class Selection
{
  Model,
  Ir
};



LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

LinkedList<Graphics*> drawQueue;
LinkedList<DrawCommand*> drawCommands;
LinkedList<String> serialCommands;
Font font(&drawPixel, &drawRect);
GraphicsBorder modelBorder;
GraphicsBorder irBorder;
GraphicsTextAndImage modelGraphics(&font);
GraphicsTextAndImage irGraphics(&font);
GraphicsImageWithoutText modelBackgroundGraphics(&font);
GraphicsImageWithoutText irBackgroundGraphics(&font);
RotaryEncoder encoder(SWITCH_DT, SWITCH_CLK, RotaryEncoder::LatchMode::FOUR3);
volatile bool pi_available;
volatile unsigned long last_pi_response;
bool modelLoaded = false;
bool irLoaded = false;
bool encoderPressed = false;
Selection currentSelection = Selection::Model;

void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  mylcd.Set_Draw_color(color);
  mylcd.Fill_Rectangle(x, y, width, height);
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  mylcd.Draw_Pixe(x, y, color);
}

void repaint(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  const unsigned char selected_background_color_r = 95;
  const unsigned char selected_background_color_g = 5;
  const unsigned char selected_background_color_b = 32;
  
  const unsigned char nonselected_background_color_r = 50;
  const unsigned char nonselected_background_color_g = 50;
  const unsigned char nonselected_background_color_b = 50;

  const unsigned char selected_text_color_r = 255;
  const unsigned char selected_text_color_g = 255;
  const unsigned char selected_text_color_b = 255;
  
  const unsigned char nonselected_text_color_r = 200;
  const unsigned char nonselected_text_color_g = 200;
  const unsigned char nonselected_text_color_b = 200;

  modelBorder.setOutset(currentSelection == Selection::Model);
  modelBorder.createDrawCommands(&drawCommands, x, y, w, h, mylcd.Get_Display_Width(), mylcd.Get_Display_Height());
  irBorder.setOutset(currentSelection == Selection::Ir);
  irBorder.createDrawCommands(&drawCommands, x, y, w, h, mylcd.Get_Display_Width(), mylcd.Get_Display_Height());
  
    /*uint16_t draw_x = max(x, BORDER_THICKNESS);
    uint16_t draw_y = max(y, BORDER_THICKNESS);
    uint16_t draw_w = w - (draw_x - x);
    uint16_t draw_h = h - (draw_y - y);
    if(draw_x+draw_w > mylcd.Get_Display_Width()-BORDER_THICKNESS)draw_w -= (draw_x+draw_w) - (mylcd.Get_Display_Width()-BORDER_THICKNESS);
    if(draw_y+draw_h > mylcd.Get_Display_Height()/2 - BORDER_THICKNESS)draw_h -= (draw_y+draw_h) - (mylcd.Get_Display_Height()/2 - BORDER_THICKNESS);*/

  modelGraphics.setBackgroundColor(
    (currentSelection == Selection::Model) ? selected_background_color_r : nonselected_background_color_r,
    (currentSelection == Selection::Model) ? selected_background_color_g : nonselected_background_color_g,
    (currentSelection == Selection::Model) ? selected_background_color_b : nonselected_background_color_b
  );
  modelGraphics.setTextColor(
    (currentSelection == Selection::Model) ? selected_text_color_r : nonselected_text_color_r,
    (currentSelection == Selection::Model) ? selected_text_color_g : nonselected_text_color_g,
    (currentSelection == Selection::Model) ? selected_text_color_b : nonselected_text_color_b
  );

    /*uint16_t draw_x = max(x, BORDER_THICKNESS);
    uint16_t draw_y = max(y, 240 + BORDER_THICKNESS);
    uint16_t draw_w = w - (draw_x - x);
    uint16_t draw_h = h - (draw_y - y);
    if(draw_x+draw_w > mylcd.Get_Display_Width()-BORDER_THICKNESS)draw_w -= (draw_x+draw_w) - (mylcd.Get_Display_Width()-BORDER_THICKNESS);
    if(draw_y+draw_h > mylcd.Get_Display_Height() - BORDER_THICKNESS)draw_h -= (draw_y+draw_h) - (mylcd.Get_Display_Height() - BORDER_THICKNESS);*/
  
  irGraphics.setBackgroundColor(
    (currentSelection == Selection::Ir) ? selected_background_color_r : nonselected_background_color_r,
    (currentSelection == Selection::Ir) ? selected_background_color_g : nonselected_background_color_g,
    (currentSelection == Selection::Ir) ? selected_background_color_b : nonselected_background_color_b
  );
  irGraphics.setTextColor(
    (currentSelection == Selection::Ir) ? selected_text_color_r : nonselected_text_color_r,
    (currentSelection == Selection::Ir) ? selected_text_color_g : nonselected_text_color_g,
    (currentSelection == Selection::Ir) ? selected_text_color_b : nonselected_text_color_b
  );

  for(int i = 0; i < drawQueue.size(); ++i)
  {
    Graphics *g = drawQueue.get(i);
    if(!g->isValid())
    {
      g->createDrawCommands(&drawCommands, x, y, w, h, mylcd.Get_Display_Width(), mylcd.Get_Display_Height());
    }
  }
  
  Utils::log(String() + drawCommands.size() + " draw commands");
}

void repaint()
{
  repaint(0, 0, mylcd.Get_Display_Width(), mylcd.Get_Display_Height());
}

void repaintAll(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  for(int i = 0; i < drawQueue.size(); ++i)
  {
    Graphics *g = drawQueue.get(i);
    g->invalidate();
  }

  repaint();
}

void repaintAll()
{
  repaintAll(0, 0, mylcd.Get_Display_Width(), mylcd.Get_Display_Height());
}

void setupScreen()
{
  drawQueue.clear();

  if(currentSelection == Selection::Model || currentSelection == Selection::Ir)
  {
    modelBorder.setX(0);
    modelBorder.setY(0);
    modelBorder.setW(mylcd.Get_Display_Width());
    modelBorder.setH(mylcd.Get_Display_Height()/2);
    modelBorder.setBorderThickness(BORDER_THICKNESS);

    irBorder.setX(0);
    irBorder.setY(mylcd.Get_Display_Height()/2);
    irBorder.setW(mylcd.Get_Display_Width());
    irBorder.setH(mylcd.Get_Display_Height()/2);
    irBorder.setBorderThickness(BORDER_THICKNESS);

    modelGraphics.setOffsetY(0);
    modelBackgroundGraphics.setOffsetY(0);
    modelBackgroundGraphics.setMargin(BORDER_THICKNESS);
    
    irGraphics.setOffsetY(mylcd.Get_Display_Height()/2);
    irBackgroundGraphics.setOffsetY(mylcd.Get_Display_Height()/2);
    irBackgroundGraphics.setMargin(BORDER_THICKNESS);
    
    drawQueue.add(&modelBorder);
    drawQueue.add(&irBorder);
    drawQueue.add(&modelGraphics);
    drawQueue.add(&irGraphics);
    drawQueue.add(&modelBackgroundGraphics);
    drawQueue.add(&irBackgroundGraphics);
  }
}

void setup() 
{
  Serial.begin(115200);
  mylcd.Init_LCD();

  pi_available = false;
  
  font.setFont(&FreeSansBold18pt7b);

  pinMode(SWITCH_SW, INPUT);
  attachInterrupt(digitalPinToInterrupt(SWITCH_SW), encoderInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_CLK), encoderCheckPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SWITCH_DT), encoderCheckPosition, CHANGE);

  mylcd.Fill_Screen(0x0000);
  setupScreen();
}

void handleCommand(String command) {
  JsonDocument doc;
  deserializeJson(doc, command);

  String action = doc["action"];

  if(action == "pong")
  {
    last_pi_response = millis();
    if(!pi_available)Utils::log("Pi online");
    pi_available = true;
    
    if(!modelLoaded)
    {
      serialCommands.add("{\"action\":\"getmodel\"}");
    }
    if(modelLoaded && !irLoaded)
    {
      serialCommands.add("{\"action\":\"getir\"}");
    }
  }

  if(action == "model")
  {
    modelLoaded = true;
    String tempModel = doc["name"];
    String tempImage = doc["image"];
    modelGraphics.setText(tempModel);
    modelBackgroundGraphics.setText(tempModel);
    modelGraphics.setImageStr(tempImage);
    modelBackgroundGraphics.setImageStr(tempImage);
    Utils::log("New model loaded: " + tempModel);
    clearDrawCommands();
    repaint();
  }

  if(action == "ir")
  {
    irLoaded = true;
    String tempIr = doc["name"];
    String tempImage = doc["image"];
    irGraphics.setText(tempIr);
    irBackgroundGraphics.setText(tempIr);
    irGraphics.setImageStr(tempImage);
    irBackgroundGraphics.setImageStr(tempImage);
    Utils::log("New ir loaded: " + tempIr);
    clearDrawCommands();
    repaint();
  }
}

void handleSerial() {
  while(serialCommands.size() != 0)
  {
    String command = serialCommands.shift();
    Serial.println(command);
  }

  static char currentCommand[256];
  static int currentPosition = 0;
  
  while(Serial.available() > 0) {
    last_pi_response = millis();
    int b = Serial.read();
    currentCommand[currentPosition++] = (char)b;

    if(currentPosition == 256)currentPosition = 0;

    if(b == '\n') {
      currentCommand[currentPosition] = '\0';
      currentPosition = 0;
      handleCommand(currentCommand);
    }
  }
}

void handleDrawCommands(int diff)
{
  if(drawCommands.size() != 0)
  {
    if(drawCommands.get(0)->type == DrawCommandType::Sleep)
    {
      DrawCommandSleep *s = drawCommands.get(0)->data;
      s->milliseconds -= diff;
      if(s->milliseconds > 0)return;
    }
    
    const DrawCommand *command = drawCommands.shift();
    command->data->handle(&mylcd, &font, command->x, command->y, command->w, command->h);
    delete command->data;
    delete command;

    last_pi_response = millis();
    
    if(drawCommands.size() == 0)Utils::log("Finished rendering");
  }
}

void changeModel(int diff)
{
  clearDrawCommands();
  serialCommands.add(String() + "{\"action\":\"modelchange\",\"value\":"+diff+"}");
}

void changeIr(int diff)
{
  clearDrawCommands();
  serialCommands.add(String() + "{\"action\":\"irchange\",\"value\":"+diff+"}");
}

void handleEncoder()
{
  static long oldPosition = encoder.getPosition();
  encoder.tick();
  
  long newPosition = encoder.getPosition();

  if (newPosition != oldPosition)
  {
    const long diff = newPosition - oldPosition;
    if(currentSelection == Selection::Model)
    {
      changeModel(diff);
    }
    else if(currentSelection == Selection::Ir)
    {
      changeIr(diff);
    }
    oldPosition = newPosition;
    Utils::log(String() + "Encoder: " + diff);
  }

  if(encoderPressed)
  {
    encoderPressed = false;
    
    if(currentSelection == Selection::Model)
    {
      currentSelection = Selection::Ir;
      repaint();
    }
    else if(currentSelection == Selection::Ir)
    {
      currentSelection = Selection::Model;
      repaint();
    }
  }
}

int freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int) __brkval);  
}

void handlePing()
{
  static unsigned long lastPing = 0;
  const unsigned long currentTime = millis();

  if(currentTime - last_pi_response > 5000)
  {
    if(pi_available)Utils::log("Pi went offline");
    pi_available = false;
  }

  if(currentTime - last_pi_response > 2000 && currentTime - lastPing > 1000)
  {
    Serial.println("{\"action\":\"ping\"}");
    lastPing = currentTime;
  }
}

void checkFreeMem() {
  static unsigned long lastPrint = 0;
  const unsigned long currentTime = millis();

  if(currentTime - lastPrint > 1000)
  {
    //Utils::log(String("Free mem ") + freeRam());
    lastPrint = currentTime;
  }
}

void clearDrawCommands()
{
  while(drawCommands.size() != 0)
  {
    const DrawCommand *command = drawCommands.shift();
    delete command->data;
    delete command;
  }

  Utils::log("Draw commands cleared");
}

void showLoading() {
  static float radius = 0;
  const int count = 11;
  const float angles[] = { -1.0, -0.9, -0.8, -0.7, -0,6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0 };
  const int16_t colors[] = {
    mylcd.Color_To_565(189,   9,  64),
    mylcd.Color_To_565(189,   9,  64),
    mylcd.Color_To_565(194,  29,  79),
    mylcd.Color_To_565(199,  49,  94),
    mylcd.Color_To_565(204,  69, 109),
    mylcd.Color_To_565(209,  89, 124),
    mylcd.Color_To_565(214, 109, 139),
    mylcd.Color_To_565(219, 129, 154),
    mylcd.Color_To_565(224, 149, 169),
    mylcd.Color_To_565(229, 169, 184),
    mylcd.Color_To_565(234, 189, 199)
  };

  for(int i = 0; i < count; ++i)
  {
    int x = mylcd.Get_Display_Width()/2 + cos(radius + angles[i]) * LOADING_CIRCLE_RADIUS;
    int y = mylcd.Get_Display_Height()/2 + sin(radius + angles[i]) * LOADING_CIRCLE_RADIUS;
    
    mylcd.Set_Draw_color(colors[i]);
    mylcd.Fill_Rectangle(x - 1, y - 1, x + 1, y + 1);
  }

  delay(50);
  radius += 0.2;
}

void loop() 
{
  static bool last_pi_available = false;
  static unsigned long lastTime = millis();
  const unsigned long currentTime = millis();
  const int diff = currentTime - lastTime;
  lastTime = currentTime;
  
  handleEncoder();

  if(pi_available)
  {
    if(!last_pi_available)
    {
      repaint(mylcd.Get_Display_Width()/2 - LOADING_CIRCLE_RADIUS - 1, mylcd.Get_Display_Height()/2 - LOADING_CIRCLE_RADIUS - 1, LOADING_CIRCLE_RADIUS * 2 + 2, LOADING_CIRCLE_RADIUS * 2 + 2);
    }
    
    handleDrawCommands(diff);
  }
  else
  {
    showLoading();
  }
  last_pi_available = pi_available;

  handleSerial();
  handlePing();
  checkFreeMem();
}

void encoderInterrupt()
{
  encoderPressed = true;
}

void encoderCheckPosition()
{
  encoder.tick();
}
