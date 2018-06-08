/* Gatech ECE2035 2017 FALL MAZE RUNNER
* Copyright (c) 2017 Gatech ECE2035
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#define R 0xFF0000 //RED
#define B 0x0000FF //BLUE
#define _ 0x8B4513 //BACKGROUND
#define W 0xFFFFFF //WHITE
#define q 0x000000 //BLACK

// Include header files for platform
#include "mbed.h"

// Include header files for pacman project
#include "globals.h"
#include "math_extra.h"
#include "physics.h"
#include "game.h"
#include "wall.h"
#include "doublely_linked_list.h"
#include "SongPlayer.h"
#include "stdlib.h"

#include "limits.h"
#include "Servo.h"


// Hardware initialization
DigitalIn left_pb(p16);  // push button
DigitalIn right_pb(p17); // push button
DigitalIn up_pb(p18);    // push button
DigitalIn down_pb(p19);  // push button

uLCD_4DGL uLCD(p28,p27,p29); // LCD (serial tx, serial rx, reset pin;)
Serial pc(USBTX,USBRX);     // used by Accelerometer
MMA8452 acc(p9, p10, 100000); // Accelerometer
SDFileSystem sd(p5, p6, p7, p8, "sd"); // SD card and filesystem (mosi, miso, sck, cs)

SongPlayer speaker(p21); // SPEAKER
DigitalOut myled1(LED1); // LED
DigitalOut myled2(LED2); // LED
DigitalOut myled3(LED3); // LED
DigitalOut myled4(LED4); // LED

// EXTERNAL RGB LED
DigitalOut redLed(p11); // RED
DigitalOut greenLed(p12); // GREEN
DigitalOut blueLed(p13); // BLUE

// Level creation method declaration (FUNCTION PROTOYPES)
DLinkedList* create_blank_level(int x, int y);

DLinkedList* level_one_easy();
DLinkedList* level_two_easy();
DLinkedList* level_three_easy();
DLinkedList* level_one_med();
DLinkedList* level_two_med();
DLinkedList* level_three_med();
DLinkedList* level_one_hard();
DLinkedList* level_two_hard();
DLinkedList* level_three_hard();

void next_level(int level);
void end_level(int level);
void title_screen();
void menu_screen(DLinkedList* levels);
void leaderboard();

// Parameters. Declared in globals.h
const float mass = 0.005;
int radius = 4;
const float bounce = 0.4;
const int backgroundColor = 0x8B4513;
int finalScore = 0;
int currentScore = 0;
bool didSkip = false;
int globalDiff = 0;
int squish = 0;
double friction = 1;
bool mudFirstTime = true;
int ball_color = RED;
bool invincible = false;
DLinkedList* saveStates = create_dlinkedlist();


const int par[3][3][2] = {
                           {{ 30, 60}, { 40, 70}, { 200,400}},
                           {{100,150}, { 90,140}, {300,500}},
                           {{100,200}, {100,200}, {500,750}}
                         };

// MUSIC ARRAYS HERE -----------------------------------------------------------
float tetris[] = {659.26, 493.88, 523.25, 587.33, 523.25, 493.88, 440.00, 
                   440.00, 523.25, 659.26, 587.33, 523.25, 493.88, 523.25, 
                   587.33, 659.26, 523.25, 440.00, 440.00, 000.00, 587.33, 
                   698.46, 880.00, 783.99, 698.46, 659.26, 523.25, 659.26, 
                   587.33, 523.25, 493.88, 493.88, 523.25, 587.33, 659.26, 
                   523.25, 440.00, 440.00, 000.00, 0.0};
float durs[] = {0.50, 0.25, 0.25, 0.50, 0.25, 0.25, 0.50, 0.25, 0.25, 0.50, 
                 0.25, 0.25, 0.75, 0.25, 0.50, 0.50, 0.50, 0.50, 0.50, 0.75, 
                 0.50, 0.25, 0.50, 0.25, 0.25, 0.75, 0.25, 0.50, 0.25, 0.25, 
                 0.50, 0.25, 0.25, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.0};
                 
float arpeggio[] = {261.626, 329.628, 391.995, 523.251, 391.995, 329.628, 261.626};
float arpeggioDurs[] = {.25,.25,.25,.25,.25,.25,.25};

int ghost_sprite[14*14] = {
    _,_,_,_,_,R,R,R,R,_,_,_,_,_,
    _,_,_,R,R,R,R,R,R,R,R,_,_,_,
    _,_,R,R,R,R,R,R,R,R,R,R,_,_,
    _,R,W,W,R,R,R,R,W,W,R,R,R,_,
    _,W,W,W,W,R,R,W,W,W,W,R,R,_,
    _,B,B,W,W,R,R,B,B,W,W,R,R,_,
    R,B,B,W,W,R,R,B,B,W,W,R,R,R,
    R,R,W,W,R,R,R,R,W,W,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,R,R,R,R,R,R,R,R,R,R,R,R,
    R,R,_,R,R,R,_,_,R,R,R,_,R,R,
    R,_,_,_,R,R,_,_,R,R,_,_,_,R
};
int mush_sprite[16*16] = {
_,_,_,_,_,q,q,q,q,q,q,_,_,_,_,_,
_,_,_,q,q,q,W,R,R,W,q,q,q,_,_,_,
_,_,q,q,W,W,W,R,R,W,W,W,q,q,_,_,
_,q,q,R,W,W,R,R,R,R,W,W,R,q,q,_,
_,q,W,R,R,R,R,R,R,R,R,R,R,W,q,_,
q,q,W,W,R,R,W,W,W,W,R,R,W,W,q,q,
q,W,W,W,R,W,W,W,W,W,W,R,W,W,W,q,
q,W,W,W,R,W,W,W,W,W,W,R,W,W,W,q,
q,W,W,R,R,W,W,W,W,W,W,R,R,W,W,q,
q,R,R,R,R,R,W,W,W,W,R,R,R,R,R,q,
q,R,R,q,q,q,q,q,q,q,q,q,q,R,R,q,
q,q,q,q,W,W,q,W,W,q,W,W,q,q,q,q,
_,q,q,W,W,W,q,W,W,q,W,W,W,q,q,_,
_,_,q,W,W,W,W,W,W,W,W,W,W,q,_,_,
_,_,q,q,W,W,W,W,W,W,W,W,q,q,_,_,
_,_,_,q,q,q,q,q,q,q,q,q,q,_,_,_
};
                 
// -----------------------------------------------------------------------------

/** Main() is where you start your implementation */
int main()
{
    //uLCD.printf("%i",acc.activate()); debug
    
    redLed = 1;
    blueLed = 1;
    greenLed = 1;
    ////////////////////////////
    // Power-on initialization
    ////////////////////////////
    // Turn up the serial data rate so we don't lag
    uLCD.baudrate(3000000);
    pc.baud(115200);

    // Initialize the buttons
    // Each *_pb variable is 0 when pressed
    left_pb.mode(PullUp);
    right_pb.mode(PullUp);
    up_pb.mode(PullUp);
    down_pb.mode(PullUp);

    // Other hardware initialization here (SD card, speaker, etc.)

    ///////////////
    // Reset loop
    ///////////////
    // This is where control between major phases of the game happens
    // This is a good place to add choose levels, add the game menu, etc.

    // Title Screen
    title_screen();
    DLinkedList* levels = create_dlinkedlist();
    // MENU SCREEN
    menu_screen(levels);

    int levelCount = 1; // setup for in between levels screen
    while(1) 
    {
        // level screen
        next_level(levelCount);
        
        // Initialze game state
        DLinkedList* arena = (DLinkedList*) getCurrent(levels);
        Physics state = {0};
        state.px = ((Ball*) getTail(arena))->x;
        state.py = ((Ball*) getTail(arena))->y;
        //state.px = 128.0 - 16.0;        // Position in the center of the screen
        //state.py = 16.0;
        state.vx = 0.0;         // Initially unmoving
        state.vy = 0.0;

        // Delegate to the game loop to execute the level
        // run_game() is in game.cpp
        run_game(arena, &state);

        // Destory the arena and entities once we're done with the level
        destroyList(arena);
        end_level(levelCount);
        if (getNext(levels) == NULL) break;
        levelCount++;
        ball_color = RED;
        invincible = false;
        radius = 4;
    }
    Servo myServo(p24);
    myServo.write(180);
    leaderboard();
}

