#pragma once
#include "Entity.h"


#ifdef FULLHD
const INT32 WINDOW_WIDTH = 1920;
const INT32 WINDOW_HEIGHT = 1080;
#else
const INT32 WINDOW_WIDTH = 640;
const INT32 WINDOW_HEIGHT = 480;
#endif



namespace PlanetParam
{
    const INT32   WIDTH = 128;                // image width
    const INT32   HEIGHT = 128;               // image height
    const INT32   COLLISION_RADIUS = 120 / 2;   // for circular collision
    const INT32   X = WINDOW_WIDTH / 2 - WIDTH / 2; // location on screen
    const INT32   Y = WINDOW_HEIGHT / 2 - HEIGHT / 2;
    const FLOAT   MASS = 1.0e14f;         // mass
    const INT32   TEXTURE_COLS = 2;       // texture has 2 columns
    const INT32   START_FRAME = 1;        // starts at frame 1
    const INT32   END_FRAME = 1;          // no animation
}

class Planet : public GameEngine::Entity
{
public:
    Planet();
    virtual ~Planet() {}
    void disable() { visible = false; active = false; }
    void enable() { visible = true; active = true; }
};