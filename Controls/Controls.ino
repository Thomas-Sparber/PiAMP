/***********************************************************************************
*This program is a demo of how to display picture and 
*how to use rotate function to display string.
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                : display_graphics.ino
* Hardware Environment: Arduino UNO&Mega2560
* Build Environment   : Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
*pin usage as follow:
*                  LCD_CS  LCD_CD  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK 
*     Arduino Uno    A3      A2      A1      A0      A4      10     11     12      13                            
*Arduino Mega2560    A3      A2      A1      A0      A4      10     11     12      13                           

*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*     Arduino Uno    8       9       2       3       4       5       6       7
*Arduino Mega2560    8       9       2       3       4       5       6       7 

*Remember to set the pins to suit your display module!
*
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include "font.h"
#include "Fonts/FreeMono9pt7b.H"

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_KBV mylcd(320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

//define some colour values
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PIXEL_NUMBER  (mylcd.Get_Display_Width()/4)

uint16_t s_width = mylcd.Get_Display_Width();  
uint16_t s_heigh = mylcd.Get_Display_Height();

//clear screen
void fill_screen_test()
{
  mylcd.Fill_Screen(BLACK);  
  mylcd.Fill_Screen(RED);
  mylcd.Fill_Screen(GREEN);
  mylcd.Fill_Screen(BLUE);
  mylcd.Fill_Screen(BLACK);
}

//display some strings
void text_test()
{
  mylcd.Set_Text_Mode(0);
  
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(1);
  mylcd.Print_String("Hello World!", 0, 0);
  
  mylcd.Set_Text_colour(YELLOW);
  mylcd.Set_Text_Size(2);
  mylcd.Print_Number_Float(1234.56,2,0, 8, '.', 0, ' ');
  
  mylcd.Set_Text_colour(RED);
  mylcd.Set_Text_Size(3);
  //mylcd.Print_String("DEADBEEF", 0, 24); 
  mylcd.Print_Number_Int(0xDEADBEF, 0, 24, 0, ' ', 16); 

  mylcd.Set_Text_colour(GREEN);
  mylcd.Set_Text_Size(5);  
  mylcd.Print_String("Groop", 0, 56); 

  mylcd.Set_Text_Size(2);
  mylcd.Print_String("I implore thee,", 0, 96);

  mylcd.Set_Text_Size(1);
  mylcd.Print_String("my foonting turlingdromes.", 0, 112);
  mylcd.Print_String("And hooptiously drangle me", 0, 120);
  mylcd.Print_String("with crinkly bindlewurdles,", 0, 128);
  mylcd.Print_String("Or I will rend thee", 0, 136);
  mylcd.Print_String("in the gobberwarts", 0, 144);
  mylcd.Print_String("with my blurglecruncheon,", 0, 152);
  mylcd.Print_String("see if I don't!", 0, 160);
}

//draw some oblique lines
void lines_test(void)
{
    mylcd.Fill_Screen(BLACK);
      mylcd.Set_Draw_color(GREEN);
    int i = 0;   
    for(i = 0; i< mylcd.Get_Display_Width();i+=5)
    {
       mylcd.Draw_Line(0, 0, i, mylcd.Get_Display_Height()-1);
     }
     for(i = mylcd.Get_Display_Height()-1; i>= 0;i-=5)
     {
       mylcd.Draw_Line(0, 0, mylcd.Get_Display_Width()-1, i);
     }
     
     mylcd.Fill_Screen(BLACK); 
       mylcd.Set_Draw_color(RED);
    for(i = mylcd.Get_Display_Width() -1; i>=0;i-=5)
    {
      mylcd.Draw_Line(mylcd.Get_Display_Width()-1, 0, i, mylcd.Get_Display_Height()-1);
     }
    for(i = mylcd.Get_Display_Height()-1; i>=0;i-=5)
    {
      mylcd.Draw_Line(mylcd.Get_Display_Width()-1, 0, 0, i);
     }
     
     mylcd.Fill_Screen(BLACK); 
      mylcd.Set_Draw_color(BLUE);
     for(i = 0; i < mylcd.Get_Display_Width();i+=5)
    {
      mylcd.Draw_Line(0, mylcd.Get_Display_Height()-1, i, 0);
     }
     for(i = 0; i < mylcd.Get_Display_Height();i+=5)
    {
      mylcd.Draw_Line(0, mylcd.Get_Display_Height()-1, mylcd.Get_Display_Width()-1, i);
     }

     mylcd.Fill_Screen(BLACK);
      mylcd.Set_Draw_color(YELLOW);
     for(i = mylcd.Get_Display_Width()-1; i >=0;i-=5)
    {
      mylcd.Draw_Line(mylcd.Get_Display_Width()-1, mylcd.Get_Display_Height()-1, i, 0);
     }
     for(i = 0; i<mylcd.Get_Display_Height();i+=5)
    {
      mylcd.Draw_Line(mylcd.Get_Display_Width()-1, mylcd.Get_Display_Height()-1, 0, i);
     }
}

//draw some vertical lines and horizontal lines
void h_l_lines_test(void)
{
    int i=0;
   mylcd.Fill_Screen(BLACK);
     mylcd.Set_Draw_color(GREEN);
    for(i =0;i<mylcd.Get_Display_Height();i+=5)
    {
      mylcd.Draw_Fast_HLine(0,i,mylcd.Get_Display_Width()); 
      delay(5);
    }
     mylcd.Set_Draw_color(BLUE);
     for(i =0;i<mylcd.Get_Display_Width();i+=5)
    {
      mylcd.Draw_Fast_VLine(i,0,mylcd.Get_Display_Height()); 
           delay(5);
    }
}

//draw some rectangles
void rectangle_test(void)
{
  int i = 0;
   mylcd.Fill_Screen(BLACK);
     mylcd.Set_Draw_color(GREEN);
   for(i = 0;i<mylcd.Get_Display_Width()/2;i+=4)
   {
      mylcd.Draw_Rectangle(i,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2+i,mylcd.Get_Display_Width()-1-i,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2-i);  
        delay(5);
   }
}

//draw some filled rectangles
void fill_rectangle_test(void)
{
  int i = 0;
   mylcd.Fill_Screen(BLACK);
     mylcd.Set_Draw_color(YELLOW);
   mylcd.Fill_Rectangle(0,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2,mylcd.Get_Display_Width()-1,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2);
    mylcd.Set_Draw_color(MAGENTA);
   for(i = 0;i<mylcd.Get_Display_Width()/2;i+=4)
   {
      mylcd.Draw_Rectangle(i,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2+i,mylcd.Get_Display_Width()-1-i,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2-i);  
        delay(5);
   }
   for(i = 0;i<mylcd.Get_Display_Width()/2;i+=4)
   {
       mylcd.Set_Draw_color(random(255), random(255), random(255));
      mylcd.Fill_Rectangle(i,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2+i,mylcd.Get_Display_Width()-1-i,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2-i);  
        delay(5);
   }
}

//draw some filled circles
void fill_circles_test(void)
{
  int r=10,i=0,j=0;
  mylcd.Fill_Screen(BLACK);
   mylcd.Set_Draw_color(MAGENTA);
  for(i=r;i<mylcd.Get_Display_Width();i+=2*r)
  {
    for(j=r;j<mylcd.Get_Display_Height();j+=2*r)
    {
      mylcd.Fill_Circle(i, j, r);
    }
  }
}

//draw some circles
void circles_test(void)
{
  int r=10,i=0,j=0;
   mylcd.Set_Draw_color(GREEN);
  for(i=0;i<mylcd.Get_Display_Width()+r;i+=2*r)
  {
    for(j=0;j<mylcd.Get_Display_Height()+r;j+=2*r)
    {
      mylcd.Draw_Circle(i, j, r);
    }
  }  
}

//draw some triangles
void triangles_test(void)
{
   int i = 0;
   mylcd.Fill_Screen(BLACK);
   for(i=0;i<mylcd.Get_Display_Width()/2;i+=5)
   {
      mylcd.Set_Draw_color(0,i+64,i+64);
      mylcd.Draw_Triangle(mylcd.Get_Display_Width()/2-1,mylcd.Get_Display_Height()/2-1-i,
                    mylcd.Get_Display_Width()/2-1-i,mylcd.Get_Display_Height()/2-1+i,
                    mylcd.Get_Display_Width()/2-1+i,mylcd.Get_Display_Height()/2-1+i);                   
   }
}

//draw some filled triangles
void fill_triangles_test(void)
{
   int i = 0;
   mylcd.Fill_Screen(BLACK);
    for(i=mylcd.Get_Display_Width()/2-1;i>0;i-=5)
   {
    
      mylcd.Set_Draw_color(0,i+64,i+64);
      mylcd.Fill_Triangle(mylcd.Get_Display_Width()/2-1,mylcd.Get_Display_Height()/2-1-i,
                    mylcd.Get_Display_Width()/2-1-i,mylcd.Get_Display_Height()/2-1+i,
                    mylcd.Get_Display_Width()/2-1+i,mylcd.Get_Display_Height()/2-1+i);                   
      mylcd.Set_Draw_color(i,0,i);
      mylcd.Draw_Triangle(mylcd.Get_Display_Width()/2-1,mylcd.Get_Display_Height()/2-1-i,
                    mylcd.Get_Display_Width()/2-1-i,mylcd.Get_Display_Height()/2-1+i,
                    mylcd.Get_Display_Width()/2-1+i,mylcd.Get_Display_Height()/2-1+i);                   
   }
}

//draw some round rectangles
void round_rectangle(void)
{
   int i = 0;
   mylcd.Fill_Screen(BLACK);
     for(i = 0;i<mylcd.Get_Display_Width()/2;i+=4)
   {
       mylcd.Set_Draw_color(255-i,0,160-i);
      mylcd.Draw_Round_Rectangle(i,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2+i,mylcd.Get_Display_Width()-1-i,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2-i,8);
        delay(5);
   } 
}

//draw some filled round rectangles
void fill_round_rectangle(void)
{
     int i = 0;
   mylcd.Fill_Screen(BLACK);
     for(i = 0;i<mylcd.Get_Display_Width()/2;i+=4)
   {
       mylcd.Set_Draw_color(255-i,160-i,0);
      mylcd.Fill_Round_Rectangle(i,(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2+i,mylcd.Get_Display_Width()-1-i,mylcd.Get_Display_Height()-(mylcd.Get_Display_Height()-mylcd.Get_Display_Width())/2-i,8);
        delay(5);
   } 
}

uint8_t img_data[] = {};
uint32_t img_data_pos = 0;

uint8_t fpread() {
  return img_data[img_data_pos++];
}

void fpread(uint8_t *out, uint32_t size) {
  memcpy(out, img_data, size);
  img_data_pos += size;
}

void fpseek(uint32_t pos) {
  img_data_pos = pos;
}

uint16_t read_16()
{
    uint8_t low;
    uint16_t high;
    low = fpread();
    high = fpread();
    return (high<<8)|low;
}

uint32_t read_32()
{
    uint16_t low;
    uint32_t high;
    low = read_16();
    high = read_16();
    return (high<<8)|low;   
 }

uint32_t analysis_bpm_header(bool &isBmp)
{
    if(read_16() != 0x4D42)
    {
      isBmp = false;
      return 0;
    }
    //get bpm size
    read_32();
    //get creator information
    read_32();
    //get offset information
    uint32_t bmp_offset = read_32();
    //get DIB infomation
    read_32();
    //get width and heigh information
    uint32_t bpm_width = read_32();
    uint32_t bpm_heigh = read_32();
    if((bpm_width != s_width) || (bpm_heigh != s_heigh))
    {
      isBmp = false;
      return 0;
    }
    if(read_16() != 1)
    {
        isBmp = false;
      return 0;
    }
    read_16();
    if(read_32() != 0)
    {
      isBmp = false;
      return 0;
     }
     isBmp = true;
      return bmp_offset;
}

void draw_bmp_picture()
{
  uint16_t i,j,k,l,m=0;
  uint8_t bpm_data[PIXEL_NUMBER*3] = {0};
  uint16_t bpm_color[PIXEL_NUMBER];
  bool isBmp;
  uint32_t bmp_offset = analysis_bpm_header(isBmp);
  if(!isBmp)return;
  fpseek(bmp_offset);
  for(i = 0;i < s_heigh;i++)
  {
    for(j = 0;j<s_width/PIXEL_NUMBER;j++)
    {
      m = 0;
      fpread(bpm_data,PIXEL_NUMBER*3);
      for(k = 0;k<PIXEL_NUMBER;k++)
      {
        bpm_color[k]= mylcd.Color_To_565(bpm_data[m+2], bpm_data[m+1], bpm_data[m+0]); //change to 565
        m +=3;
      }
      for(l = 0;l<PIXEL_NUMBER;l++)
      {
        mylcd.Set_Draw_color(bpm_color[l]);
        mylcd.Draw_Pixel(j*PIXEL_NUMBER+l,i);
      }    
     }
   }    
}

void setup() 
{
  Serial.begin(9600);
  mylcd.Init_LCD();
  Serial.println(mylcd.Read_ID(), HEX);
  //draw_bmp_picture();
  Font f(&mylcd);
  f.setFont(&FreeMono9pt7b);
  f.setCursor(100,100);
  f.print("Test");
  
  delay(5000);
  fill_screen_test();
  delay(500);
  text_test();
  delay(500);
  lines_test();
  delay(500);
  h_l_lines_test();
  delay(500);
  rectangle_test();
  delay(500);
  fill_rectangle_test();
  delay(500);
  fill_circles_test();
  delay(500);
  circles_test();
  delay(500);
  triangles_test();
  delay(500);
  fill_triangles_test();
  delay(500);
   round_rectangle();
   delay(500);
   fill_round_rectangle();
   delay(3000);
}

void loop() 
{
  //rotate for four directions
 for(uint8_t rotation=0; rotation<4; rotation++) 
 {
    mylcd.Set_Rotation(rotation);
    text_test();
    delay(2000);
 }
}
