#include "Bar.h"


BOOL Bar::initialize(GameEngine::DirectGraphic* graphics, GameEngine::TextureManager* textureM, INT32 left, INT32 top,
    FLOAT scale, COLOR_ARGB color)
{
	try
	{
		Image::initialize(graphics, BarParam::IMAGE_SIZE, BarParam::IMAGE_SIZE, BarParam::TEXTURE_COLS, textureM);
			
		setCurrentFrame(BarParam::BAR_FRAME);
		spriteData.x = static_cast<FLOAT>(left);
		spriteData.y = static_cast<FLOAT>(top);
		spriteData.scale = scale;
		colorFilter = color;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}



VOID Bar::set(FLOAT percentOn)
{
	if (percentOn < 0)
		percentOn = 0;
	else if (percentOn > 100)
		percentOn = 100;
	spriteData.objrect.right = spriteData.objrect.left + static_cast<LONG>(spriteData.width * percentOn / 100);
}