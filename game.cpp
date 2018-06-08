#include "game.h"

#include "globals.h"
#include "physics.h"
#include "wall.h"
// additional imports
#include "math.h"
// Speaker
#include "SongPlayer.h"

int score = 0; // score for each level
int state = 0; // current save state (4) available
int numOfStates = 0; // number of saved states: max 4 per level
int button_tick = 0; // rate limiter for buttons
int saveRestore = 0;
Physics squareOne;

/** Erases the ball from the screen by drawing over it with the background color. */
void erase_ball(Ball* ball)
{
    // TODO: Draw background color over curriously drawn ball location
    uLCD.filled_circle(ball->x, ball->y, radius, backgroundColor);
}

/** Draws the ball on the screen at the updated location (according to the state) */
void draw_ball(Ball* ball, Physics* state)
{
    // TODO: Save that updated ball position for later erasing
    ball->x = state->px;
    ball->y = state->py;
    // TODO: Draw ball in its updated location
    if (squish == 0) {
        uLCD.filled_circle(ball->x, ball->y, radius, ball_color);
    } else if (squish == 1) { // horizontal
        uLCD.filled_circle(ball->x+2, ball->y, radius/2,ball_color); // 1/2 squish
        uLCD.filled_circle(ball->x-2, ball->y, radius/2,ball_color);
        wait(.015);
        uLCD.filled_circle(ball->x+1, ball->y, radius/4,ball_color); // 1/4 squish
        uLCD.filled_circle(ball->x-1, ball->y, radius/4,ball_color);
        uLCD.filled_circle(ball->x+3, ball->y, radius/4,ball_color);
        uLCD.filled_circle(ball->x-3, ball->y, radius/4,ball_color);
        wait(.015);
        uLCD.filled_circle(ball->x+2, ball->y, radius/2,ball_color); // 1/2 squish
        uLCD.filled_circle(ball->x-2, ball->y, radius/2,ball_color);
    } else if (squish == 2) { //vertical
        uLCD.filled_circle(ball->x, ball->y+2, radius/2,ball_color); // 1/2 squish
        uLCD.filled_circle(ball->x, ball->y-2, radius/2,ball_color);
        wait(.015);
        uLCD.filled_circle(ball->x, ball->y+1, radius/4,ball_color); // 1/4 squish
        uLCD.filled_circle(ball->x, ball->y-1, radius/4,ball_color);
        uLCD.filled_circle(ball->x, ball->y+3, radius/4,ball_color);
        uLCD.filled_circle(ball->x, ball->y-3, radius/4,ball_color);
        wait(.015);
        uLCD.filled_circle(ball->x, ball->y+2, radius/2,ball_color); // 1/2 squish
        uLCD.filled_circle(ball->x, ball->y-2, radius/2,ball_color);
    }
    squish = 0;
}

/** Reads inputs to the game, such as accelerometer and buttons */
GameInputs read_inputs()
{
    GameInputs inputs = {0};

    // TODO: Get acceleration vector from accelerometer

    double ax, ay, az;
    acc.readXYZGravity(&ax,&ay,&az);
    
    int factor = 300; // for scaling accelerometer
    inputs.ax = ay * factor;
    inputs.ay = ax * factor;
    inputs.az = az * factor;
    

    // TODO: Read buttons
    inputs.left = left_pb;
    inputs.right = right_pb;
    inputs.up = up_pb;
    inputs.down = down_pb;

    return inputs;
}

