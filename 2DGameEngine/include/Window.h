#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <SDKDDKVer.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <mmsystem.h>

#ifdef _DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include <ctime>
#include <string>

#include "CoreError.h"
#include "constants.h"
#define D3D9


namespace GameEngine {
	
	using std::wstring;
	using std::string;

	struct IconResource
	{
		HICON smallIcon;
		HICON standartIcon;
		IconResource(HICON _smallIco = nullptr, HICON _standIco = nullptr) : smallIcon(_smallIco), standartIcon(_standIco) {}
		IconResource(const IconResource& ir) { smallIcon = ir.smallIcon, standartIcon = ir.standartIcon; }
		IconResource& operator=(const IconResource& ir) { smallIcon = ir.smallIcon, standartIcon = ir.standartIcon; return *this; }
	};

	class Window
	{
	private:
		INT32 X;
		INT32 Y;
		INT32 Width;
		INT32 Height;
		BOOL MainIsCreate;
		BOOL FullScreen;
		WNDCLASSEX* wcx;
		HWND* hWnd;
		Window();
		VOID  AdjustWindow(const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT) noexcept;
		BOOL WINAPI SetupWindow(DWORD Style, HINSTANCE hInstance, const LPCWSTR CLASS_NAME, const LPCWSTR APP_TITLE,
			const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT,
			LRESULT(*handlerproc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = nullptr) noexcept;
	public:
		virtual ~Window();

		BOOL WINAPI CreateMainWindow(HINSTANCE hInstance, const LPCWSTR APP_TITLE,
			const INT32 WINDOW_WIDTH, const INT32 WINDOW_HEIGHT,
			BOOL FULLSCREEN = FALSE,
			LRESULT(*handlerproc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = nullptr);

		static Window* WINAPI CreateWindowClass();

		VOID changeDisplayMode(GraphColor::DISPLAY_MODE mode = GraphColor::TOGGLE);
		VOID resetWindowMode();
		HWND* WINAPI getHWND() noexcept {
			return this->hWnd;
		}

		INT32 getWidth() const {
			return Width;
		}
		INT32 getHeight() const {
			return Height;
		}
		INT32 getX() const {
			return X;
		}
		INT32 getY() const {
			return Y;
		}

		VOID setSmallIcon(const HICON& icons);
	};
	LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};