/** Creates a level with only bounding walls and a ball. */
DLinkedList* create_blank_level(int x, int y)
{
    DLinkedList* arena = create_dlinkedlist();

    // Initialize the walls
    Wall* walls[4];
    walls[0] = create_wall(HORIZONTAL, 0, 0, 127, bounce);  // top
    walls[1] = create_wall(HORIZONTAL, 0, 127, 127, bounce);// bottom
    walls[2] = create_wall(VERTICAL, 0, 0, 127, bounce);    // left
    walls[3] = create_wall(VERTICAL, 127, 0, 127, bounce);  // right
    
    // Add the walls to the arena
    for (int i = 0; i < 4; i++)
        insertTail(arena, (void*)walls[i]);

    // Initialize the ball
    Ball* ball = (Ball*) malloc(sizeof(Ball));
    ball->type = BALL;
    ball->x = x;
    ball->y = y;
    
    // Add ball to the arena 
    // NOTE: The ball should always be last in the arena list, so that the other 
    // ArenaElements have a chance to compute the Physics updates before the 
    // ball applies forward euler method.
    insertTail(arena, (void*)ball);
    
    return arena;
}

/** level intro screen */
void next_level(int level) {
    uLCD.textbackground_color(BLACK);
    uLCD.background_color(BLACK);
    uLCD.printf("\n\n\n\n\n\n\n");
    uLCD.printf("      Level %i", level);
    wait(1.5);
    uLCD.cls(); // clear screeen
}
//
void end_level(int level) {
    // music chime
    speaker.turnOn();
    speaker.PlaySong(7,false,arpeggio,arpeggioDurs,.04);
    
    int s = 1; // speed (bigger is faster)
    // RGB LED "grade" -> green is great, blue is ok, red is slow
    if (currentScore <= par[globalDiff][level][0]) {
        greenLed = 0;
    } else if (par[globalDiff][level][0] < currentScore 
            && currentScore < par[globalDiff][level][1]) {
        blueLed = 0;
    } else {
        redLed = 0;   
    }
    // BLACK BARS ANIMATION
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 128-s; i+=s) {
            uLCD.filled_rectangle(i,j*32,i+s,(j+1)*32, BLACK);
        }
    }
    uLCD.textbackground_color(BLACK);
    uLCD.background_color(BLACK);
    uLCD.printf("\n\n\n\n\n\n\n");
    uLCD.printf("    Your Score\n");
    uLCD.printf("       %i", currentScore);
    wait(1.5);
    uLCD.cls();
    redLed = 1; // turn off all LEDS
    blueLed = 1;
    greenLed = 1;
}
// beginning title sequence
void title_screen() {
    uLCD.printf("\n\n\n\n\n");
    uLCD.printf("      Chris's\n");
    uLCD.printf("     Labyrinth\n\n\n");
    uLCD.printf(" PRESS ANY BUTTON\n");
    uLCD.printf("     TO START");
    speaker.turnOn();
    speaker.PlaySong(40,true,tetris,durs,.03);
    while (left_pb && right_pb && up_pb && down_pb); // hold until button is pressed
    speaker.turnOff();
    uLCD.cls();
}
// menu select
void menu_screen(DLinkedList* levels) {
    // menu screen options
    uLCD.printf("\n\n");
    uLCD.printf("      Select\n");
    uLCD.printf("    Difficulty\n\n\n");
    uLCD.printf("      Easy\n\n\n");
    uLCD.printf("      Medium\n\n\n");
    uLCD.printf("      Hard");

    int difficulty = 0; // 0 easy, 1 medium, 2 hard

    // SELECTOR
    int x = 25; // selector x pos remains constant
    int y[] = {52,75,99}; // y pos of selector changes
    int r = 4;
    uLCD.filled_circle(x,y[0],r,GREEN); // THIS IS THE SELECTOR (default green)
    // SOUND
    float sound[] = {1200};
    float dur[] = {.15};
    // stay until on left button press
    while (left_pb) {
        if (difficulty == 0) {
            if (down_pb == false) { // down button pressed
                difficulty = 1;
                uLCD.filled_circle(x,y[0],r,BLACK); // erase current selector
                uLCD.filled_circle(x,y[1],r,GREEN); // draw new selector
                speaker.PlaySong(1,false,sound,dur,.05);
                wait(.25);
            }
        } else if (difficulty == 1) {
            if (up_pb == false) { // up button pressed
                difficulty = 0;
                uLCD.filled_circle(x,y[1],r,BLACK); // erase current selector
                uLCD.filled_circle(x,y[0],r,GREEN); // draw new selector
                speaker.PlaySong(1,false,sound,dur,.05);
                wait(.25);
            } else if (down_pb == false) { // down button pressed
                difficulty = 2;
                uLCD.filled_circle(x,y[1],r,BLACK); // erase current selector
                uLCD.filled_circle(x,y[2],r,GREEN); // draw new selector
                speaker.PlaySong(1,false,sound,dur,.05);
                wait(.25);
            }
        } else if (difficulty == 2) {
            if (up_pb == false) { // down button pressed
                difficulty = 1;
                uLCD.filled_circle(x,y[2],r,BLACK); // erase current selector
                uLCD.filled_circle(x,y[1],r,GREEN); // draw new selector
                speaker.PlaySong(1,false,sound,dur,.05);
                wait(.25);
            }
        }
    }
    if (difficulty == 2) {
        insertTail(levels, level_one_hard());
        insertTail(levels, level_two_hard());
        insertTail(levels, level_three_hard());
        globalDiff = 2;
    } else if (difficulty == 1) {
        insertTail(levels, level_one_med());
        insertTail(levels, level_two_med());
        insertTail(levels, level_three_med());
        globalDiff = 1;
    } else { // default to easy
        insertTail(levels, level_one_easy());
        insertTail(levels, level_two_easy());
        insertTail(levels, level_three_easy());
        globalDiff = 0;
    }
    getHead(levels);
    uLCD.cls();
}
// leaderboard screen
void leaderboard() {
    uLCD.background_color(BLACK);
    uLCD.textbackground_color(BLACK);

    if (!didSkip) {
        uLCD.printf("\n\n\n\n\n");
        uLCD.printf("    Your Score\n");
        uLCD.printf("       %i\n\n", finalScore);
    } else {
        uLCD.printf("\n\n");
        uLCD.printf("    Your Score\n");
        uLCD.printf("       %i\n\n", finalScore);
        uLCD.printf(" Don't skip any\n");
        uLCD.printf(" levels next time\n");
        uLCD.printf(" to get onto the\n");
        uLCD.printf("   leaderboard.\n\n");
    }
    uLCD.printf(" PRESS ANY BUTTON\n");
    uLCD.printf("    TO CONTINUE");
    while (left_pb && right_pb && up_pb && down_pb);
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\n    Leaderboard\n\n\n");
    FILE *fp;
    if (globalDiff == 2) {
        fp = fopen("/sd/ECE_2035/hard_scores.txt", "r");
    } else if (globalDiff == 1) {
        fp = fopen("/sd/ECE_2035/medium_scores.txt", "r");
    } else {
        fp = fopen("/sd/ECE_2035/easy_scores.txt", "r");
    }
    if (fp == NULL){
        uLCD.printf("MicroSD card error");
        fclose(fp);
        return;
    }
    char buffer[10];
    int leaderboard[5];
    int shift = -1;
    bool replace = true;
    // read leaderboard
    int ii = 0;
    while(!feof(fp)) {
        fgets(buffer, 10, fp);
        leaderboard[ii] = atoi(buffer);
        if (leaderboard[ii] == 0) leaderboard[ii] = INT_MAX;
        if (finalScore < leaderboard[ii] && replace) {
            shift = ii;
            replace = false;
        }
        ii++;
    }
    fclose(fp);
    
    if (!didSkip) {
        // splice in new score
        if (shift != -1) {
            for (int i = 4; i > shift; i--) {
                leaderboard[i] = leaderboard[i-1];
            }
            leaderboard[shift] = finalScore;  
        }
    }
    // write to leaderboard
    if (globalDiff == 2) {
        fp = fopen("/sd/ECE_2035/hard_scores.txt", "w");
    } else if (globalDiff == 1) {
        fp = fopen("/sd/ECE_2035/medium_scores.txt", "w");
    } else {
        fp = fopen("/sd/ECE_2035/easy_scores.txt", "w");
    }
    for (int i = 0; i < 5; i++) {
        if (leaderboard[i] == INT_MAX) {
            uLCD.printf("  %i. -----\n\n", i+1);
            fprintf(fp,"0\n");
        } else {
            uLCD.printf("  %i. %i\n\n", i+1, leaderboard[i]);
            fprintf(fp,"%i\n",leaderboard[i]);
        }
    }
    fclose(fp);
}

