#pragma once

// Forward declarations
struct Physics;
struct Entity;

/**
 * Structure to represent a Wall in the arena.
 * 
 * This structure is a ArenaElement; its first data element is an integer (type)
 * whose value is always WALL.
 */
struct Wall {
    int type;
    int direction;
    int x, y;
    int length;
    float bounce;
    int should_draw;    
};

// Directions
#define HORIZONTAL  0
#define VERTICAL    1

/**
 * Allocate and initialize a new wall object.
 *
 * @param[in] direction The direction of the wall (HORIZONTAL or VERTICAL)
 * @param[in] x         The x location of the origin of the wall
 * @param[in] y         The y location of the origin of the wall
 * @param[in] length    The length of the wall. (HORIZONTAL = right, VERTICAL = down)
 * @param[in] bounce    The coefficient of bounciness for the wall. Between 0 and 1.
 * @return              The newly initialized wall.
 */
Wall* create_wall(int direction, int x, int y, int length, float bounce);

/**
 * Computes the physics update for a wall.
 *
 * @param[out] phys The result physics update. Assumed valid at function start.
 * @param[in]  ball The ball to update
 * @param[in]  wall The wall to bounce off of
 * @param[in]  delta The elapsed time, in seconds, since the previous update
 */
void do_wall(Physics* next, const Physics* curr, Wall* wall, float delta);

/**
 * Draws a given wall to the screen.
 * 
 * @param[in] wall  The wall to draw
 */
void draw_wall(Wall* wall);
