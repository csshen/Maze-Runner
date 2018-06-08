#pragma once

/**
 * The main state structure for the game.
 * This structure holds all the global state for the game, and is updated during
 * the game loop. When you implement the "jump back" feature, this is the 
 * structure you need to save/restore.
 */
struct Physics {    
    float px, py; // Ball position
    float vx, vy; // Ball velocity
    float ax, ay; // Ball acceleration
};

/**
 * Performs one step of Forward Euler integration, given a time step delta.
 *
 * @param[out] out   The structure to store the result of the integration.
 * @param[in]  in    The current state of the system.
 * @param[in]  delta The time step, in seconds.s
 */
void forward_euler(Physics* state, float delta);