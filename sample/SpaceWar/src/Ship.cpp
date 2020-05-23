#include "Ship.h"

Ship::Ship() : Entity(), shieldOn(FALSE) , engineOn(FALSE), explosionOn(FALSE)
{
	GAME_WIDTH = 0;
	GAME_HEIGHT = 0;
	spriteData.width = SHIP_WIDTH;
	spriteData.height = SHIP_HEIGHT;
	spriteData.x = 0;
	spriteData.y = 0;
	spriteData.objrect.right = SHIP_WIDTH;
	spriteData.objrect.bottom = SHIP_HEIGHT;
	Entity::setVelocity(Vec2<real32>(0, 0));
	frameDelay = SHIP_ANIMATION_DELAY;
	startFrame = SHIP1_START_FRAME;
	endFrame = SHIP1_END_FRAME;
	currentFrame = startFrame;
	prevX = 0;
	prevY = 0;
	prevAngle = 0.f;
	rotation = 0.f;
	Entity::setCollisionType(GameEngine::COLLISION_TYPE::CIRCLE);
	Entity::setRadius(SHIP_WIDTH / 2);
	Entity::setMass(SHIP_MASS);
	direct = DIRECTION::NONE;
}


Ship::Ship(UINT X, UINT Y) : Entity(), shieldOn(FALSE), engineOn(FALSE), explosionOn(FALSE)
{
	GAME_WIDTH = 0;
	GAME_HEIGHT = 0;
	spriteData.width = SHIP_WIDTH;
	spriteData.height = SHIP_HEIGHT;
	spriteData.x = X;
	spriteData.y = Y;
	spriteData.objrect.right = SHIP_WIDTH;
	spriteData.objrect.bottom = SHIP_HEIGHT;
	Entity::setVelocity(Vec2<real32>(0, 0));
	frameDelay = SHIP_ANIMATION_DELAY;
	startFrame = SHIP1_START_FRAME;
	endFrame = SHIP1_END_FRAME;
	currentFrame = startFrame;
	prevX = X;
	prevY = Y;
	prevAngle = 0.f;
	rotation = 0.f;
	Entity::setCollisionType(GameEngine::COLLISION_TYPE::CIRCLE);
	Entity::setRadius(SHIP_WIDTH / 2);
	Entity::setMass(SHIP_MASS);
	direct = DIRECTION::NONE;
}

BOOL Ship::initialize(GameEngine::Game2D* pGame, INT32 width, INT32 height, UINT ncol, GameEngine::TextureManager* pTextureM)
{
	GAME_WIDTH = pGame->getGraphicDriver()->getGraphicWidth();
	GAME_HEIGHT = pGame->getGraphicDriver()->getGraphicHeight();
	// shield init
	shield.initialize(pGame->getGraphicDriver(), width, height, ncol, pTextureM);
	shield.setFrames(SHIELD_START_FRAME, SHIELD_END_FRAME);
	shield.setCurrentFrame(SHIELD_START_FRAME);
	shield.setFrameDelay(SHIELD_ANIMATION_DELAY);
	shield.setLoop(FALSE);
	// engine init
	engine.initialize(pGame->getGraphicDriver(), width, height, ncol, pTextureM);
	engine.setFrames(ENGINE_START_FRAME, ENGINE_END_FRAME);
	engine.setCurrentFrame(ENGINE_START_FRAME);
	engine.setFrameDelay(ENGINE_ANIMATION_DELAY);
	// explosion init
	explosion.initialize(pGame->getGraphicDriver(), width, height, ncol, pTextureM);
	explosion.setFrames(EXPLOSION_START_FRAME, EXPLOSION_END_FRAME);
	explosion.setCurrentFrame(EXPLOSION_START_FRAME);
	explosion.setFrameDelay(EXPLOSION_ANIMATION_DELAY);
	explosion.setLoop(FALSE);
	return (Entity::initialize(pGame, width, height, ncol, pTextureM));
}
	

VOID Ship::toPrevPosition() noexcept
{
	rotation = 0.f;
	spriteData.x = prevX;
	spriteData.y = prevY;
	spriteData.angle = prevAngle;
}

VOID Ship::draw()
{
	if (explosionOn)	// draw explosion ship if on
		explosion.draw(spriteData);
	else {
	Image::draw();
	if (engineOn)
		engine.draw(spriteData);
	if (shieldOn)
		shield.draw(spriteData, GraphColor::ALPHA50 & colorFilter);
	}
}

VOID Ship::update(FLOAT frameTime)
{
	if (explosionOn) {
		explosion.update(frameTime);
		if (explosion.getAnimateIsComplete()) {
			explosionOn = FALSE;
			visible = FALSE;
			explosion.setAnimationComplete(FALSE);
			explosion.setCurrentFrame(EXPLOSION_START_FRAME);
		}
	}

	if (shieldOn){
		shield.update(frameTime);
		if (shield.getAnimateIsComplete()){
			shieldOn = FALSE;
			shield.setAnimationComplete(FALSE);
		}
	}

	if (engineOn) {
		Vec2<real32> SpeedUp(cos_rad(spriteData.angle) * SHIP_SPEED * frameTime, sin_rad(spriteData.angle) * SHIP_SPEED * frameTime);
		Vec2<real32> vel = getVelocity() + SpeedUp;
		setVelocity(vel);
		engine.update(frameTime);
	}

	Entity::update(frameTime);
	prevX = spriteData.x;
	prevY = spriteData.y;
	prevAngle = spriteData.angle;

	switch (direct)
	{
	case LEFT:
		rotation -= frameTime * SHIP_ROTATION_RATE;
		break;
	case RIGHT:
		rotation += frameTime * SHIP_ROTATION_RATE;
		break;
	}
	spriteData.angle += frameTime * rotation;

	spriteData.x += frameTime * physObj.velocity.x;    
	spriteData.y += frameTime * physObj.velocity.y;
	// Wrap around screen edge
	if (spriteData.x > GAME_WIDTH)            
		spriteData.x = -SHIP_WIDTH;
	else if (spriteData.x < -SHIP_WIDTH)
		spriteData.x = GAME_WIDTH;             
	if (spriteData.y > GAME_HEIGHT)             
		spriteData.y = -SHIP_HEIGHT;
	else if (spriteData.y < -SHIP_HEIGHT)
		spriteData.y = GAME_HEIGHT;             
}

#pragma warning(push)
#pragma warning(disable : 26812)
VOID Ship::damage(WEAPON weapon)
{
	if (shieldOn)
		return;

	switch (weapon)
	{
	case TORPEDO:
		audio->play(TORPEDO_HIT);
		health -= TORPEDO_DAMAGE;
		break;
	case SHIP:
		audio->play(COLLIDE);    // play sound
		health -= SHIP_DAMAGE;
		break;
	case PLANET:
		health = 0;
		break;
	}
	if (health <= 0)
		explode();
	else
		shieldOn = true;
}

VOID Ship::explode() 
{
	audio->play(EXPLODE);
	active = false;
	health = 0;
	explosionOn = true;
	engineOn = false;
	shieldOn = false;
	physObj.velocity.x = 0.0f;
	physObj.velocity.y = 0.0f;
}

VOID Ship::repair()
{
	Entity::health = FULL_HEALTH;
	explosionOn = FALSE;
	engineOn = FALSE;
	shieldOn = FALSE;
	rotation = 0.f;
	direct = DIRECTION::NONE;
	Entity::visible = TRUE;
	Entity::active = TRUE;
}
#pragma warning(pop)
