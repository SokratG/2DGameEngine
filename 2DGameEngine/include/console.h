#pragma once
#include "Window.h"
#include "DirectGraphic.h"
#include "Input.h"
#include "DirectText.h"
#include <deque>

using std::string;
using std::deque;

// create debug console in system
HANDLE CreateConsole(VOID)
{
	static HANDLE _console;
	// detach from the current console window
	// if launched from a console window, that will still run waiting for the new console (below) to close
	FreeConsole();
	// create a separate new console window
	AllocConsole();
	// attach the new console to this application's process
	AttachConsole(GetCurrentProcessId());

	// reopen the std I/O streams to redirect I/O to the new console
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);

	_console = GetStdHandle(STD_OUTPUT_HANDLE);

	return _console;
}

VOID SetColorConsole(_In_ HANDLE& hConsole, _In_ WORD color)
{
	SetConsoleTextAttribute(hConsole, color);
}

VOID ResetConsoleColor(_In_ HANDLE& hConsole)
{
	SetConsoleTextAttribute(hConsole, CDEFUALT);
}


namespace GameEngine {

	struct ConsoleParameters
	{
		UINT WIDTH;
		UINT HEIGHT;
		UINT X;
		UINT Y;
		UINT MARGIN;
		string FONT;
		INT32 FONT_HEIGHT;
		COLOR_ARGB FONT_COLOR;
		COLOR_ARGB BACK_COLOR;
		INT32 MAX_LINES;
		ConsoleParameters() : X(5), Y(5), WIDTH(500), HEIGHT(400), MARGIN(4), FONT("Courier New"),
			FONT_HEIGHT(14), FONT_COLOR(GraphColor::WHITE), BACK_COLOR(GraphColor::GRAY), MAX_LINES(256)
		{
		}
		ConsoleParameters(const ConsoleParameters& cp) {
			X = cp.X; Y = cp.Y; WIDTH = cp.WIDTH; HEIGHT = cp.HEIGHT; FONT = cp.FONT;
			FONT_HEIGHT = cp.FONT_HEIGHT; FONT_COLOR = cp.FONT_COLOR; BACK_COLOR = cp.BACK_COLOR;
			MAX_LINES = cp.MAX_LINES; MARGIN = cp.MARGIN;
		}
		ConsoleParameters& operator=(const ConsoleParameters& cp) {
			X = cp.X; Y = cp.Y; WIDTH = cp.WIDTH; HEIGHT = cp.HEIGHT; FONT = cp.FONT;
			FONT_HEIGHT = cp.FONT_HEIGHT; FONT_COLOR = cp.FONT_COLOR; BACK_COLOR = cp.BACK_COLOR;
			MAX_LINES = cp.MAX_LINES; MARGIN = cp.MARGIN;
			return *this;
		}
	};

	// Game Console
	class GameConsole
	{
	protected:
		DirectGraphic* graphicDevice;
		Input* input;
		DirectText DXFont;
		ConsoleParameters consParam;
		FLOAT x, y;
		INT32 Rows;
		string cmdSTR;		// console command
		string inputSTR;	// console input text
		deque<string> text;	// console text
		COLOR_ARGB FontCOLOR;
		COLOR_ARGB BackgroundCOLOR;
		RECT TextRect;		// rect for text output
		VertexC VertConsole[4];	// vertices for draw rectangle console 
		LP_VERTEXBUFFER vertexBuffer; // vertex buffer for DirectX
		BOOL initialized;
		INT32 scrollAmount;		// number of lines for scrolling the display
		BOOL visible;
	public:
		GameConsole();
		virtual ~GameConsole();
		virtual BOOL initialize(DirectGraphic* pGraphicDevice, Input* inputSystem, ConsoleParameters& cp);
		virtual const VOID draw();
		virtual VOID showHide();
		BOOL getVisible() const { return visible; }
		VOID show() { visible = TRUE; }
		VOID hide() { visible = FALSE; }
		VOID print(const string& str);
		string getCommand();
		string getInputString() { return inputSTR; }
		VOID clearInputString() { inputSTR.clear(); }
		VOID onLostDevice();
		VOID onResetDevice();
	};

}