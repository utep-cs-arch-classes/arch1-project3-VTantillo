#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

int main() {
  configureClocks();
  lcd_init();

  u_int width = screenWidth, height = screenHeight;

  clearScreen(COLOR_GREEN);
  fillRectangle(5, 0, 25, 160, COLOR_RED);
  fillRectangle(30, 0, 25, 160, COLOR_BLUE);
  fillRectangle(55, 0 , 25, 160, COLOR_ORANGE);
  fillRectangle(80, 0 , 25, 160, COLOR_YELLOW);
  fillRectangle(105, 0, 25, 160, COLOR_DEEP);
}
