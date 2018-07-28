/*
 * (c) Digital Geyser.
 *
 * Screen library.
 */

#include "DGScreen.h"

DGScreenPage::DGScreenPage(const char *title, DGColor fg, DGColor bg) {
  this->title = title;
  this->fgColor = fg;
  this->bgColor = bg;
  this->nextPage = NULL;
  this->firstArea = NULL;
}

DGScreenArea *DGScreenPage::area() {
  return this->firstArea;
}

void DGScreenPage::addArea(DGScreenArea *area) {
  area->nextArea = this->firstArea;
  this->firstArea = area;
}

DGColor DGScreenPage::fg() { return this->fgColor; }
DGColor DGScreenPage::bg() { return this->bgColor; }
void DGScreenPage::setFg(DGColor fg) { this-> fgColor = fg; }
void DGScreenPage::setBg(DGColor bg) { this-> bgColor = bg; }
