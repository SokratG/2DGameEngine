#include "Torpedo.h"
#include "SpaceWar.h"

Torpedo::Torpedo(INT32 GAME_WIDTH, INT32 GAME_HEIGHT) : Entity()
{
    active = FALSE;                                 // torpedo starts inactive
    this->GAME_HEIGHT = GAME_HEIGHT;
    this->GAME_WIDTH = GAME_WIDTH;
    spriteData.width = TorpedoParam::WIDTH;     // size of 1 image
    spriteData.height = TorpedoParam::HEIGHT;
    spriteData.objrect.bottom = TorpedoParam::HEIGHT;    // rectangle to select parts of an image
    spriteData.objrect.right = TorpedoParam::WIDTH;
    cols = TorpedoParam::TEXTURE_COLS;
    frameDelay = TorpedoParam::ANIMATION_DELAY;
    startFrame = TorpedoParam::START_FRAME;       // first frame of ship animation
    endFrame = TorpedoParam::END_FRAME;         // last frame of ship animation
    currentFrame = startFrame;
    physObj.radius = TorpedoParam::COLLISION_RADIUS;  // for circular collision
    visible = FALSE;
    fireTimer = 0.0f;
    physObj.mass = TorpedoParam::MASS;
    collisionType = GameEngine::COLLISION_TYPE::CIRCLE;
}

VOID Torpedo::update(FLOAT frameTime)
{
    fireTimer -= frameTime;                     // time remaining until fire enabled

    if (visible == FALSE)
        return;

    if (fireTimer < 0)                           // if ready to fire
    {
        visible = FALSE;                        // old torpedo off
        active = FALSE;
    }

    Image::update(frameTime);

    spriteData.x += frameTime * physObj.velocity.x;     // move along X 
    spriteData.y += frameTime * physObj.velocity.y;     // move along Y

    // Wrap around screen edge
    if (spriteData.x > GAME_WIDTH)              // if off right screen edge
        spriteData.x = -TorpedoParam::WIDTH;       // position off left screen edge
    else if (spriteData.x < -TorpedoParam::WIDTH)  // else if off left screen edge
        spriteData.x = GAME_WIDTH;              // position off right screen edge
    if (spriteData.y > GAME_HEIGHT)             // if off bottom screen edge
        spriteData.y = -TorpedoParam::HEIGHT;      // position off top screen edge
    else if (spriteData.y < -TorpedoParam::HEIGHT) // else if off top screen edge
        spriteData.y = GAME_HEIGHT;             // position off bottom screen edge
}

void Torpedo::fire(Entity& ship)
{
    if (fireTimer <= 0.0f)                       // if ready to fire
    {
        physObj.velocity.x = static_cast<FLOAT>(cos(ship.getRadiansAngle()) * TorpedoParam::SPEED);
        physObj.velocity.y = static_cast<FLOAT>(sin(ship.getRadiansAngle()) * TorpedoParam::SPEED);
        spriteData.x = ship.getCenterX() - spriteData.width / 2;
        spriteData.y = ship.getCenterY() - spriteData.height / 2;
        visible = TRUE;                         // make torpedo visible
        active = TRUE;                          // enable collisions
        fireTimer = TorpedoParam::FIRE_DELAY;      // delay firing
        audio->play(TORPEDO_FIRE);
    }
}
