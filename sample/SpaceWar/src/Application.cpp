#include <SpaceWar.h>

HINSTANCE hInst;





//const BOOL FULLSCREEN = FALSE;



int WINAPI WinMain(_In_ HINSTANCE hInstance, 
					_In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPSTR lpCmdLine, 
					_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// static HANDLE console = CreateConsole();
#endif

	
	
	

	// Prevent Multiple Instances
	if (GameEngine::CheckAnotherInstance())
		return ERROR_APP;
	// init random
	std::srand((std::time(NULL)));

	MSG msg;
	Spacewar* war = nullptr;
	
	try
	{
		 war = new Spacewar;
		 war->initialize(APP_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, FALSE);
	}
	catch (CoreError::GameEngineError& ge)
	{
		MessageBoxA(nullptr, ge.getMessage(), "Error", MB_OK | MB_ICONERROR);
		return ERROR_APP;
	}
	
	/*
	UINT done = 0;
	while (!done) {

			// PeekMessage is a non-blocking test for Windows messages
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				done = COMPLETE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			war->run();

	}
	*/
	GameEngine::Game2D::GameLoop(war, msg);

	SAFE_DELETE(war);
	return static_cast<int>(msg.wParam);
}




