#pragma once
#include "Entity.h"



namespace TorpedoParam
{
    const INT32     WIDTH = 32;             // image width
    const INT32     HEIGHT = 32;           // image height
    const INT32     COLLISION_RADIUS = 4;   // for circular collision
    const FLOAT     SPEED = 200.f;            // pixels per second
    const FLOAT     MASS = 300.0f;          // mass
    const FLOAT     FIRE_DELAY = 4.0f;      // 4 seconds between torpedo firing
    const INT32     TEXTURE_COLS = 8;       // texture has 8 columns
    const INT32     START_FRAME = 40;       // starts at frame 40
    const INT32     END_FRAME = 43;         // animation frames 40,41,42,43
    const FLOAT     ANIMATION_DELAY = 0.1f; // time between frames
}


class Torpedo : public GameEngine::Entity
{
private:
    FLOAT fireTimer;
    INT32 GAME_WIDTH;
    INT32 GAME_HEIGHT;
public:
    Torpedo(INT32 GAME_WIDTH, INT32 GAME_HEIGHT);
    virtual ~Torpedo() {}
    VOID update(FLOAT frameTime);
    FLOAT getMass() const { return TorpedoParam::MASS; }

    void fire(Entity& ship);

};