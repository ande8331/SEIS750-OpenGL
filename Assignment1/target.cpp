#include <stdlib.h>
#include "target.h"

target::target ()
{
    x = rand() % 400;
    y = rand() % 400;
    height = 40;
    width = 40;
    xRate = ((rand() %9) +1) * .2;
    yRate = ((rand() %9) +1) * .2;
    hitCount = 0;
    hitTolerance = 1;
}

float target::getX(void)
{
    return x;
}

float target::getY(void)
{
    return y;
}

float target::getHeight(void)
{
    return height;
}

float target::getLength(void)
{
    return width;
}

/* Move position, reverse direction if at edge of screen */
void target::updatePosition(float maxX, float maxY)
{
    x += xRate;
    y += yRate;

    if (x <= 0)
    {
        x = 0;
        xRate *= -1.0;
    }
    
    if (x+width >= maxX)
    {
        x = maxX-width;
        xRate *= -1.0;
    }

    if (y <= 0)
    {
        y = 0;
        yRate *= -1.0;
    }
    
    if (y+height >= maxY)
    {
        y = maxY-height;
        yRate *= -1.0;
    }
}

/* 
   Take the coordinates of a shot and determine
   if they hit this target.
*/
bool target::takingFire(float xIn, float yIn)
{
    if (targetAtPosition(xIn, yIn))
    {
        hitCount++;
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Determine if target is at specified coordinates.
*/
bool target::targetAtPosition(float xIn, float yIn)
{
    if ((xIn >= x) && (xIn <= x+width) && (yIn >= y) && (yIn <= y+height))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Determine if this target should be displayed.
*/
bool target::targetDisplayable(void)
{
    if (hitCount < hitTolerance)
    {
        return true;
    }
    return false;
}