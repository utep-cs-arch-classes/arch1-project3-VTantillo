#include <msp430.h>
#include <libtimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6

void main() {
  P1DIR |= GREEN_LED; //Green led is on when cpu is on.
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(1);

  shapeInit();

  //layerGetBounds(&fieldLayer, &fieldFence);

  enableWDTInterrupts();    //Enable periodic interrupts
  or_sr(0x8);               //Enable interrupts 

}
