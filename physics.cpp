#include "physics.h"
#include "math_extra.h"

void forward_euler(Physics* state, float delta)
{ 
    // TODO: Implement proper forward euler updates for position and velocity
    
    // position: p = p0 + v*t
    state->px = state->px + (state->vx) * delta;
    state->py = state->py + (state->vy) * delta;
    // velocity: v = v0 + a*t
    state->vx = state->vx + (state->ax) * delta;
    state->vy = state->vy + (state->ay) * delta;
    // acceleration determined by accelerometer
    
}