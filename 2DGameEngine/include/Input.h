#pragma once
#include <window.h>
#include <WindowsX.h>
#include <Xinput.h>		// for gamepad
#include <string.h>
#include "constants.h"
#include "CoreError.h"
using std::string;


// for high-definition mouse
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC      ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE     ((USHORT) 0x02)
#endif

namespace inputKEYS 
{
	const int KEYS_ARRAY_LEN = 256;     // size of key arrays

	// what values for clear(), bit flag
	const UCHAR KEYS_DOWN = 1;
	const UCHAR KEYS_PRESSED = 2;
	const UCHAR MOUSE = 4;
	const UCHAR TEXT_IN = 8;
	const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;
}

const SHORT GAMEPAD_THUMBSTICK_DEADZONE = (SHORT)(0.20f * 0X7FFF);    // default to 20% of range as deadzone
const SHORT GAMEPAD_TRIGGER_DEADZONE = 20;                      // trigger range 0-255
const DWORD MAX_CONTROLLERS = 4;                                // Maximum number of controllers supported by XInput

// Bit corresponding to gamepad button in state.Gamepad.wButtons
const DWORD GAMEPAD_DPAD_UP = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
const DWORD GAMEPAD_START_BUTTON = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A = 0x1000;
const DWORD GAMEPAD_B = 0x2000;
const DWORD GAMEPAD_X = 0x4000;
const DWORD GAMEPAD_Y = 0x8000;

struct ControllerState
{
	XINPUT_STATE        state;
	XINPUT_VIBRATION    vibration;
	FLOAT               vibrateTimeLeft;    // ms
	FLOAT               vibrateTimeRight;   // ms
	BOOL                connected;
};



namespace GameEngine {
	class Input
	{
	private:
		BOOL keysDown[inputKEYS::KEYS_ARRAY_LEN]; // TRUE if specified key os down
		BOOL keysPressed[inputKEYS::KEYS_ARRAY_LEN]; // True if specifed key was pressed
		string textIn; // entire user input text
		CHAR charIn;	// last character entered
		BOOL newLine; // new line flag
		INT32 mouseX, mouseY;
		INT32 mouseRawX, mouseRawY;	// high-definition mouse screen coordinates
		INT32 mouseWheel;
		RAWINPUTDEVICE Rid[1];	// For high-definition mosue
		BOOL mouseCaptured;
		BOOL mouseLButton;
		BOOL mouseMButton;
		BOOL mouseRButton;
		BOOL mouseX1Button;	// mouse X1 button down
		BOOL mouseX2Button; // mouse X2 button down
		ControllerState controllers[MAX_CONTROLLERS]; // state of controllers
		SHORT thumbstickDeadzone;
		SHORT triggerDeadzone;
	public:
		Input();
		virtual ~Input();
		virtual VOID initialize(HWND hWnd, BOOL capture = FALSE);
		// Update connection status of game controllers.
		VOID checkControllers() noexcept;
		VOID keyDown(WPARAM wParam) noexcept;
		VOID keyUp(WPARAM wParam) noexcept;
		VOID keyIn(WPARAM wParam) noexcept;
		BOOL isKeyDown(UCHAR vkey) const;
		BOOL isKeyPressed(UCHAR vkey) const;
		BOOL anyKeyPressed() const;
		string getTextIn() const;
		CHAR getCharIn() const;
		VOID clearKeyPress(UCHAR vkey) noexcept;
		VOID clearTextIn() noexcept;
		VOID clearAll() noexcept;
		VOID clear(UCHAR ch) noexcept;
		VOID mouseIn(LPARAM lParam) noexcept;
		VOID mouseRawIn(LPARAM lParam) noexcept;
		VOID setMouseLButton(BOOL state) noexcept;
		VOID setMouseMButton(BOOL state) noexcept;
		VOID setMouseRButton(BOOL state) noexcept;
		VOID setMouseXButton(WPARAM wParam) noexcept;
		INT32 getMouseX() const;
		INT32 getMouseY() const;
		INT32 getMouseRawX() const;
		INT32 getMouseRawY() const;
		BOOL getMouseLButton() const;
		BOOL getMouseMButton() const;
		BOOL getMouseRButton() const;
		BOOL getMouseX1Button() const;
		BOOL getMouseX2Button() const;
		VOID readControllers() noexcept;
		const ControllerState& getControllerState(UINT n);
		const WORD getGamepadButtons(UINT n) const;
		BOOL getGamepadConnected(UINT n) const;
		BOOL getGamepadDPadUp(UINT n) const;
		BOOL getGamepadDPadDown(UINT n) const;
		BOOL getGamepadDPadLeft(UINT n) const;
		BOOL getGamepadDPadRight(UINT n) const;
		BOOL getGamepadStart(UINT n) const;
		BOOL getGamepadBack(UINT n) const;
		BOOL getGamepadLeftThumb(UINT n) const;
		BOOL getGamepadRightThumb(UINT n) const;
		BOOL getGamepadLeftShoulder(UINT n) const;
		BOOL getGamepadRightShoulder(UINT n) const;
		BOOL getGamepadA(UINT n) const;
		BOOL getGamepadB(UINT n) const;
		BOOL getGamepadX(UINT n) const;
		BOOL getGamepadY(UINT n) const;
		BYTE getGamepadLeftTrigger(UINT n) const;
		BYTE getGamepadLeftTriggerUndead(UINT n) const;
		BYTE getGamepadRightTrigger(UINT n) const;
		BYTE getGamepadRightTriggerUndead(UINT n) const;
		SHORT getGamepadThumbLX(UINT n) const;
		SHORT getGamepadThumbLXUndead(UINT n) const;
		SHORT getGamepadThumbLY(UINT n) const;
		SHORT getGamepadThumbLYUndead(UINT n) const;
		SHORT getGamepadThumbRX(UINT n) const;
		SHORT getGamepadThumbRXUndead(UINT n) const;
		SHORT getGamepadThumbRY(UINT n) const;
		SHORT getGamepadThumbRYUndead(UINT n) const;
		VOID gamePadVibrateLeft(UINT n, WORD speed, FLOAT sec);
		VOID gamePadVibrateRight(UINT n, WORD speed, FLOAT sec);
		VOID vibrateControllers(FLOAT frameTime);
	};

}