int update_game(DLinkedList* arena, Physics* curr, GameInputs inputs, float delta, DLinkedList* saveStates, Timer timer)
{
    // reset if next level
    button_tick = (timer.read_ms() < button_tick) ? timer.read_ms() : button_tick;

    ///////////////////////////////
    // Prepare for physics update
    ///////////////////////////////
    // Make a copy of the current state for modification
    Physics next = *curr;
    // No acceleration unless the ArenaElements apply them. (Newton's 1st law)
    next.ax = next.ay = 0.0;
    
    // CHECK PUSHBUTTONS
    
    // SKIP: level advance, record score, but DO NOT add to leaderboard
    if (!inputs.left && !inputs.right) {
        finalScore += score;
        currentScore = score;
        didSkip = true;
        return 1;
    }
    
    // push button combo CHEAT CODE
    if (inputs.right && !inputs.down && inputs.up && !inputs.left) {
        ball_color = 0xFFFF00;
        invincible = true;
    }

    // UP BUTTON - ADD STATE
    if (!inputs.up && inputs.down && inputs.left && inputs.right && numOfStates < 4 &&
        timer.read_ms() - button_tick > 500) {
        if (numOfStates == 0) {
            myled1 = 1;    
        } else if (numOfStates == 1) {
            myled2 = 1;    
        } else if (numOfStates == 2) {
            myled3 = 1;
        } else if (numOfStates == 3) {
            myled4 = 1;
        }
        
        Physics* saved = (Physics*) malloc(sizeof(Physics)); // allocate in heap
        saved->px = curr->px;
        saved->py = curr->py;
        saved->vx = 0.0;
        saved->vy = 0.0;
        saved->ax = 0.0;
        saved->ay = 0.0;
        insertHead(saveStates, saved);
        numOfStates++;
        button_tick = timer.read_ms();
        state = -1;
    }
    // restore head if only down is pressed
    if (inputs.up && !inputs.down && inputs.left && inputs.right &&
        timer.read_ms() - button_tick > 500 && saveStates->size != 0) {
        state = (state+1)%(saveStates->size);
        LLNode* focus = saveStates->head;
        int i = 0;
        while (i < state) {
            focus = focus->next;
            i++;    
        }
        next = *((Physics*) focus->data);
        *curr = next;
        button_tick = timer.read_ms();

    }
    
    // Loop over all arena elements
    bool died = false;
    ArenaElement* elem = (ArenaElement*)getHead(arena);
    do {
        switch(elem->type) {
            case WALL:
                do_wall(&next, curr, (Wall*) elem, delta);
                break;
            case BALL:
                next.ax = inputs.ax*friction;
                next.ay = inputs.ay*friction;
                forward_euler(&next, delta);
                break;
            case GOAL:
                if (touching_goal((Goal*) elem, curr, 0)){
                    finalScore += score;
                    currentScore = score;
                    return 1;
                }
                if (touching_goal((Goal*) elem, curr, 1)){
                    ((Goal*) elem)->shouldReDraw = true;
                }
                break;
            case POTHOLE:
                 if (touching_pothole((Pothole*) elem, curr, 0) && !invincible) {
                    died = true;
                    float ns[] = {1200, 600};
                    float ds[] = {.15, .15 };
                    speaker.PlaySong(2,false,ns,ds,.04);
                    draw_pothole((Pothole*) elem); 
                }
                if (touching_pothole((Pothole*) elem, curr, 1)) {
                    ((Pothole*) elem)->shouldReDraw = true;
                }
                break;
            case GHOST:
                updateGhost((Ghost*) elem, delta);
                if (touching_ghost((Ghost*) elem, curr)) {
                    died = true;
                    float ns[] = {1200, 600};
                    float ds[] = {.15, .15 };
                    speaker.PlaySong(2,false,ns,ds,.04);
                }
                break;
            case MUD:
                if (touching_mud((Mud*) elem, curr, 1)) {
                    ((Mud*) elem)->shouldReDraw = true;
                }
                if (mudFirstTime && touching_mud((Mud*) elem, curr, 0) && !invincible) {
                    next.vx /= 6;
                    next.vy /= 6;
                    mudFirstTime = false;    
                }
                if (touching_mud((Mud*) elem, curr, 0) && !invincible) {
                    friction = .2;
                } else {
                    friction = 1;
                    mudFirstTime = true;
                }
                break;
            case MUSHROOM:
                if (    touching_mushroom((Mushroom*) elem, curr, 1)     ) {
                    ((Mushroom*) elem)->shouldReDraw = true;
                }
                if (touching_mushroom((Mushroom*) elem, curr, 0) && !((Mushroom*) elem)->eaten) {
                    radius = 6;
                    ((Mushroom*) elem)->eaten = true;
                    erase_mush( (Mushroom*) elem);
                    float oneUp[3] = {329.628, 523.251, 659.255};
                    float oneUpDurs[3] = {.15,.15,.15};
                    speaker.PlaySong(3,false,oneUp,oneUpDurs);
                    wait(.5);
                }
                break;
            default:
                break;
        }
    } while(elem = (ArenaElement*)getNext(arena));

    // Last thing! Update state, so it will be saved for the next iteration.
    if (died) {
        *curr = squareOne;
    } else {
        *curr = next;
    }
    
    // Zero means we aren't done yet
    return 0;
}

