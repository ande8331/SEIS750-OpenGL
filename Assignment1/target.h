#include <stdlib.h>

class target
{
    float x;
    float y;
    float height;
    float width;
    float xRate;
    float yRate;
    int hitCount;
    int hitTolerance;

public:
    target::target (void);
    float getX(void);
    float getY(void);
    float getLength(void);
    float getHeight(void);
    bool takingFire(float xIn, float yIn);
    bool targetAtPosition(float xIn, float yIn);
    bool targetDisplayable();
    void updatePosition(float maxX, float maxY);
};
