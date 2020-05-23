#pragma once

#include "Image.h"
#include "constants.h"
#include "TextureManager.h"

namespace BarParam
{
	const INT32 IMAGE_SIZE = 32;
	const INT32 TEXTURE_COLS = 8;
	const INT32 BAR_FRAME = 44;
}

class Bar : public GameEngine::Image
{
public:
    
    BOOL initialize(GameEngine::DirectGraphic* graphics, GameEngine::TextureManager* textureM, INT32 left, INT32 top,
                    FLOAT scale, COLOR_ARGB color);
    // Set Bar Size
    VOID set(FLOAT percentOn);
    // Override update so setRect() is not called.
    virtual VOID update(FLOAT frameTime) {}
};