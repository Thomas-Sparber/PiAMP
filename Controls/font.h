#pragma once

#include "Print.h"

#include "gfxfont.h"

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

class Font : public Print {

public:
  Font(void (*writePixel_fn)(int16_t,int16_t,uint16_t), void (*writeFillRect_fn)(int16_t,int16_t,int16_t,int16_t,int16_t)) :
    writePixel(writePixel_fn),
    writeFillRect(writeFillRect_fn),
    gfxFont(),
    cursor_x(0),
    cursor_y(0),
    textcolor(0xFFFF),
    textsize_x(1),
    textsize_y(1)
  { }

  void setTextSize(uint8_t s) {
    setTextSize(s, s);
  }

  void setTextSize(uint8_t s_x, uint8_t s_y) {
    textsize_x = (s_x > 0) ? s_x : 1;
    textsize_y = (s_y > 0) ? s_y : 1;
  }

  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  void setTextColor(uint16_t c) {
    textcolor = c;
  }
  
  void setFont(const GFXfont *f) {
    if (f) {          // Font struct pointer passed in?
      if (!gfxFont) { // And no current font struct?
        // Switching from classic to new font behavior.
        // Move cursor pos down 6 pixels so it's on baseline.
        cursor_y += 6;
      }
    } else if (gfxFont) { // NULL passed.  Current font struct defined?
      // Switching from new to classic font behavior.
      // Move cursor pos up 6 pixels so it's at top-left of char.
      cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
  }

  void getTextWidth(const char *str, uint16_t *w) {
    int16_t h;
    getTextBounds(str, w, &h);
  }

  void getTextHeight(const char *str, uint16_t *h) {
    int16_t w;
    getTextBounds(str, &w, h);
  }

  void getTextBounds(const char *str, uint16_t *w, uint16_t *h) {
    int16_t x1, y1;
    getTextBounds(str, 0, 0, &x1, &y1, w, h);
  }

  void getTextBounds(const char *str, int16_t x, int16_t y,
                                   int16_t *x1, int16_t *y1, uint16_t *w,
                                   uint16_t *h) {
  
    uint8_t c; // Current character
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
    // Bound rect is intentionally initialized inverted, so 1st char sets it
  
    *x1 = x; // Initial position is value passed in
    *y1 = y;
    *w = *h = 0; // Initial size is zero
  
    while ((c = *str++)) {
      // charBounds() modifies x/y to advance for each character,
      // and min/max x/y are updated to incrementally build bounding rect.
      charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }
  
    if (maxx >= minx) {     // If legit string bounds were found...
      *x1 = minx;           // Update x1 to least X coord,
      *w = maxx - minx + 1; // And w to bound rect width
    }
    if (maxy >= miny) { // Same for height
      *y1 = miny;
      *h = maxy - miny + 1;
    }
  }

  void charBounds(unsigned char c, int16_t *x, int16_t *y,
                                int16_t *minx, int16_t *miny, int16_t *maxx,
                                int16_t *maxy) {
  
    if (c == '\n') { // Newline?
      *x = 0;        // Reset x to zero, advance y by one line
      *y += textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') { // Not a carriage return; is normal char
      uint8_t first = pgm_read_byte(&gfxFont->first),
              last = pgm_read_byte(&gfxFont->last);
      if ((c >= first) && (c <= last)) { // Char present in this font?
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t gw = pgm_read_byte(&glyph->width),
                gh = pgm_read_byte(&glyph->height),
                xa = pgm_read_byte(&glyph->xAdvance);
        int8_t xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
        int16_t tsx = (int16_t)textsize_x, tsy = (int16_t)textsize_y,
                x1 = *x + xo * tsx, y1 = *y + yo * tsy, x2 = x1 + gw * tsx - 1,
                y2 = y1 + gh * tsy - 1;
        if (x1 < *minx)
          *minx = x1;
        if (y1 < *miny)
          *miny = y1;
        if (x2 > *maxx)
          *maxx = x2;
        if (y2 > *maxy)
          *maxy = y2;
        *x += xa * tsx;
      }
    }
  }

  void drawChar(int16_t x, int16_t y, unsigned char c,
                              uint16_t color, uint16_t bg, uint8_t size) {
    drawChar(x, y, c, color, bg, size, size);
  }

  void drawChar(int16_t x, int16_t y, unsigned char c,
                              uint16_t color, uint16_t bg, uint8_t size_x,
                              uint8_t size_y) {

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            writePixel(x + xo + xx, y + yo + yy, color);
          } else {
            writeFillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y, size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }
  }

  using Print::write;
  size_t write(uint8_t c) {
    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          drawChar(cursor_x, cursor_y, c, textcolor, textcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
    return 1;
  }

protected:
  inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
#ifdef __AVR__
    return &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
#else
    // expression in __AVR__ section may generate "dereferencing type-punned
    // pointer will break strict-aliasing rules" warning In fact, on other
    // platforms (such as STM32) there is no need to do this pointer magic as
    // program memory may be read in a usual way So expression may be simplified
    return gfxFont->glyph + c;
#endif //__AVR__
  }
  
  inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
#ifdef __AVR__
    return (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
#else
    // expression in __AVR__ section generates "dereferencing type-punned pointer
    // will break strict-aliasing rules" warning In fact, on other platforms (such
    // as STM32) there is no need to do this pointer magic as program memory may
    // be read in a usual way So expression may be simplified
    return gfxFont->bitmap;
#endif //__AVR__
  }

protected:
  void (*writePixel)(int16_t,int16_t,uint16_t);
  void (*writeFillRect)(int16_t,int16_t,int16_t,int16_t,int16_t);
  GFXfont *gfxFont;
  int16_t cursor_x;
  int16_t cursor_y;
  uint16_t textcolor;
  uint8_t textsize_x;
  uint8_t textsize_y;
  
}; //end class Font
