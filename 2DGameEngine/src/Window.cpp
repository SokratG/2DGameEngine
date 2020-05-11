#include "Window.h"

namespace GameEngine {
	// 


	Window::Window() : FullScreen(FALSE), MainIsCreate(FALSE), X(0), Y(0), Width(0), Height(0)
	{
		try
		{
			this->wcx = new WNDCLASSEX;
			this->hWnd = new HWND;
		}
		catch (const std::bad_alloc& alloc)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, alloc.what()));
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating window class"));
		}
	}

	Window::~Window()
	{
		SAFE_DELETE(this->hWnd);
		SAFE_DELETE(this->wcx);
	}

	BOOL WINAPI Window::CreateMainWindow(HINSTANCE hInstance, const LPCWSTR APP_TITLE,
		const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT,
		BOOL FULLSCREEN,
		LRESULT(*handlerproc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam))
	{

		if (MainIsCreate)
			return FALSE;

		MainIsCreate = TRUE;

		// X = (GetSystemMetrics(SM_CXSCREEN) >> 1) - WINDOW_WIDTH * 0.5; // take a center of window
		// Y = (GetSystemMetrics(SM_CYSCREEN) >> 1) - WINDOW_HEIGHT * 0.5;
		X = 0;
		Y = 0;
		Width = WINDOW_WIDTH;
		Height = WINDOW_HEIGHT;

		this->FullScreen = FULLSCREEN;

		DWORD Style = 0;
		if (FULLSCREEN)
			Style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
		else
			Style = WS_OVERLAPPEDWINDOW;



		if (!SetupWindow(Style, hInstance, CLASS_NAME, APP_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, handlerproc))
			return FALSE;


		if (!FULLSCREEN) {
			AdjustWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
		}

		ShowWindow(*hWnd, SW_SHOWDEFAULT); // SW_SHOWNORMAL

		UpdateWindow(*hWnd);

		return TRUE;
	}

	Window* WINAPI Window::CreateWindowClass()
	{
		Window* wnd = nullptr;
		try
		{
			wnd = new Window;
		}
		catch (const std::bad_alloc& alloc)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, alloc.what()));
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating window class"));
		}
		return wnd;
	}

	// Subroutine for setup window 
	BOOL WINAPI Window::SetupWindow(DWORD Style, HINSTANCE hInstance, const LPCWSTR CLASS_NAME, const LPCWSTR APP_TITLE,
		const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT,
		LRESULT(*handlerproc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)) noexcept
	{
		wcx->cbSize = sizeof(*wcx);
		wcx->style = CS_HREDRAW | CS_VREDRAW;
		if (!handlerproc)
			wcx->lpfnWndProc = DefaultWndProc;
		else
			wcx->lpfnWndProc = handlerproc;
		wcx->cbClsExtra = 0;
		wcx->cbWndExtra = 0;
		wcx->hInstance = hInstance;
		wcx->hIcon = nullptr;
		wcx->hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcx->hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcx->lpszMenuName = nullptr;
		wcx->lpszClassName = CLASS_NAME;
		wcx->hIconSm = nullptr;


		if (RegisterClassEx(wcx) == FALSE)
			return FALSE;


		*this->hWnd = CreateWindowW(
			CLASS_NAME,
			APP_TITLE,
			Style,
			CW_USEDEFAULT,
			0,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			(HWND) nullptr,	// no parent window
			(HMENU) nullptr,
			hInstance,
			(LPVOID) nullptr
		);

		if (*hWnd == nullptr)
			return FALSE;

		return TRUE;
	}

#pragma warning(push)
#pragma warning(disable : 26812)
	VOID Window::changeDisplayMode(GraphColor::DISPLAY_MODE mode)
	{
		try
		{
			switch (mode)
			{
			case GraphColor::FULLSCREEN:
				if (FullScreen)
					return;
				FullScreen = TRUE;
				break;
			case GraphColor::WINDOW:
				if (!FullScreen)
					return;
				FullScreen = FALSE;
				break;
			case GraphColor::TOGGLE:
				FullScreen = !FullScreen;
				break;
			}
			if (FullScreen)
				SetWindowLongA(*hWnd, GWL_STYLE, WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
			else {
				resetWindowMode();
				// adjust window size to client area
				AdjustWindow(Width, Height);

			}
		}
		catch (...) {
			resetWindowMode();
			// adjust window size to client area
			AdjustWindow(Width, Height);
		}
	};
#pragma warning(pop)

	// Adjust window size so client area
	VOID Window::AdjustWindow(const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT) noexcept
	{
		RECT clientArea;
		GetClientRect(*this->hWnd, &clientArea);

		MoveWindow(*this->hWnd, X, Y,
					WINDOW_WIDTH + (WINDOW_WIDTH - clientArea.right),
					WINDOW_HEIGHT + (WINDOW_HEIGHT - clientArea.bottom),
					TRUE);	// repaint call

	}
	
	VOID Window::resetWindowMode() {
		SetWindowLongA(*hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(*hWnd,			// handle window
			HWND_TOP,		// window to top Z order(over windows) 
			0, 0, Width, Height, // X, Y, width , height
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); // Applies new frame styles | Retains the current position | Retains the current size | Displays the window.
	}

	LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CHAR:               // a character was entered by the keyboard
			switch (wParam)         // the character is in wParam
			{
			case ESC_KEY:              // backspace
				PostQuitMessage(0);
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}


};