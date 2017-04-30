#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#include "drawMenus.h"

int redrawScreen = 1;

u_char state; // 1 for splash, 2 for controls, 3 for play

static u_int seed = 0;

void main() {
  configureClocks();
  lcd_init();
  p2sw_init(0xf);

  //enableWDTInterrupts();
  or_sr(0x8);

  state = 0; // Initially should show the splash screen
  
  u_int switches;   //value from switch library
  u_char switch1;   //value for just switch 1
  u_char switch2;   //value for just switch 2
  u_char switch3;   //value for just switch 3
  u_char switch4;   //value for just switch 4

  for(;;) {
    switches = p2sw_read();
    switch1 = (switches & (1 << 0)) ? 0 : 1;
    switch2 = (switches & (1 << 1)) ? 0 : 1;
    switch3 = (switches & (1 << 2)) ? 0 : 1;
    switch4 = (switches & (1 << 3)) ? 0 : 1;

    switch(state) {
    case 1:
      if (switch1) {
	state = 3;
	drawGameScreen(); // going to need to change this for actually playing the game
      } else if (switch2) {
	seed--;
      } else if (switch3) {
	seed++;
      } else if (switch4) {
	state = 2;
	drawControlsScreen();
      }
      break;
    case 2:
      if (switch1) {
	state = 1;
	drawSplashScreen();
      }
      break;
    case 3:
      // Put logic for playing the game here
      break;
    default:
      state = 1;
      drawSplashScreen();
      break;
    }
  }
  /*
  char str[5];
  
  for (;;) {
    switches = p2sw_read();
    for (i = 0; i < 4; i++) {
      str[i] = (switches & (1<<i)) ? '-' : '0' + i;
    }
    str[4] = 0;
    drawString5x7(20, 20, str, COLOR_GREEN, COLOR_BLUE);
  }
  */
}

