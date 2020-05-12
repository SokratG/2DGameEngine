#include "Game2DEngine.h"


namespace GameEngine {

	Game2D* Game2D::currentGame = nullptr;

	LRESULT WINAPI WinProcWrapper(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		return Game2D::currentGame->messageHandler(hWnd, msg, wParam, lParam); // only for message handler
	}

	Game2D::Game2D() : 
		fps(0.f), frameTime(0.f), timeStart{ 0 }, timeEnd{ 0 }, timerFreq{0}, sleepTime(0)
	{
		input = new Input;
		audio = nullptr;
		textdraw = nullptr;
		result = FALSE;
		paused = FALSE;
		graphicDriver = nullptr;
		initialized = FALSE;
		wnd = nullptr;
	}	

	VOID Game2D::initialize(const LPCWSTR APP_TITLE, const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT, BOOL FULLSCREENMODE)
	{
		Game2D::currentGame = this;
		try {
			wnd = Window::CreateWindowClass();
			wnd->CreateMainWindow(GetModuleHandle(NULL), APP_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREENMODE, WinProcWrapper);
			graphicDriver = DirectGraphic::CreateDirectGraphic();
			graphicDriver->initialize(wnd, WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREENMODE);
			input->initialize(*wnd->getHWND(), FALSE); // init input, and don't capture mouse
			audio = Audio::CreateAudioControl();
			textdraw = new Text;
		}
		catch (CoreError::GameEngineError& ge) {
			throw ge;
		}
		catch (...) {
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Unknow error, when try init window and/or graphic device");
		}

		if (QueryPerformanceFrequency(&timerFreq) == FALSE) {
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error init high resolution timer");
		}
		QueryPerformanceCounter(&timeStart);
		initialized = TRUE;

	}

	VOID Game2D::initAudio(LPCWSTR WAVE_BANK_SOURCE, LPCWSTR SOUND_BANK_SOURCE)
	{
		if (WAVE_BANK_SOURCE && SOUND_BANK_SOURCE) {
			result = audio->initialize(WAVE_BANK_SOURCE, SOUND_BANK_SOURCE);
			if (FAILED(result)) {
				if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
					throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error failed to initialize sound system, media file not found");
				else
					throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error failed to initialize sound system");
			}
		}
	}

	VOID Game2D::initTextDraw(LPCWSTR filename, FontParameters& fp)
	{
		if (filename) {
			BOOL res = textdraw->initialize(graphicDriver, filename, fp);
			if (res == FALSE)
				throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error failed to initialize text draw system");
		}
	}

	VOID Game2D::setCurrentGame(Game2D* _pGame) {
		Game2D::currentGame = _pGame;
	}

	VOID Game2D::renderGame()
	{
		if (SUCCEEDED(graphicDriver->beginScene())) {
			// Render in inheriting class, which provide this
			render();

			// end of render scene
			graphicDriver->endScene();
		}
		handleLostGraphicDevice(); // check graphic device

		// display backbuffer 
		graphicDriver->showBackBuffer();
	}


	VOID Game2D::run()
	{
		if (graphicDriver == nullptr)
			return;
		// calc elapsed time from last frame, and save him
		QueryPerformanceCounter(&timeEnd);
		frameTime = static_cast<real32>(timeEnd.QuadPart - timeStart.QuadPart) / timerFreq.QuadPart;

		// Power-saving code
		// If not enough time has elapsed for desired frame rate
		if (frameTime < MIN_FRAME_TIME) // for control game speed
		{
			sleepTime = static_cast<DWORD>((MIN_FRAME_TIME - frameTime) * 1000);
			timeBeginPeriod(1); // request 1ms resolution for windows timer
			Sleep(sleepTime);
			timeEndPeriod(1);  // end request
			return;
		}
		if (frameTime > 0.f)
			fps = (fps * 0.99f) + (0.1f / frameTime);	// Average FPS
		if (frameTime > MAX_FRAME_TIME)
			frameTime = MAX_FRAME_TIME;	// if frame rate is slow limit max frameTime
		timeStart = timeEnd;
		
		if (!paused) {
			update();
			processAI();
			collisions();
			input->vibrateControllers(frameTime); // handle controller vibration
		}
		renderGame();
		input->readControllers(); // read state controllers

		audio->run();                       // perform periodic sound engine tasks

#define ON
#ifdef ON
		if (input->isKeyDown(ALT_KEY) && input->isKeyPressed(ENTER_KEY))
			setDisplayMode(GraphColor::TOGGLE); // toggle fullscreen/window

		// if Esc key, set window mode
		if (input->isKeyDown(ESC_KEY))
			setDisplayMode(GraphColor::WINDOW); // set window mode
#endif
#undef ON
		// clear input for next game loop cycle
		input->clear(inputKEYS::KEYS_PRESSED);
	}


