#pragma once
#include <Game2DEngine.h>
#include "Ship.h"
#include "Planet.h"
#include "Torpedo.h"
#include "Bar.h"

const LPCWSTR APP_TITLE = L"Space War";
const UCHAR _FIRE_KEY = 'F';
const WCHAR NEBULA_IMAGE[] = L"resource/pictures/orion.jpg";  
const WCHAR TEXTURES_IMAGE[] = L"resource/pictures/textures1.png";
// graphic images font
const WCHAR FONT_IMAGE[] = L"resource/pictures/CKfont.png";
const CHAR FONT_NAME[] = "Arial Bold";
const CHAR FONT_SCORE_NAME[] = "Courier New";
const UINT POINT_SIZE = 14;
const COLOR_ARGB FONT_COLOR = GraphColor::WHITE;
// WAVE_BANK must be location of .xwb file.
const WCHAR WAVE_BANK[] = L"resource/audio/Win/waveBank.xwb";
// SOUND_BANK must be location of .xsb file.
const WCHAR SOUND_BANK[] = L"resource/audio/Win/soundBank.xsb";

// audio cues
const CHAR CHEER[] = "cheer";
const CHAR ACTION_THEME[] = "actionTheme";
// texture parameter
const UINT NEBULA_SIZE = 128;
const INT32 PLANET_SIZE = 128;                  // size of planet image
const INT32 PLANET_FRAME = 1;                   // frame of planet
// font parameter
const INT32 FONT_BIG_SIZE = 256;     // font height
const INT32 FONT_SCORE_SIZE = 48;
const COLOR_ARGB FONT_BIG_COLOR = GraphColor::YELLOW;
const INT32 FONT_BORDER = 3;      // 1 pixel wide transparent border + visible cell border on right and bottom
const INT32 FONT_WIDTH = 48;
const INT32 FONT_HEIGHT = 62;     // font is 62 pixels high
const INT32 GRID_WIDTH = FONT_WIDTH + FONT_BORDER;
const INT32 GRID_HEIGHT = FONT_HEIGHT + FONT_BORDER;
const INT32 COLUMNS = 16;         // number of columns in font image
const INT32 ROWS = 14;            // number of rows in font image
const INT32 FRAMES = 1;           // how many frames of animation (1 = not animated)
const DOUBLE ANIM_DELAY = 0.0;  // delay between animation frames
const INT32 MAX_FONT_HEIGHT = 1000;
const INT32 MIN_CHAR = 0x0020;    // minimum character code
const INT32 MAX_CHAR = 0x00FF;    // maximum character code
const INT32 PROPORTIONAL_SPACING = 5; // pixels between 1:1 scaled characters
const INT32 TAB_SIZE = 8;
const CHAR UNDERLINE = '_';
const CHAR SOLID = 0x7F;        // solid block used for fill character ASCII $7F
const INT32 BOLD_SIZE = 4;        // pixel shift for bold display

// Game input parameter
const UCHAR SHIP1_LEFT_KEY =	'A';
const UCHAR SHIP1_RIGHT_KEY =	'D';
const UCHAR SHIP1_FORWARD_KEY = 'W';
const UCHAR SHIP1_FIRE_KEY =	'S';
const UCHAR SHIP2_LEFT_KEY =	VK_LEFT; // left arrow
const UCHAR SHIP2_RIGHT_KEY =	VK_RIGHT; // right arrow
const UCHAR SHIP2_FORWARD_KEY = VK_UP;   // up arrow
const UCHAR SHIP2_FIRE_KEY =	VK_DOWN;

// Game entity parameter
const INT32 SCORE_Y = 10;
const INT32 SCORE1_X = 60;
const INT32 SCORE2_X = WINDOW_WIDTH - 80;
const INT32 HEALTHBAR_Y = 30;

const INT32 SHIP2_HEALTHBAR_X = WINDOW_WIDTH - 100;
const INT32 HEALTHBAR_START_FRAME = 22;
const INT32 HEALTHBAR_END_FRAME = 22;
const INT32 HEALTHBAR_HEIGHT = 32;
const INT32 HEALTHBAR_WIDTH = 64;
static INT32 COUNT_DOWN_X = WINDOW_WIDTH / 2 - FONT_BIG_SIZE / 4;
static INT32 COUNT_DOWN_Y = WINDOW_HEIGHT / 2 - FONT_BIG_SIZE / 2;;
const INT32 COUNT_DOWN = 3;           // count down
const INT32 ROUND_TIME = 5;           // time until new round starts
const INT32 HEALTHBAR_TEXTURE_COLS = 4;   // health bar is a 4 column texture

class Spacewar : public GameEngine::Game2D
{
private:
	Bar healthBar;
	Ship ship1;
	Ship ship2;
	Planet planet;
	Torpedo torpedo1, torpedo2;
	GameEngine::Image nebula;
	GameEngine::FontParameters fp;
	FLOAT FPS;
	BOOL fpsON;
	BOOL countDownOn;        // true when count down is displayed
	FLOAT countDownTimer;
	BOOL ship1Scored, ship2Scored;
	BOOL roundOver;
	FLOAT roundTimer;
	INT32 ship1Score, ship2Score; // scores
	BOOL musicOff;
public:
	Spacewar();
	virtual ~Spacewar();
	void initialize(const LPCWSTR APP_TITLE, const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT, BOOL FULLSCREENMODE);
	void update();
	void processAI();
	void collisions();
	void render();
	void roundStart();
	void releaseAll();
	void resetAll();
};