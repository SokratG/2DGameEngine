#include "Image.h"





namespace GameEngine {
	// constr
	Image::Image() : initialized(FALSE), cols(1), textureManager(nullptr), 
					startFrame(0), endFrame(0), currentFrame(0), frameDelay(1.f),
					animationTimer(0.f), visible(TRUE), loop(TRUE), animationCompleted(FALSE),
					graphicDevice(nullptr), colorFilter(GraphColor::WHITE)
	{
		// setup sprite data by default
		spriteData.width = 2;	
		spriteData.height = 2;
		spriteData.x = 0.f;
		spriteData.y = 0.f;
		spriteData.scale = 1.f;
		spriteData.angle = 0.f;
		spriteData.objrect.left = 0;
		spriteData.objrect.top = 0;
		spriteData.objrect.right = spriteData.width;
		spriteData.objrect.bottom = spriteData.height;
		spriteData.texture = nullptr;
		spriteData.flipHorizontal = FALSE;
		spriteData.flipVertical = FALSE;
		

		result = E_FAIL;
	}

	// destr
	Image::~Image() {

	}


	BOOL Image::initialize(DirectGraphic* pGDirect, UINT width, UINT height, UINT ncol, TextureManager* pTextureManager)
	{
		try
		{
			graphicDevice = pGDirect;
			textureManager = pTextureManager;
			spriteData.texture = pTextureManager->getTexture();
			if (width == 0)
				width = pTextureManager->getWidth();
			spriteData.width = width;
			if (height == 0)
				height = pTextureManager->getHeight();
			spriteData.height = height;
			cols = (ncol == 0) ? 1 : ncol;
			// filled objrect - rect in sprite data for the draw current frame
			spriteData.objrect.left = (currentFrame % cols) * spriteData.width; 
			spriteData.objrect.top = (currentFrame / cols) * spriteData.height;
			// Right Edge + 1 and Bottom edge + 1
			spriteData.objrect.right = spriteData.objrect.left + spriteData.width;
			spriteData.objrect.bottom = spriteData.objrect.top + spriteData.height;
		}
		catch (...)
		{
			return FALSE;
		}
		initialized = TRUE;
		return TRUE;
	}

	VOID Image::setCurrentFrames(UINT currentframe) {

		if (currentframe <= endFrame) {
			currentFrame = currentframe;
			animationCompleted = FALSE;
			setRect();
		}
		

	}

	inline VOID Image::setRect() {
		// configure objrect - rect in sprite data for the draw current frame
		spriteData.objrect.left = (currentFrame % cols) * spriteData.width;
		spriteData.objrect.top = (currentFrame / cols) * spriteData.height;
		// Right Edge + 1 and Bottom edge + 1
		spriteData.objrect.right = spriteData.objrect.left + spriteData.width;
		spriteData.objrect.bottom = spriteData.objrect.top + spriteData.height;
	}


	VOID Image::draw(COLOR_ARGB color) {
		if (!visible || !graphicDevice)
			return;

		// get fresh texture incase on Reset() was called
		spriteData.texture = textureManager->getTexture();
		if (color == GraphColor::FILTER)
			graphicDevice->drawSprite(spriteData, colorFilter); // draw with filter color
		else
			graphicDevice->drawSprite(spriteData, color);
	}

	VOID Image::draw(SpriteData spritedata, COLOR_ARGB color) {
		if (!visible || !graphicDevice)
			return;

		spritedata.objrect = spriteData.objrect; // use this images rect for select texture
		spritedata.texture = textureManager->getTexture();

		if (color == GraphColor::FILTER)
			graphicDevice->drawSprite(spritedata, colorFilter); // draw with filter color
		else
			graphicDevice->drawSprite(spritedata, color);

	}

	VOID Image::update(FLOAT frameTime) { // synchronized sprite with elapsed time
		if (endFrame - startFrame > 0) {	// check animation sprite

			animationTimer += frameTime;	// elapsed time

			if (animationTimer > frameDelay) {
				animationTimer -= frameDelay;
				currentFrame += 1;
				if (currentFrame < startFrame || currentFrame > endFrame) {
					if (loop)
						currentFrame = startFrame;
					else {
						currentFrame = endFrame;
						animationCompleted = TRUE;
					}
				}
				setRect();		// set sprite objrect
			}
		}
	}
}