	Game2D::~Game2D()
	{
		deleteAll();
		ShowCursor(TRUE);
	}

	LRESULT WINAPI Game2D::messageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (initialized) {
			switch (msg)
			{
			case WM_KEYDOWN:	
			case WM_SYSKEYDOWN:
				input->keyDown(wParam);
				return 0;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				input->keyUp(wParam);
				return 0;
			case WM_CHAR:	// character entered
				input->keyIn(wParam);
				return 0;
			case WM_MOUSEMOVE:
				input->mouseIn(lParam);
				return 0;
			case WM_INPUT:
				input->mouseRawIn(lParam);
				return 0;
			case WM_LBUTTONDOWN:	// check left button click mouse
				input->setMouseLButton(TRUE);
				input->mouseIn(lParam);
				return 0;
			case WM_LBUTTONUP:
				input->setMouseLButton(FALSE);
				input->mouseIn(lParam);
				return 0;
			case WM_MBUTTONDOWN:	// check middle button click mouse
				input->setMouseMButton(TRUE);
				input->mouseIn(lParam);
				return 0;
			case WM_MBUTTONUP:
				input->setMouseMButton(FALSE);
				input->mouseIn(lParam);
				return 0;
			case WM_RBUTTONDOWN:	// check middle button click mouse
				input->setMouseRButton(TRUE);
				input->mouseIn(lParam);
				return 0;
			case WM_RBUTTONUP:
				input->setMouseRButton(FALSE);
				input->mouseIn(lParam);
				return 0;
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
				input->setMouseXButton(wParam);
				input->mouseIn(lParam);
				return 0;
			case WM_DEVICECHANGE:	// check controllers
				input->checkControllers();
				return 0;
			}
		}
		return DefaultWndProc(hWnd, msg, wParam, lParam);
	}


	VOID Game2D::releaseAll()
	{
		for (auto& texture : TextureTable)
			texture.second->onLostDevice();
		for (auto& text : DXTTable)
			text.second.onLostDevice();
		textdraw->onLostDevice();
	}
	VOID Game2D::resetAll()
	{
		for (auto& texture : TextureTable)
			texture.second->onResetDevice();
		for (auto& text : DXTTable)
			text.second.onResetDevice();
		textdraw->onResetDevice();
	}
	VOID Game2D::deleteAll()
	{
		releaseAll();
		SAFE_DELETE(input);
		if (initialized) {
			for (auto& texture : TextureTable)
				SAFE_DELETE(texture.second);			
			SAFE_DELETE(audio);
			SAFE_DELETE(graphicDriver);
			SAFE_DELETE(wnd);
			SAFE_DELETE(textdraw);
		}
		initialized = FALSE;
	}

	VOID Game2D::handleLostGraphicDevice() noexcept
	{
		result = graphicDriver->getDeviceState();
		if (FAILED(result)) {	// check if device is not in valid state
			if (result == D3DERR_DEVICELOST) {
				Sleep(100);	// yeild CPU 100 ms
				return;
			}
			else if (result == D3DERR_DEVICENOTRESET) {
				releaseAll();
				result = graphicDriver->reset();
				if (FAILED(result))
					return;
				resetAll();
			}
			else
				return;
		}
	}

	VOID Game2D::addTexture(LPCSTR TEXTURE_NAME, LPCWSTR TEXTURE_PATH)
	{
		TextureTable[TEXTURE_NAME] = GameEngine::TextureManager::CreateTextureManager();
		if (!TextureTable[TEXTURE_NAME]->initialize(graphicDriver, TEXTURE_PATH))
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing texture"));
	}

	VOID Game2D::addDirectTextFont(const string& TextName, INT32 FontHeight, BOOL bold, BOOL italic, LPCSTR FontName)
	{
		DXTTable[TextName] = DirectText(wnd->getWidth(), wnd->getHeight());
		if (!DXTTable[TextName].initialize(graphicDriver, FontHeight, bold, italic, FontName))
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing direct text"));
	}


#pragma warning(push)
#pragma warning(disable : 26812)
	VOID Game2D::setDisplayMode(GraphColor::DISPLAY_MODE mode) {
		if (!initialized)
			return;
		releaseAll();
		graphicDriver->changeDisplayMode(mode);
		resetAll();
	}
#pragma warning(pop)

	VOID Game2D::exitGame()
	{	
		PostMessage(*wnd->getHWND(), WM_DESTROY, 0, 0);
	}


	BOOL WINAPI CheckAnotherInstance()
	{
		HANDLE Mutex;
		Mutex = CreateMutex(nullptr, TRUE, L"You have allready start the application!");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			return TRUE;				// Another instance was found
		return FALSE;
	}
}