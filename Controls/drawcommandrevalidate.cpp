#include "drawcommandrevalidate.h"

#include "graphics.h"

void DrawCommandRevalidate::handle(LCDWIKI_KBV *mylcd, Font *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  graphics->revalidate();
}