int run_game(DLinkedList* arena, Physics* state)
{
    // Initialize game loop timers
    int tick, phys_tick, draw_tick;
    Timer timer;
    timer.start();
    tick = timer.read_ms();
    phys_tick = tick;
    draw_tick = tick;

    // Initialize debug counters
    int count = 0;
    int count2 = 0;

    // Initial draw of the game
    uLCD.background_color(backgroundColor);
    uLCD.cls();
    
    squareOne = *state; // square one initilized at start
    
    // initialize save states
    destroyList(saveStates);
    saveStates = create_dlinkedlist();
    // turn off LEDs
    myled1 = 0;
    myled2 = 0;
    myled3 = 0;
    myled4 = 0;
    // reset numofstates
    numOfStates = 0;

    ///////////////////
    // Main game loop
    ///////////////////
    //DRAW STATIC THINGS ONCE TO SPEED UP GAMEPLAY
    ArenaElement* sElem = (ArenaElement*)getHead(arena); // sElem -> static elements
    do {
        switch(sElem->type) {
            case WALL:
                draw_wall((Wall*) sElem);
                break;
            case GOAL:
                draw_goal((Goal*) sElem);
                break;
            case POTHOLE:
                draw_pothole((Pothole*) sElem);
                break;
            case MUD:
                drawMud((Mud*) sElem);
                break;
            case MUSHROOM:
                drawMushroom((Mushroom*) sElem);
                break;
            default:
                break;
        }
    } while(sElem = (ArenaElement*)getNext(arena));
    
    while(1) {
        // Read timer to determine how long the last loop took
        tick = timer.read_ms();
        
        ///////////////////
        // Physics Update
        ///////////////////
        // Rate limit: 1 ms
        int diff = tick - phys_tick;
        if (diff < 1) continue;
        phys_tick = tick;

        // Compute elapsed time in milliseconds
        float delta = diff*1e-3;

        // Read inputs
        GameInputs inputs = read_inputs();

        // Update game state
        int done = update_game(arena, state, inputs, delta, saveStates, timer);
        if (done) {
            //destroyList(saveStates);
            return done;
        }

        // Debug: Count physics updates
        count2++;

        //////////////////
        // Render update
        //////////////////
        // Rate limit: 40ms
        if(tick - draw_tick < 40) continue;
        draw_tick = tick;

        // Erase moving stuff
        ArenaElement* elem = (ArenaElement*)getHead(arena);
        do {
            switch(elem->type) {
                case BALL:
                    erase_ball((Ball*) elem);
                    break;
                case GHOST:
                    eraseGhost((Ghost*) elem);
                default: break;
            }
        } while(elem = (ArenaElement*)getNext(arena));

        // Draw everything
        elem = (ArenaElement*)getHead(arena);
        do {
            switch(elem->type) {
                case WALL:
                    draw_wall((Wall*) elem);
                    break;
                case BALL:
                    draw_ball((Ball*) elem, state);
                    break;
                case GOAL:
                    if (((Goal*) elem)->shouldReDraw) {
                        draw_goal((Goal*) elem);
                        ((Goal*) elem)->shouldReDraw = false;
                    }
                case POTHOLE:
                    if (((Pothole*) elem)->shouldReDraw) {
                        draw_pothole((Pothole*) elem);
                        ((Pothole*) elem)->shouldReDraw = false;
                    }
                    break;
                case GHOST:
                    drawGhost((Ghost*) elem);
                    break;
                case MUD:
                    if (((Mud*) elem)->shouldReDraw) {
                        drawMud((Mud*) elem);
                        ((Mud*) elem)->shouldReDraw = false;
                    }
                    break;
                case MUSHROOM:
                    if ( (((Mushroom*) elem)->shouldReDraw) && !(((Mushroom*) elem)->eaten) ) {
                        drawMushroom((Mushroom*) elem);
                        ((Mushroom*) elem)->shouldReDraw = false;
                    }
                    break;
                default:
                    break;
            }
        } while(elem = (ArenaElement*)getNext(arena));

        ///////////////
        // Debug info
        ///////////////
        // Displays rate info in the top corner
        //  First number is total time to update and render this frame
        //  Second number is how many physics iterations between drawing frames
        //  Only displayed every 10th render update (roughly 2.5 Hz)
        // TODO: Take this out before you turn your code in!
        // REPLACED WITH SCORE CODE
        if ((count = (count+1)%10) == 0) {
            uLCD.locate(0, 0);
            uLCD.textbackground_color(backgroundColor);
            score = timer.read_ms()/100;
            uLCD.printf("%i", score);
            //uLCD.printf("%d %d \r\n", timer.read_ms()-tick, count2);
        }
        
        // Reset physics iteration counter after every render update
        count2 = 0;
        
    }
}


