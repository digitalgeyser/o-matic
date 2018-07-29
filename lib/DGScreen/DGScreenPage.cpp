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
  this->buttonFirst = NULL;
}

DGScreenButton *DGScreenPage::firstButton() {
  return this->buttonFirst;
}

void DGScreenPage::addButton(DGScreenButton *button) {
  button->nextButton = this->buttonFirst;
  this->buttonFirst = button;
}

DGColor DGScreenPage::fg() { return this->fgColor; }
DGColor DGScreenPage::bg() { return this->bgColor; }
void DGScreenPage::setFg(DGColor fg) { this-> fgColor = fg; }
void DGScreenPage::setBg(DGColor bg) { this-> bgColor = bg; }
