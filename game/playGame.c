#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#include "playGame.h"
#include "drawMenus.h"

// Constants to define the starting positions of the cones.
// Used in my version of mlAdvance so they can reset when they hit the bottom.
#define coneY 20
#define cone1x 25
#define cone2x 50
#define cone3x (screenWidth - 50)
#define cone4x (screenWidth - 25)

int redrawScreen = 1; // boolean if the screen needs to be redrawn
int gameOver = 0;
AbRect car = {abRectGetBounds, abRectCheck, {7, 10}}; // Rectangle for the car

void buzzer_init() {
  timerAUpmode();
  P2SEL &= ~(BIT6 | BIT7);
  P2SEL &= ~BIT7;
  P2SEL |= BIT6;
  P2DIR = BIT6;
}

void setPeriod(short cycles) {
  CCR0 = cycles;
  CCR1 = cycles >> 1;
}

AbRectOutline road = {  // Outline for the road
  abRectOutlineGetBounds, abRectOutlineCheck,
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer layer5 = { // Lane 4
  (AbShape *) &circle8,
  {cone4x, coneY},
  {0,0},{0,0},
  COLOR_ORANGE,
  0
};

Layer layer4 = { // Cone 3
  (AbShape *) &circle8,
  {cone3x, coneY},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer5
};

Layer layer3 = { // Cone 2
  (AbShape *) &circle8,
  {cone2x, coneY},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer4
};

Layer layer2 = { // Cone 1
  (AbShape *) &circle8,
  {cone1x, coneY},
  {0,0},{0,0},
  COLOR_ORANGE,
  &layer3
};

Layer layer1 = {  // Car
  (AbShape *) &car,
  {screenWidth - 20, screenHeight - 30},
  {0, 0}, {0,0},
  COLOR_RED,
  &layer2
};

Layer layer0 = {  // Road that the cones and car stay on
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

//All layers as mov layers
MovLayer ml5 = { &layer5, {0, 2}, 0 };    // 4th cone
MovLayer ml4 = { &layer4, {0, 2}, &ml5 }; // 3rd cone
MovLayer ml3 = { &layer3, {0, 2}, &ml4 }; // 2nd cone
MovLayer ml2 = { &layer2, {0, 2}, &ml3 }; // 1st cone
MovLayer ml1 = { &layer1, {1, 0}, &ml2 }; // car
MovLayer ml0 = { &layer0, {0, 0}, &ml1 }; // road

// Method shown in demos
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

// Modified version of mlAdvance to detect collisions
/*
void mlAdvance(Region *fence, MovLayer *cones, MovLayer *car)
{
  u_char axis;

  Vec2 newCarPos;
  Vec2 newConePos;

  Region carBoundary;
  Region coneBoundary;

  vec2Add(&newCarPos, &car->layer->posNext, &car->velocity);
  abShapeGetBounds(car->layer->abShape, &newCarPos, &carBoundary); // car boundaries
		   
  for (; cones; cones = cones->next) {
    vec2Add(&newConePos, &cones->layer->posNext, &cones->velocity);
    abShapeGetBounds(cones->layer->abShape, &newConePos, &coneBoundary);

    if (coneBoundary.botRight.axes[1] < fence->botRight.axes[1]) {
      score++;
      if (cones == &ml2) {
	Vec2 conePos = {cone1x, coneY};
	cones->layer->posNext = conePos;
      } else if (cones == &ml3) {
	Vec2 conePos = {cone2x, coneY};
	cones->layer->posNext = conePos;
      } else if (cones == &ml4) {
	Vec2 conePos = {cone3x, coneY};
	cones->layer->posNext = conePos;
      } else if (cones == &ml5) {
	Vec2 conePos = {cone4x, coneY};
	cones->layer->posNext = conePos;
      }
    }

    if (abShapeCheck(cones->layer->abShape, &newConePos, &newCarPos)) {
      gameOver = 1;
    }
    
  }
}
*/
 // Version of mlAdvance given in demos
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

  if (gameOver) {
    return;
  }
  for(;;) {
    while(!redrawScreen) {
      or_sr(0x10);
    }
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
  }
}

void __interrupt_vec(WDT_VECTOR) WDT() {
  static short count = 0;
  count++;
  if (count == 15) {
    u_int switches = p2sw_read();
    int sw1 = (switches & (1 << 0)) ? 0 : 1;
    int sw4 = (switches & (1 << 3)) ? 0 : 1;

    /// Moves the car back and forth across the bottom of the screen
    if(sw1) {
      ml1.velocity.axes[0] = -2;
      setPeriod(500);
    } else if(sw4) {
      ml1.velocity.axes[0] = 2;
      setPeriod(500);
    } else {
      ml1.velocity.axes[0] = 0;
      setPeriod(0);
    }
    mlAdvance(&ml1, &roadFence);
    // My version of mlAdvance that was supposed to detect the collisions
    //mlAdvance(&roadFence, &ml2, &ml1);
    redrawScreen = 1;
    count = 0;
  }
}