// BALL METHODS ----------------------------------------------------------------
/** ball constructor */
Ball* create_ball(int x, int y) {
    Ball* ball = (Ball*) malloc(sizeof(Ball));
    ball->type = BALL;
    ball->x = x;
    ball->y = y;
    return ball;
}
// POTHOLE METHODS -------------------------------------------------------------
/** constructor **/
Pothole* newPothole(int x, int y, int r) {
    Pothole* pothole = (Pothole*) malloc(sizeof(Pothole));
    pothole->type = POTHOLE;
    pothole->x = x;
    pothole->y = y;
    pothole->r = r;
    pothole->shouldReDraw = false;
    return pothole;
}
/** draws pothole */
void draw_pothole(Pothole* pothole) {
    uLCD.filled_circle(pothole->x,pothole->y,pothole->r, BLACK);    
}
/** returns true if ball falls into pothole **/
bool touching_pothole(Pothole* pothole, Physics* curr, int mode) {
    // mode one returns true if ball should fall into hole
    double dist = sqrt(pow(pothole->x - curr->px, 2) + pow(pothole->y - curr->py, 2));
    if (mode == 0) {
        return dist <= pothole->r;
    } else if (mode == 1) { // mode 1 returns true if you need to redraw pothole
        return dist < (pothole->r + radius + 2);
    } else {
        return false;
    }
}

