#include "drawcommandrevalidate.h"

#include "graphics.h"

void DrawCommandRevalidate::handle(LCDWIKI_KBV *mylcd, Font *font, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  graphics->revalidate();
}
