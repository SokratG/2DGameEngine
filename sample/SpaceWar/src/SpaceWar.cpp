#include <SpaceWar.h>
#include "resource.h"

Spacewar::Spacewar() : Game2D(), fpsON(FALSE), countDownOn(FALSE), roundOver(FALSE), ship1Scored(FALSE), ship2Scored(FALSE), 
						torpedo1(WINDOW_WIDTH, WINDOW_HEIGHT), torpedo2(WINDOW_WIDTH, WINDOW_HEIGHT)
{
	fp = GameEngine::FontParameters::initFontParameters(FONT_BORDER, FONT_WIDTH, FONT_HEIGHT, COLUMNS, ROWS, FRAMES,
														ANIM_DELAY, MAX_FONT_HEIGHT, PROPORTIONAL_SPACING, TAB_SIZE, BOLD_SIZE, SOLID);


}

Spacewar::~Spacewar() {
	this->releaseAll();	
}

void Spacewar::initialize(const LPCWSTR APP_TITLE, const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT, BOOL FULLSCREENMODE)
 {
	
	Game2D::initialize(APP_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREENMODE);
	Game2D::initAudio(WAVE_BANK, SOUND_BANK);
	
	Game2D::SetGameIcon(makeIconFromResource(IDI_ICON1));
	ship1 = std::move(Ship(WINDOW_WIDTH / 2 - SHIP_WIDTH / 2, WINDOW_HEIGHT / 6 - SHIP_HEIGHT));
	ship2 = std::move(Ship(WINDOW_WIDTH / 2 - SHIP_WIDTH / 2, WINDOW_HEIGHT / 6 - SHIP_HEIGHT));
	try
	{
		addTexture("nebula", NEBULA_IMAGE);
		addTexture("textures", TEXTURES_IMAGE);
		addDirectTextFont("BigGameFont", FONT_BIG_SIZE, FALSE, FALSE, FONT_NAME);
		addDirectTextFont("FontScore", FONT_SCORE_SIZE, FALSE, FALSE, FONT_SCORE_NAME);
		Game2D::DXTTable["BigGameFont"].setFontColor(FONT_BIG_COLOR);				// table contain font data
	}
	catch (const std::exception&)
	{
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error add new texture in game"));
	}

	if (!nebula.initialize(graphicDriver, 0, 0, 0, TextureTable["nebula"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing nebula image"));

	if (!planet.initialize(this, PlanetParam::WIDTH, PlanetParam::HEIGHT, 2, TextureTable["textures"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing nebula image"));


	if (!ship1.initialize(this, SHIP_WIDTH, SHIP_HEIGHT, SHIP_TEXTURE_COLS, TextureTable["textures"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing ship1 image"));
	ship1.setFrames(SHIP1_START_FRAME, SHIP1_END_FRAME);	// range frame
	ship1.setCurrentFrame(SHIP1_START_FRAME);			// starting frame
	ship1.setColorFilter(GraphColor::LBLUE);
	ship1.setMass(SHIP_MASS);

	if (!ship2.initialize(this, SHIP_WIDTH, SHIP_HEIGHT, SHIP_TEXTURE_COLS, TextureTable["textures"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing ship2 image"));
	ship2.setFrames(SHIP2_START_FRAME, SHIP2_END_FRAME);	// range frame
	ship2.setCurrentFrame(SHIP2_START_FRAME);			// starting frame
	ship2.setColorFilter(GraphColor::LYELLOW);
	ship2.setMass(SHIP_MASS);

	if (!torpedo1.initialize(this, TorpedoParam::WIDTH, TorpedoParam::HEIGHT, TorpedoParam::TEXTURE_COLS, TextureTable["textures"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing torpedo1 image"));
	torpedo1.setFrames(TorpedoParam::START_FRAME, TorpedoParam::END_FRAME);
	torpedo1.setCurrentFrame(TorpedoParam::START_FRAME);
	torpedo1.setColorFilter(GraphColor::LBLUE);


	if (!torpedo2.initialize(this, TorpedoParam::WIDTH, TorpedoParam::HEIGHT, TorpedoParam::TEXTURE_COLS, TextureTable["textures"]))
		throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing torpedo2 image"));
	torpedo2.setFrames(TorpedoParam::START_FRAME, TorpedoParam::END_FRAME);
	torpedo2.setCurrentFrame(TorpedoParam::START_FRAME);
	torpedo2.setColorFilter(GraphColor::LYELLOW);


	healthBar.initialize(graphicDriver, TextureTable["textures"], 0, HEALTHBAR_Y, 2.0f, GraphColor::WHITE);

	ship1.setX(WINDOW_WIDTH / 4 - SHIP_WIDTH);
	ship2.setX(WINDOW_WIDTH - WINDOW_WIDTH / 4);
	ship1.setY(WINDOW_HEIGHT / 2 - SHIP_HEIGHT);
	ship2.setY(WINDOW_HEIGHT / 2);
	ship1.setVelocity(Vec2<real32>(0, -SHIP_SPEED));
	ship2.setVelocity(Vec2<real32>(0, SHIP_SPEED));

	audio->play(ACTION_THEME);

	roundStart();
}

void Spacewar::update() {
	if (input->isKeyDown(VK_ESCAPE))
		exitGame();

	if (countDownOn) {
		countDownTimer -= frameTime;
		if (countDownTimer <= 0)
			countDownOn = FALSE;
	}
	else
	{
		if (ship1.getActive())
		{
			if (input->isKeyDown(SHIP1_FORWARD_KEY))   // if engine on
			{
				ship1.setEngineOn(TRUE);
				audio->play(ENGINE1);
			}
			else
			{
				ship1.setEngineOn(FALSE);
				audio->stop(ENGINE1);
			}
			ship1.setRotation(DIRECTION::NONE);
			if (input->isKeyDown(SHIP1_LEFT_KEY) )   // if turn ship1 left
				ship1.setRotation(DIRECTION::LEFT);
			if (input->isKeyDown(SHIP1_RIGHT_KEY)) // if turn ship1 right
				ship1.setRotation(DIRECTION::RIGHT);
			if (input->isKeyDown(SHIP1_FIRE_KEY))          // if ship1 fire
				torpedo1.fire(ship1);                  // fire torpedo1
		}
		if (ship2.getActive())
		{
			if (input->isKeyDown(SHIP2_FORWARD_KEY)) // if engine on
			{
				ship2.setEngineOn(TRUE);
				audio->play(ENGINE2);
			}
			else
			{
				ship2.setEngineOn(FALSE);
				audio->stop(ENGINE2);
			}
			ship2.setRotation(DIRECTION::NONE);
			if (input->isKeyDown(VK_LEFT))      // if turn ship2 left
				ship2.setRotation(DIRECTION::LEFT);
			if (input->isKeyDown(VK_RIGHT))    // if turn ship2 right
				ship2.setRotation(DIRECTION::RIGHT);
			if (input->isKeyDown(SHIP2_FIRE_KEY))      // if ship2 fire
				torpedo2.fire(ship2);                  // fire torpedo2
		}
		if (roundOver)
		{
			roundTimer -= frameTime;
			if (roundTimer <= 0)
				roundStart();
		}
	}
	ship1.gravityForce(planet, frameTime);
	ship2.gravityForce(planet, frameTime);
	torpedo1.gravityForce(planet, frameTime);
	torpedo2.gravityForce(planet, frameTime);

	// Update the entities
	planet.update(frameTime);
	ship1.update(frameTime);
	ship2.update(frameTime);
	torpedo1.update(frameTime);
	torpedo2.update(frameTime);

}

void Spacewar::processAI() 
{
}



void Spacewar::collisions() 
{
	Vec2<real32> collisionVector;
	// if collision between ship1 and planet
	if (ship1.collidesWith(planet, collisionVector))
	{
		ship1.toPrevPosition();      // move ship out of collision
		ship1.damage(PLANET);
	}
	// if collision between ship2 and planet
	if (ship2.collidesWith(planet, collisionVector))
	{
		ship2.toPrevPosition();  // move ship out of collision
		ship2.damage(PLANET);
	}
	// if collision between ship1 and ship2
	if (ship1.collidesWith(ship2, collisionVector))
	{
		// bounce off other ship
		ship1.bounce(ship2, collisionVector);
		ship2.bounce(ship1, collisionVector.neg());
		ship1.damage(SHIP);
		ship2.damage(SHIP);
	}
	// if collision between torpedos and ships
	if (torpedo1.collidesWith(ship2, collisionVector))
	{
		ship2.damage(TORPEDO);
		torpedo1.setVisible(FALSE);
		torpedo1.setActive(FALSE);
	}
	if (torpedo2.collidesWith(ship1, collisionVector))
	{
		ship1.damage(TORPEDO);
		torpedo2.setVisible(FALSE);
		torpedo2.setActive(FALSE);
	}
	// if collision between torpedos and planet
	if (torpedo1.collidesWith(planet, collisionVector))
	{
		torpedo1.setVisible(FALSE);
		torpedo1.setActive(FALSE);
		audio->play(TORPEDO_CRASH);
	}
	if (torpedo2.collidesWith(planet, collisionVector))
	{
		torpedo2.setVisible(FALSE);
		torpedo2.setActive(FALSE);
		audio->play(TORPEDO_CRASH);
	}

	// check for scores
	if (ship1.getActive() == FALSE && ship2Scored == FALSE)
	{
		ship2Score++;
		ship2Scored = TRUE;
		if (roundOver == FALSE)
		{
			roundTimer = ROUND_TIME;
			roundOver = TRUE;
		}
	}
	if (ship2.getActive() == FALSE && ship1Scored == FALSE)
	{
		ship1Score++;
		ship1Scored = TRUE;
		if (roundOver == FALSE)
		{
			roundTimer = ROUND_TIME;
			roundOver = TRUE;
		}
	}
}

void Spacewar::render() 
{
	static string FPS_S;
	static string score1, score2, countDown;
	graphicDriver->spriteBeginScene();	// begin draw sprite
	
	nebula.draw();                         
	planet.draw();

	Game2D::DXTTable["fontScore"].setFontColor(SHIP1_COLOR);
	score1 = std::to_string(ship1Score);
	Game2D::DXTTable["fontScore"].print(score1, SCORE1_X, SCORE_Y);


	Game2D::DXTTable["fontScore"].setFontColor(SHIP2_COLOR);
	score2 = std::to_string(ship2Score);
	Game2D::DXTTable["fontScore"].print(score2, SCORE2_X, SCORE_Y);

	// display health bars
	healthBar.setX((FLOAT)SHIP1_HEALTHBAR_X);
	healthBar.set(ship1.getHealth());
	healthBar.draw(SHIP1_COLOR);
	healthBar.setX((FLOAT)SHIP2_HEALTHBAR_X);
	healthBar.set(ship2.getHealth());
	healthBar.draw(SHIP2_COLOR);

	ship1.draw();
	ship2.draw();

	torpedo1.draw(GraphColor::FILTER);      
	torpedo2.draw(GraphColor::FILTER);

	if (fpsON){
		FPS_S = std::to_string(Game2D::getFPS());
		Game2D::DXTTable["BigGameFont"].print(FPS_S, 5, 5);
	}
	if (countDownOn) {
		countDown = std::to_string(static_cast<INT32>(ceil(countDownTimer)));
		Game2D::DXTTable["BigGameFont"].print(countDown, COUNT_DOWN_X, COUNT_DOWN_Y);
	}


	graphicDriver->spriteEndScene();	// end draw sprite
}

void Spacewar::releaseAll()  // graphic device was lost
{
	Game2D::releaseAll();
	return;
}

void Spacewar::resetAll()  // graphic device was lost
{
	Game2D::resetAll();
	return;
}


void Spacewar::roundStart()
{
	ship1.setX(WINDOW_WIDTH / 4 - SHIP_WIDTH);
	ship2.setX(WINDOW_WIDTH - WINDOW_WIDTH / 4);
	ship1.setY(WINDOW_HEIGHT / 2 - SHIP_HEIGHT);
	ship2.setY(WINDOW_HEIGHT / 2);
	ship1.setVelocity(Vec2<real32>(0, -SHIP_SPEED));
	ship2.setVelocity(Vec2<real32>(0, SHIP_SPEED));

	ship1.setDegreesAngle(0);
	ship2.setDegreesAngle(180);
	ship1.repair();
	ship2.repair();
	countDownTimer = COUNT_DOWN;
	countDownOn = TRUE;
	roundOver = FALSE;
	ship1Scored = FALSE;
	ship2Scored = FALSE;
}

