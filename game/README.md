# Introduction
My idea for a game was to have a road with 4 lanes on it.  The player controls a car on the bottom of the screen with the buttons.  The game will randomly place cones on the road that the player has to avoid with their car.  If the player hits an obstacle the game is over.

Controls:
The player moves the car to the right with switch 1.
The player moves the car to the left with switch 2.

Compiling:
After compiling with make in the parent directory with the libraries, go to the game directory and type "make". After compiled use "make load" to load the game on the msp430.

Issues:
- The graphics load very slowly.
- Collisions do not work because when I tried to make the collisions work by modifying the mlAdvance method.