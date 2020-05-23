#pragma once
#include "Entity.h"
#include "Game2DEngine.h"


const INT32 SHIP_WIDTH = 32;
const INT32 SHIP_HEIGHT = 32;
const UINT  SHIP_TEXTURE_COLS = 8;
const UINT  SHIP1_START_FRAME = 0;
const UINT  SHIP1_END_FRAME = 3;
const FLOAT SHIP_MASS = 300.f;
const FLOAT SHIP_ANIMATION_DELAY = 0.2f; // tume between frames of ship animation
const FLOAT SHIP_ROTATION_RATE = _PI; //_PI / 4;		// rotate degree per second
const FLOAT SCALE_RATE = 0.15f;			// 15 % change size per second
const FLOAT SHIP_SPEED = 100.f;			// speed pixel per second
const FLOAT SHIP_SCALE = 1.5f;			// starting ship scale
const UINT  SHIP2_START_FRAME = 8;      // ship2 starts at frame 8
const UINT  SHIP2_END_FRAME = 11;       // ship2 animation frames 8,9,10,11
const UINT  SHIELD_START_FRAME = 24;    // shield start frame
const UINT  SHIELD_END_FRAME = 27;      // shield end frame
const FLOAT SHIELD_ANIMATION_DELAY = 0.1f; // time between frames
const INT32 SHIP1_HEALTHBAR_X = 40;
const INT32 EXPLOSION_START_FRAME = 32; // explosion start frame
const INT32 EXPLOSION_END_FRAME = 39;   // explosion end frame
const FLOAT EXPLOSION_ANIMATION_DELAY = 0.2f;   // time between frames
const INT32 ENGINE_START_FRAME = 16;    // engine start frame
const INT32 ENGINE_END_FRAME = 19;      // engine end frame
const FLOAT ENGINE_ANIMATION_DELAY = 0.1f;  // time between frames
const FLOAT TORPEDO_DAMAGE = 46;        // amount of damage caused by torpedo
const FLOAT SHIP_DAMAGE = 10;           // damage caused by collision with another ship
const FLOAT FULL_HEALTH = 100.f;
const COLOR_ARGB SHIP2_COLOR = GraphColor::YELLOW;
const COLOR_ARGB SHIP1_COLOR = GraphColor::BLUE;
enum WEAPON { TORPEDO, SHIP, PLANET };
enum DIRECTION { NONE, LEFT, RIGHT };
const CHAR COLLIDE[] = "collide";
const CHAR TORPEDO_CRASH[] = "torpedoCrash";
const CHAR TORPEDO_FIRE[] = "torpedoFire";
const CHAR TORPEDO_HIT[] = "torpedoHit";
const CHAR EXPLODE[] = "explode";
const CHAR ENGINE1[] = "engine1";
const CHAR ENGINE2[] = "engine2";

struct Ship : public GameEngine::Entity
{
private:
	DIRECTION direct;
	BOOL explosionOn;
	BOOL engineOn;
	BOOL  shieldOn;
	Image engine;
	Image explosion;
	Image shield;
	FLOAT prevX, prevY, prevAngle;
	FLOAT rotation;
	INT32 GAME_WIDTH;
	INT32 GAME_HEIGHT;
public:
	Ship();
	Ship(UINT X, UINT Y);
	virtual BOOL initialize(GameEngine::Game2D* pGame, INT32 width, INT32 height, UINT ncol, GameEngine::TextureManager* pTextureM);
	virtual VOID draw();
	VOID update(FLOAT frameTime);
	VOID damage(WEAPON weapon);
	// move ship out of collision
	VOID toPrevPosition() noexcept;
	// getters
	BOOL getEngineOn() const { return engineOn; }
	BOOL getShieldOn() const { return shieldOn; }
	BOOL getExplosionOn() const { return explosionOn; }
	FLOAT getRotation() const { return rotation; }
	// setters
	VOID setEngineOn(BOOL EngineOn) { engineOn = EngineOn; }
	VOID setShieldOn(BOOL ShieldOn) { shieldOn = ShieldOn; }
	VOID setRotation(FLOAT Rotate) { rotation = Rotate; }
	VOID setMass(FLOAT Mass) { Entity::setMass(Mass); }
	VOID setDirection(DIRECTION Dir) { direct = Dir; }
	// ship explodes effect
	VOID explode();
	// ship repair 
	VOID repair();

};