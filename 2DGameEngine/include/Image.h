#pragma once
#include "textureManager.h"
#include "Sprite.h"
#include "GameMath.h"

//
namespace GameEngine {

	class Image {
	protected:
		DirectGraphic* graphicDevice;
		TextureManager* textureManager;
		SpriteData spriteData;	// contaion data to required to draw the image
		COLOR_ARGB colorFilter; // applied as a color filter(WHITE - default)
		UINT cols;			// number of columns (1 to size) on the grid in multi-frame sprite
		UINT startFrame;	// first frame in animation
		UINT endFrame;		// end frame in animation
		UINT currentFrame;
		FLOAT frameDelay;	// how long between frames change animation
		FLOAT animationTimer;	
		BOOL animationCompleted; // check for animation is completed
		BOOL loop;			// true for loop frames
		BOOL visible;		// visible frame
		BOOL initialized;	// check init 
		HRESULT result;		// result operation
	public:
		Image();
		virtual ~Image();
		const virtual SpriteData& getSpiteInfo() const { return spriteData; }
		// getters
		virtual BOOL getVisible() const { return visible; }
		virtual FLOAT getX() const { return spriteData.x; }
		virtual FLOAT getY() const { return spriteData.y; }
		virtual FLOAT getScale() const { return spriteData.scale; }
		virtual INT32 getWidth() const { return spriteData.width; }
		virtual INT32 getHeight() const { return spriteData.height; }
		virtual FLOAT getCenterX() const { return spriteData.x + spriteData.width / 2 * spriteData.scale; }
		virtual FLOAT getCenterY() const { return spriteData.y + spriteData.height / 2 * spriteData.scale; }
		virtual FLOAT getDegreesAngle() const { return RadianToDegree(spriteData.angle); }
		virtual FLOAT getRadiansAngle() const { return spriteData.angle; }
		virtual FLOAT getFrameDelay() const { return frameDelay; }
		virtual UINT getStartFrame() const { return startFrame; }
		virtual UINT getEndFrame() const { return endFrame; }
		virtual UINT getCurrentFrame() const { return currentFrame; }
		virtual RECT getSpriteRect() const { return spriteData.objrect; }
		virtual BOOL getAnimateIsComplete() const { return animationCompleted; }
		virtual COLOR_ARGB getColorFilter() const { return colorFilter; }
		const virtual SpriteData& getSpriteData() { return spriteData; }
		const virtual SpriteData& getSpriteInfo() { return spriteData; }

		// setters
		virtual VOID setVisible(BOOL v) { visible = v; }
		virtual VOID setX(FLOAT X) { spriteData.x = X; }
		virtual VOID setY(FLOAT Y) { spriteData.y = Y; }
		virtual VOID setScale(FLOAT scale) { spriteData.scale = scale; }
		virtual VOID setDegreesAngle(FLOAT angle) { spriteData.angle = DegreeToRadian(angle); }
		virtual VOID setRadiansAngle(FLOAT rad) { spriteData.angle = rad; }
		virtual VOID setFrameDelay(FLOAT delay) { frameDelay = delay; }
		virtual VOID setFrames(UINT start, UINT end) { startFrame = start; endFrame = end; }
		virtual VOID setSpirteDataRect(RECT rect) { spriteData.objrect = rect; }
		virtual VOID setLoop(BOOL lp) { loop = lp; }
		virtual VOID setAnimationComplete(BOOL ac) { animationCompleted = ac; }
		virtual VOID setColorFilter(COLOR_ARGB colorfilter) { colorFilter = colorfilter; }
		virtual VOID setTextureManager(TextureManager* texturemanager) { textureManager = texturemanager; }
		virtual VOID flipHorizontal(BOOL flip) { spriteData.flipHorizontal = flip; }
		virtual VOID flipVertical(BOOL flip) { spriteData.flipVertical = flip; }

		// initilize image class 
		virtual BOOL initialize(DirectGraphic* pGDirect, UINT width, UINT height, UINT ncol, TextureManager* pTextureManager);
		virtual VOID setCurrentFrame(UINT currentframe);
		virtual VOID setRect(); // Set spriteData.objrect to draw currentFrame
		virtual VOID draw(COLOR_ARGB color = GraphColor::WHITE); // draw image, using color filter - color
		virtual VOID draw(SpriteData spritedata, COLOR_ARGB color = GraphColor::WHITE);  // draw image, using color filter and sprite datastructure
		virtual VOID update(FLOAT frameTime); // update animation, with frame time to regulate speed
	};

};
