#include <msp430.h>
#include "game.h"

void __interrupt_vec(WDT_VECTOR) WDT(){
  static short count = 0;
  count ++;
  if (count == 15) {
    if (p2sw_read())
  }
}