// GOAL METHODS ----------------------------------------------------------------
/**constructor**/
Goal* newGoal(int x, int y,int r) {
    Goal* goal = (Goal*) malloc(sizeof(Goal));
    goal->type = GOAL;
    goal->x = x;
    goal->y = y;
    goal->r = r;
    goal->shouldReDraw = false;
    return goal;
}
/** draws goal */
void draw_goal(Goal* goal) {
    uLCD.filled_circle(goal->x, goal->y, goal->r, GREEN);     
}
/** returns true when touching goal */
bool touching_goal(Goal* goal, Physics* curr, int mode) {
    double dist = sqrt(pow(goal->x - curr->px, 2) + pow(goal->y - curr->py, 2));
    if (mode == 0) {
        return dist <= goal->r;
    } else if (mode == 1) {
        return dist <= (goal->r + radius);
    } else {
        return false;    
    }
}
/** GHOST METHODS */
/** ghost constructor */
Ghost* newGhost(int x, int y, int d, int s, int v) {
    Ghost* ghost = (Ghost*) malloc(sizeof(Ghost));
    ghost->type = GHOST;
    ghost->x = x; // curr
    ghost->y = y;
    ghost->xp = x; // prev
    ghost->yp = y;
    ghost->ox = x; // anchor
    ghost->oy = y;
    ghost->d = d;
    ghost->s = s;
    ghost->v = v;
    ghost->state = true;
    return ghost;
}
/** draw ghost */
void drawGhost(Ghost* ghost) {
    // update
    ghost->xp = ghost->x; // save previous
    ghost->yp = ghost->y;
    // draw
    uLCD.BLIT(ghost->x,ghost->y,14,14, ghost_sprite);    
}
void eraseGhost(Ghost* ghost) {
    uLCD.filled_rectangle(ghost->xp,ghost->yp,ghost->xp+13,ghost->yp+13,backgroundColor);
}
/** update ghost */
void updateGhost(Ghost* ghost, float delta) {
    if (ghost->d == 0) {
        if (ghost->x >= ghost->ox + ghost->s) {
            ghost->state = false;
        } else if (ghost->x <= ghost->ox - ghost->s) {
            ghost->state = true;
        }
        if (ghost->state) {
            ghost->x += (ghost->v)*delta;
        } else {
            ghost->x -= (ghost->v)*delta;
        }
    } else {
        if (ghost->y >= ghost->oy + ghost->s) {
            ghost->state = false;
        } else if (ghost->y <= ghost->oy - ghost->s) {
            ghost->state = true;
        }
        if (ghost->state) {
            ghost->y += (ghost->v)*delta;
        } else {
            ghost->y -= (ghost->v)*delta;
        }
    }
}
/** returns true when touching ghost */
bool touching_ghost(Ghost* ghost, Physics* curr) {
    int dx = abs(ghost->x+6 - curr->px);
    int dy = abs(ghost->y+6 - curr->py);
    return (dx < 11 && dy < 11);
}
//** MUD METHODS
Mud* newMud(int x, int y, int w, int h) {
    Mud* mud = (Mud*) malloc(sizeof(Mud));
    mud->type = MUD;
    mud->x = x;
    mud->y = y;
    mud->h = h;
    mud->w = w;
    mud->shouldReDraw = false;
    return mud;
}
// draw mud
void drawMud(Mud* mud) {
    uLCD.filled_rectangle(mud->x,mud->y,mud->x+mud->w-1,mud->y+mud->h-1,0xF4A460);
}
// is overlapping with ball ? 
bool touching_mud(Mud* mud, Physics* curr, int mode) {
    if (mode == 0) {
        bool xx = (mud->x <= curr->px) && (curr->px <= mud->x + mud->w - 1);
        bool yy = (mud->y <= curr->py) && (curr->py <= mud->y + mud->h - 1);
        return xx && yy;
    } else {
        bool xx = (mud->x - 4 <= curr->px) && (curr->px <= mud->x + mud->w + 3);
        bool yy = (mud->y - 4 <= curr->py) && (curr->py <= mud->y + mud->h + 3);
        return xx && yy;
    }
}
/** MUSHROOM METHODS */
// constructor
Mushroom* newMushroom(int x, int y) {
    Mushroom* mush = (Mushroom*) malloc(sizeof(Mushroom));
    mush->type = MUSHROOM;
    mush->x = x;
    mush->y = y;
    mush->shouldReDraw = false;
    mush->eaten = false;
    return mush;
}
// draw mushroom
void drawMushroom(Mushroom* mush) {
    uLCD.BLIT(mush->x,mush->y,16,16,mush_sprite);
}
// touching mud method
bool touching_mushroom(Mushroom* mush, Physics* curr, int mode) {
    if (mode == 0) {
        bool xx = (mush->x <= curr->px) && (curr->px <= mush->x + 15);
        bool yy = (mush->y <= curr->py) && (curr->py <= mush->y + 15);
        return xx && yy;
    } else {
        bool xx = (mush->x - 4 <= curr->px) && (curr->px <= mush->x + 19);
        bool yy = (mush->y - 4 <= curr->py) && (curr->py <= mush->y + 19);
        return xx && yy;
    }
}
//erase mush method
void erase_mush(Mushroom* mush) {
    uLCD.filled_rectangle(mush->x,mush->y,mush->x+15,mush->y+15,backgroundColor);    
}




