#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#include "drawMenus.h"

int redrawScreen = 1;

u_char state = 1; // 1 for splash, 2 for controls, 3 for play

static u_int seed = 0;

void main() {
  configureClocks();
  lcd_init();
  p2sw_init(0xf);

  //enableWDTInterrupts();
  or_sr(0x8);

  clearScreen(COLOR_BLUE);
  drawString5x7(10, 10, "Switches:", COLOR_GREEN, COLOR_BLUE);

  u_int switches, i;
  char str[5];
  for (;;) {
    switches = p2sw_read();
    for (i = 0; i < 4; i++) {
      str[i] = (switches & (1<<i)) ? '-' : '0' + i;
    }
    str[4] = 0;
    drawString5x7(20, 20, str, COLOR_GREEN, COLOR_BLUE);
  }
}

