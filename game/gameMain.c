#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <shape.h>
#include <p2switches.h>

#include "drawMenus.h"
#include "playGame.h"

char* displayScore(int score);

int score = 0;

u_char state; // 1 for splash, 2 for controls, 3 for play

static u_int seed = 0; // Was going to be used by the playGame to generate which cones were up.

void main() {
  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(0xf);

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

    // Make this switch statement my assm. code
    //stateMachine(state);
    
    switch(state) {
    case 1:  // On splash screen
      if (switch1) { // Start playing game
	state = 3;
	playGame();
      } else if (switch2) { // decrease seed in the background
	seed--;
      } else if (switch3) { // increase seed in the background
	seed++;
      } else if (switch4) { // go to the controls screen
	state = 2;
	drawControlsScreen();
      }
      break;
    case 2:  // On the controls screen
      if (switch1) {  // Go back to spalsh screen
	state = 1;
	drawSplashScreen();
      }
      break;
    default:
      state = 1;
      drawSplashScreen();
      break;
    }
  }

}   // end main

