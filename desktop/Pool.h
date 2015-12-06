#ifndef H_POOL
#define H_POOL

#include "objectTracking/Vec2.h"

class Pool
{
public:
    void setWhiteBall(Vec2 whiteBall);
    void setTargetBall(Vec2 targetBall);
    void setHole(Vec2 hole);

    float getHitAngle();
private:
    Vec2 whiteBall;
    Vec2 targetBall;
    Vec2 hole;
};
#endif
