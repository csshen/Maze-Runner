#include "wall.h"

#include "globals.h"
#include "physics.h"
#include "game.h"
#include "math_extra.h"
#include "math.h"


Wall* create_wall(int direction, int x, int y, int length, float bounce)
{
    // Alocate memory for wall
    Wall* wall = (Wall*) malloc(sizeof(Wall));
    
    // Initialize type info.
    // Always draw on the first frame
    wall->type = WALL;
    wall->should_draw = 1;
    
    // Set user parameters
    wall->direction = direction;
    wall->x = x;
    wall->y = y;
    wall->length = length;
    wall->bounce = bounce;
    
    return wall;
}

void do_wall(Physics* next, const Physics* curr, Wall* wall, float delta)
{
    // Debug info
//    pc.printf("wall: %d %d %d %d\r\n", wall->direction, wall->x, wall->y, wall->length);
    
    float adj, percent, contact;

    if (wall->direction == VERTICAL)
    {
        // Step 0: Adjust the wall location based on ball radius
        adj = wall->x - sgn(curr->vx)*(radius+1);

        // Step 1: Compute percent of motion before bounce
        percent = (adj - curr->px) / (curr->vx*delta);
        
        // Negative percent means we're moving away from the wall
        // >=100% means we don't need to bounce yet
        if (percent >= 0.0 && percent < 1.0)
        {
            // Compute the bounce location
            contact = curr->py + curr->vy*percent*delta;
            if (in_range(contact, wall->y, wall->y + wall->length))
            {
                
                // Reverse velocity and slow down
                next->vx = -wall->bounce*curr->vx;

                // Adjust X position.
                // This dampens the integration based on how far the ball had 
                // to travel before reflecting.
                next->px = adj - percent*next->vx*delta;
                //
                // play sound on contact if fast enough
                if (abs(curr->vx) > 20) {
                    float f[] = {100};
                    float d[] = {0.1};
                    speaker.PlaySong(1, false,f, d, .03);
                    squish = 2;
                }
            }
            // Draw the wall if we're close by
            wall->should_draw = 1;
        }
        
        // Make sure we aren't inside the wall
        if (in_range((int)curr->px, wall->x - (radius+1), wall->x + (radius+1))
             && in_range((int)curr->py, wall->y, wall->y + wall->length))
        {
            next->px = wall->x + sgn(curr->px - wall->x)*(radius+1);
            wall->should_draw = 1;
        }
    }
    else //direction == HORIZONTAL
    {
        // Same as vertical case, but swap x/y
        adj = wall->y - sgn(curr->vy)*(radius+1); 
        percent = (adj - curr->py) / (curr->vy*delta);
        if (percent >= 0.0 && percent < 1.0)
        {
            contact = curr->px + curr->vx*percent*delta;
            if (in_range(contact, wall->x, wall->x + wall->length))
            {
                next->vy = -wall->bounce*curr->vy;
                next->py = adj - percent*next->vy*delta;
                //
                // play sound on contact if fast enough
                if (abs(curr->vy) > 20) {
                    float f[] = {100};
                    float d[] = {0.1};
                    speaker.PlaySong(1,false,f,d,.03);
                    squish = 1;
                }
            }
            // Draw the wall if we bounce
            wall->should_draw = 1;
            
        }
        if (in_range((int)curr->py, wall->y - (radius+1), wall->y + (radius+1))
         && in_range((int)curr->px, wall->x, wall->x + wall->length))
        {
            next->py = wall->y + sgn(curr->py - wall->y)*(radius+1);
            wall->should_draw = 1;
        }
    }
}

void draw_wall(Wall* wall)
{
    if (wall->should_draw)
    {
        int wx1, wx2, wy1, wy2;
        wx1 = wall->x;
        wy1 = wall->y;
        wx2 = wx1 + ((wall->direction == HORIZONTAL) ? wall->length : 0);
        wy2 = wy1 + ((wall->direction == VERTICAL) ? wall->length : 0);
        uLCD.line(wx1, wy1, wx2, wy2, WHITE);
        
        // Don't draw again unless requested
        wall->should_draw = 0;
    }
}    