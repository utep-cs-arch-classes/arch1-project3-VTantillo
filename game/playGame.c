#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#include "playGame.h"
#include "drawMenus.h"

int redrawScreen = 1; // boolean if the screen needs to be redrawn

AbRect car = {abRectGetBounds, abRectCheck, {7, 10}};

AbRectOutline road = {
  abRectOutlineGetBounds, abRectOutlineCheck,
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer layer5 = { // Lane 4
  (AbShape *) &circle8,
  {screenWidth - 25, 20},
  {0,0},{0,0},
  COLOR_ORANGE,
  0
};

Layer layer4 = { // Lane 3
  (AbShape *) &circle8,
  {screenWidth - 50, 20},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer5
};

Layer layer3 = { // Lane 2
  (AbShape *) &circle8,
  {50, 20},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer4
};

Layer layer2 = { // Lane 1
  (AbShape *) &circle8,
  {25, 20},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer3
};

Layer layer1 = {
  (AbShape *) &car,
  {screenWidth - 20, screenHeight - 30},
  {0, 0}, {0,0},
  COLOR_RED,
  &layer2
};

Layer layer0 = {
  (AbShape *) &road,
  {screenWidth/2, screenHeight/2 - 5},
  {0,0}, {0,0},
  COLOR_BLACK,
  &layer1
};

typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml5 = { &layer5, {0, 2}, 0 };
MovLayer ml4 = { &layer4, {0, 2}, &ml5 };
MovLayer ml3 = { &layer3, {0, 2}, &ml4 };
MovLayer ml2 = { &layer2, {0, 2}, &ml3 };
MovLayer ml1 = { &layer1, {1, 0}, &ml2 };
MovLayer ml0 = { &layer0, {0, 0}, &ml1 };

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

u_int bgColor = COLOR_WHITE;
Region roadFence;

void playGame() {
  shapeInit();
  
  layerInit(&layer0);
  layerDraw(&layer0);

  layerGetBounds(&layer0, &roadFence);

  enableWDTInterrupts();
  or_sr(0x8);
  
  for(;;) {
    while(!redrawScreen) {
      or_sr(0x10);
      
    }
    redrawScreen = 0;
    movLayerDraw(&ml1, &layer0);
  }
}

void __interrupt_vec(WDT_VECTOR) WDT() {
  static short count = 0;
  count++;
  if (count == 15) {
    u_int switches = p2sw_read();
    int sw1 = (switches & (1 << 0)) ? 0 : 1;
    int sw4 = (switches & (1 << 3)) ? 0 : 1;

    if(sw1) {
      ml1.velocity.axes[0] = -2;
    } else if(sw4) {
      ml1.velocity.axes[0] = 2;
    } else {
      ml1.velocity.axes[0] = 0;
    }
    
    mlAdvance(&ml1, &roadFence);
    redrawScreen = 1;
    count = 0;
  }
}