// LEVEL FUNCTIONS ------------------------------------------------
DLinkedList* level_one_easy() {
    DLinkedList* arena = create_blank_level(96, 96);
    getTail(arena);
    // MAIN ELEMENTS HERE
    insertBefore(arena, create_wall(VERTICAL,64, 32, 96, bounce));
    insertBefore(arena, newPothole(32, 32, 5));
    insertBefore(arena, newMud(80,32,20,20));
    insertBefore(arena, newGoal(32,96,6));
    return arena;
}
DLinkedList* level_two_easy() {
    DLinkedList* arena = create_blank_level(63, 63);
    getTail(arena);
    // MAIN ELEMENTS HERE
    insertBefore(arena, create_wall(VERTICAL,49, 32, 96, bounce));
    insertBefore(arena, create_wall(VERTICAL,79, 0, 96, bounce));

    insertBefore(arena, newPothole(12, 115, 6));
    insertBefore(arena, newGoal(115,12,6));
    return arena;
}
DLinkedList* level_three_easy() {
    DLinkedList* arena = create_blank_level(10, 63);
    getTail(arena);
    // MAIN ELEMENTS HERE
    insertBefore(arena, create_wall(VERTICAL,32, 0, 96, bounce));
    insertBefore(arena, create_wall(HORIZONTAL,64, 32, 64, bounce));
    insertBefore(arena, create_wall(HORIZONTAL,32, 64, 64, bounce));
    insertBefore(arena, create_wall(HORIZONTAL,64, 96, 64, bounce));

    insertBefore(arena, newPothole(42, 10, 9));
    insertBefore(arena, newPothole(40, 72, 6));
    insertBefore(arena, newPothole(112, 112, 14));

    insertBefore(arena, newGoal(118,12,8));
    return arena;
}


