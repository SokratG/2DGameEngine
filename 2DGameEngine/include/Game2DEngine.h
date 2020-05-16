#pragma once
#include "GameMath.h"
#include "DirectGraphic.h"
#include "Input.h"
#include "Image.h"
#include "Audio.h"
#include "Entity.h"
#include "Text.h"
#include "DirectText.h"


using TexturesTable = std::unordered_map<string, GameEngine::TextureManager*>;
using DXTextTable = std::unordered_map<string, GameEngine::DirectText>;
using Icon = HICON;

namespace GameEngine {

#define MAX_PATH 256

	class Game2D
	{
	protected:
		Window* wnd;
		DirectGraphic* graphicDriver;
		Input* input;
		Audio* audio;
		Text* textdraw;
		HRESULT result;
		LARGE_INTEGER timeStart;	// Perfomance counter start
		LARGE_INTEGER timeEnd;		// Perfomance counter end
		LARGE_INTEGER timerFreq;		// Perfomance counter frequency
		real32 frameTime;			// time required for last frame
		real32 fps;					// frame per second
		DWORD sleepTime;			// ms to sleep between frames
		BOOL paused;				// game pause
		BOOL initialized;			// game is init
		TexturesTable TextureTable;			// texture table
		DXTextTable   DXTTable;
		wstring PATH_APP;
		static Game2D* currentGame;
		friend LRESULT WinProcWrapper(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		Game2D();
		Game2D(const Game2D&) = delete;
		Game2D& operator=(const Game2D&) = delete;
		virtual ~Game2D();
		LRESULT WINAPI messageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // windows message handler
		virtual VOID initialize(const LPCWSTR APP_TITLE, 
								const INT32 WINDOW_WIDTH = GAME_WIDTH, const INT32 WINDOW_HEIGHT = GAME_HEIGHT,
								BOOL FULLSCREENMODE = FALSE); // init game	
		virtual VOID initAudio(LPCWSTR WAVE_BANK_SOURCE, LPCWSTR SOUND_BANK_SOURCE);
		virtual VOID initTextDraw(LPCWSTR filename, FontParameters& fp);
		virtual VOID run(); // run game in main message loop
		virtual VOID releaseAll(); // release all resource
		virtual VOID resetAll(); // recreate all entities
		virtual VOID deleteAll(); // delete all reserved memory
		virtual VOID renderGame(); // render game object
		virtual VOID handleLostGraphicDevice() noexcept;
		DirectGraphic* getGraphicDriver() noexcept {return graphicDriver;};
		Input* getInput() noexcept { return input; }
		Audio* getAudio() noexcept { return audio; }
		Text* getTextDraw() noexcept { return textdraw; }
		VOID exitGame();
		VOID addTexture(LPCSTR TEXTURE_NAME, LPCWSTR TEXTURE_PATH);
		VOID SetGameIcon(const Icon& icon);
		VOID addDirectTextFont(const string& TextName, INT32 FontHeight, BOOL bold, BOOL italic, LPCSTR FontName);
		VOID setDisplayMode(GraphColor::DISPLAY_MODE mode = GraphColor::TOGGLE); // Set display mode (fullscreen, window or toggle)
		virtual Icon makeIconFromResource(USHORT ID_ICO);
		virtual VOID setCurrentGame(Game2D* _pGame) final;
		virtual VOID update() = 0; // update entites
		virtual VOID processAI() = 0; // perfom AI calc
		virtual VOID collisions() = 0; // check collision 

		// Call graphics->spriteBegin();
		//   draw sprites
		// Call graphics->spriteEnd();
		//   draw non-sprites
		virtual VOID render() = 0; // render graphics
	};
	
	

	BOOL WINAPI CheckAnotherInstance(VOID);
}


