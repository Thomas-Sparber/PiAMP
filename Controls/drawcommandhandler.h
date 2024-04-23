#pragma once

#include "font.h"

#include <LCDWIKI_KBV.h>

struct DrawCommandHandler
{

  virtual ~DrawCommandHandler()
  {}

  virtual void handle(LCDWIKI_KBV *mylcd, Font *font, int16_t x, int16_t y, uint16_t w, uint16_t h) = 0;

}; //end struct DrawCommandHandler