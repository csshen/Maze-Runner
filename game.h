#pragma once
#include "mbed.h"
#include "physics.h"
#include "doublely_linked_list.h"

/* A structure for holding all the game inputs */
struct GameInputs
{
    // Measurements from the accelerometer
    double ax, ay, az;
    // Pushbutton states
    bool left, right, up, down;
};
//////////////////////////
// Arena Element structs
//////////////////////////
/** The basic ArenaElement structure.
    Every item in an arena DLinkedList should be able to be cast to this struct.
    The type member is used to tell what other struct this element might be.
*/
struct ArenaElement {
    int type;
};

// Element types
#define WALL        0
#define BALL        1
#define GOAL        2
#define POTHOLE     3
#define GHOST       4
#define MUD         5
#define MUSHROOM    6

/** An ArenaElement struct representing the ball. */
struct Ball {
    // ArenaElement type (must be first element)
    int type;
    // Drawing info
    int x, y;
};
/** ball constructor */
Ball* create_ball(int x, int y);
/** An ArenaElement struct representing the goal. */
struct Goal {
    int type;
    int x, y, r;  
    bool shouldReDraw;  
};
/** goal constrcutor **/
Goal* newGoal(int x, int y, int r);
/** goal draw method **/
void draw_goal(Goal* goal);
/** touching ball method **/
bool touching_goal(Goal* goal, Physics* curr, int mode);

/** An ArenaElement struct representing a pothole. */
struct Pothole {
    int type;
    int x, y, r;
    bool shouldReDraw;    
};
/** pothole constructor **/
Pothole* newPothole(int x, int y, int r);
/** draw pothole method **/
void draw_pothole(Pothole* pothole);
/** returns true if ball is touching pothole **/
bool touching_pothole(Pothole* pothole, Physics* curr, int mode);

/** An ArenaElement struct representing a mud patch. */
struct Mud {
    int type;
    int x, y, h, w;
    bool shouldReDraw;
};
// constructor
Mud* newMud(int x, int y, int h, int w);
// draw mud
void drawMud(Mud* mud);
// touching mud method
bool touching_mud(Mud* mud, Physics* curr, int mode);

/** An ArenaElement struct representing a MUSHROOM. */
struct Mushroom {
    int type;
    int x, y;
    bool shouldReDraw, eaten;
};
// constructor
Mushroom* newMushroom(int x, int y);
// draw mushroom
void drawMushroom(Mushroom* mush);
// touching mush method
bool touching_mushroom(Mushroom* mush, Physics* curr, int mode);
// erase mush
void erase_mush(Mushroom* mush);


/////////////////////////
// ArenaElement helpers
/////////////////////////
/** Erases the ball */
void erase_ball(Ball* ball);
/** Draws the ball at the current state */
void draw_ball(Ball* ball, Physics* state);

/** An ArenaElement struct representing a GHOST. */
struct Ghost {
    int type;
    double x,y,d,s,xp,yp,v; // curr(x,y), dir, span, prev(x,y), velocity
    double ox, oy; // anchor point
    bool state;
};
/** ghost constructor */
Ghost* newGhost(int x, int y, int d, int s, int v);
/** draw ghost */
void drawGhost(Ghost* ghost);
/** erase ghost */
void eraseGhost(Ghost* ghost);
/** update ghost */
void updateGhost(Ghost* ghost, float delta);
/** returns true when touching ghost */
bool touching_ghost(Ghost* ghost, Physics* curr);


///////////////////////////
// Game control functions 
///////////////////////////
/* Reads all game inputs */
GameInputs read_inputs();

/* Performs a single physics update. */
int update_game(DLinkedList* arena, Physics* prev, GameInputs inputs, float delta, 
                DLinkedList* saveStates, Timer timer);

/* Implements the game loop */
int run_game(DLinkedList* arena, Physics* state);

/* collision sound */
void playOnCollision();