DLinkedList* level_one_med() {
    DLinkedList* arena = create_blank_level(112,16);
    getTail(arena);
    insertBefore(arena, create_wall(VERTICAL, 32, 0, 96, bounce));
    insertBefore(arena, create_wall(VERTICAL, 64, 32, 96, bounce));
    insertBefore(arena, create_wall(VERTICAL, 96, 0, 96, bounce));
    int xy[10] = {121,121,70,121,58,121,6,121,64,6};
    for (int i = 0; i < 10; i+=2) {
        insertBefore(arena, newPothole(xy[i], xy[i+1], 5));
    }
    Goal* goal = newGoal(16,16,6);
    insertBefore(arena, goal);
    return arena; 
}
DLinkedList* level_three_med() {
    DLinkedList* arena = create_blank_level(10, 63);
    getTail(arena);
    // MAIN ELEMENTS HERE
    int s = 25; // scale
    int horz[] = {0,s,3*s, 0,3*s,s,  s,2*s,2*s, s,4*s,3*s};
    int vert[] = {2*s,2*s,2*s, 4*s,0,2*s};
    for (int i = 0; i < 6; i+=3) {
        insertBefore(arena, create_wall(VERTICAL,vert[i],vert[i+1],vert[i+2], bounce));
    }
    for (int i = 0; i < 12; i+=3) {
        insertBefore(arena, create_wall(HORIZONTAL,horz[i],horz[i+1],horz[i+2],bounce));
    }
    insertBefore(arena, newPothole(2.5*s, 2.5*s, 7));
    insertBefore(arena, newPothole(9, s+7, 6));
    insertBefore(arena, newPothole(118, 118, 6));
    insertBefore(arena, newPothole(8, 120, 5));
    
    insertBefore(arena, newGhost(4*s,3*s,1,s,60));
    insertBefore(arena, newGoal(116,12,6));
    return arena;
}
DLinkedList* level_two_med() {
    DLinkedList* arena = create_blank_level(15, 15);
    getTail(arena);
    // MAIN ELEMENTS HERE
    insertBefore(arena, (void*) create_wall(VERTICAL, 32, 0, 96, bounce));
    insertBefore(arena, newGhost(75,63,0,20,50));
    
    insertBefore(arena, (void*) newPothole(22,32,5));
    insertBefore(arena, (void*) newPothole(22,64,5));
    insertBefore(arena, (void*) newPothole(64,10,7));
    insertBefore(arena, (void*) newPothole(118,118,6));
    
    insertBefore(arena, newGoal(110,15,6));
    return arena;
}

