#pragma once

#include "font.h"

#include <LCDWIKI_KBV.h>

struct DrawCommandHandler
{
  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
}; //end struct DrawCommandHandler