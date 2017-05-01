#include <lcdutils.h>
#include <lcddraw.h>

#include "drawMenus.h"

u_char width = screenWidth, height = screenHeight;

void drawSplashScreen() {
  clearScreen(COLOR_BLUE);
  
  drawString5x7(5, 5, "Cars n Cones", COLOR_RED, COLOR_BLUE);
  drawString5x7(20, 40, "Last Score:", COLOR_RED, COLOR_BLUE);
  drawString5x7(100, 40, "0", COLOR_RED, COLOR_BLUE);
  drawString5x7(5, (height - 10), "Play", COLOR_RED, COLOR_BLUE);
  drawString5x7((width - 50), (height - 10), "Controls", COLOR_RED, COLOR_BLUE);
}

void drawGameScreen() {
  clearScreen(COLOR_BLUE);
  
  drawString5x7(5, (height-10), "L", COLOR_RED, COLOR_BLUE);
  drawString5x7((width-10), (height-10), "R", COLOR_RED, COLOR_BLUE);
  drawString5x7(((width/2) - 30), (height-10), "+", COLOR_RED, COLOR_BLUE);
  drawString5x7(((width/2) + 10), (height-10), "-", COLOR_RED, COLOR_BLUE);
}

void drawControlsScreen() {
  clearScreen(COLOR_BLUE);

  drawString5x7(5, 5, "How to play:", COLOR_RED, COLOR_BLUE);
  drawString5x7(5, 15, "Switch 1 moves car" , COLOR_RED, COLOR_BLUE);
  drawString5x7(5, 25, "to the left.", COLOR_RED, COLOR_BLUE);
  drawString5x7(5, 40, "Switch 4 moves car", COLOR_RED, COLOR_BLUE);
  drawString5x7(5, 50, "to the right.", COLOR_RED, COLOR_BLUE);

  // Didn't have time to implement this
  //drawString5x7(5, 65, "Switch 2 increases", COLOR_RED, COLOR_BLUE);
  //drawString5x7(5, 75, "the car's speed.", COLOR_RED, COLOR_BLUE);
  //drawString5x7(5, 90, "Switch 3 decreases", COLOR_RED, COLOR_BLUE);
  //drawString5x7(5, 100, "the car's speed.", COLOR_RED, COLOR_BLUE);
  drawString5x7(5, (height-10), "Back", COLOR_RED, COLOR_BLUE);
  
}