DLinkedList* level_one_hard() {
    DLinkedList* arena = create_blank_level(10, 63);
    getTail(arena);
    //CORNERS
    int s = 16; // spacing var
    int horz[] = {
        0,s,s, 0,128-s,s, 128-s,s,s, 128-s,128-s,s, 50,96,20, 96,48,16, 70,64,24,
        96,96,32
    };
    int vert[] = {
        s,0,s, s,128-s,s, 128-s,0,3*s, 128-s,128-s,s, 32,32,64, 50,0,52, 50,76,52,
        70,32,64
    };
    int holes[] = {
        8,23, 8,105, 120,23, 120,105, 40,87, 59,87, 59,8, 74,120, 86,120, 98,120
    };
    for (int i = 0; i < sizeof(horz)/sizeof(int); i+=3) {
        insertBefore(arena, create_wall(HORIZONTAL, horz[i], horz[i+1], horz[i+2], bounce));
    }
    for (int i = 0; i < sizeof(vert)/sizeof(int); i+=3) {
        insertBefore(arena, create_wall(VERTICAL, vert[i], vert[i+1], vert[i+2], bounce));
    }
    for (int i = 0; i < sizeof(holes)/sizeof(int); i+=2) {
        insertBefore(arena, newPothole(holes[i],holes[i+1],5));
    }
    insertBefore(arena, newGoal(58,120,6));
    return arena;
}
DLinkedList* level_two_hard() {
    DLinkedList* arena = create_blank_level(10, 10);
    getTail(arena);
    // MAIN ELEMENTS HERE
    int s = 21; // scaling 
    int horz[] = {s,5*s,4*s, 2*s,s,3*s, 2*s,4*s,2*s, 3*s,2*s,s};
    int vert[] = {s,0,5*s, 5*s,s,4*s, 2*s,s,3*s, 4*s,2*s,2*s, 3*s,2*s,s};
    int pots[] = {s/4+1,2*s,s/4+1,4*s,3*s/4,3*s, 2*s,4.75*s,  3*s,4.25*s+1, 4*s,4.75*s, 2*s,5.75*s-1,
                    3*s,5.25*s+1,4*s,5.75*s-1,4.75*s-1,1.25*s+1,.25*s+1,5.75*s-1,5.75*s-1,5.75*s-1};
    for (int i = 0; i < 15; i+=3) {
        insertBefore(arena, create_wall(VERTICAL,vert[i],vert[i+1],vert[i+2], bounce));
    }
    for (int i = 0; i < 12; i+=3) {
        insertBefore(arena, create_wall(HORIZONTAL,horz[i],horz[i+1],horz[i+2],bounce));
    }
    for (int i = 0; i < sizeof(pots)/sizeof(int); i+=2) {
        insertBefore(arena, newPothole(pots[i],pots[i+1],5));
    }
    insertBefore(arena, newGoal(3.5*s,2.5*s,6));
    return arena;
}
DLinkedList* level_three_hard() {
    DLinkedList* arena = create_blank_level(10, 10);
    getTail(arena);
    // MAIN ELEMENTS HERE
    insertBefore(arena, create_wall(HORIZONTAL,0,30,36,bounce));
    insertBefore(arena, create_wall(VERTICAL,30,64,36,bounce));
    insertBefore(arena, newMushroom(60,60));
    insertBefore(arena, newGhost(70,24,0,30,60));
    insertBefore(arena, newGhost(100,64,1,20,100));
    insertBefore(arena, newMud(80,80,20,20));
    insertBefore(arena, newPothole(40,50,6));
    insertBefore(arena, newPothole(24,64,8));
    insertBefore(arena, newPothole(120,15,5));
    insertBefore(arena, newPothole(68,90,5));
    insertBefore(arena, newPothole(68,90,6));
    insertBefore(arena, newPothole(90,110,6));
    insertBefore(arena, newGoal(110,110,7));
    return arena;
}
