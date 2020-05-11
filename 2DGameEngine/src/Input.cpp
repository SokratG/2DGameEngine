#include "Input.h"
#include "GameMath.h"

namespace GameEngine {



	Input::Input() : newLine(TRUE), textIn(""), charIn(0), mouseX(0), mouseY(0),
		mouseRawX(0), mouseRawY(0), mouseWheel(0), mouseLButton(FALSE), mouseMButton(FALSE),
		mouseRButton(FALSE), mouseX1Button(FALSE), mouseX2Button(FALSE),
		keysDown{ FALSE }, keysPressed{ FALSE }
	{
		for (size_t i = 0; i < MAX_CONTROLLERS; ++i) {
			controllers[i].vibrateTimeLeft = 0;
			controllers[i].vibrateTimeRight = 0;
		}

		thumbstickDeadzone = GAMEPAD_THUMBSTICK_DEADZONE;    // default
		triggerDeadzone = GAMEPAD_TRIGGER_DEADZONE;          // default
	}

	Input::~Input()
	{
		if (mouseCaptured)
			ReleaseCapture();
	}

	VOID Input::initialize(HWND hWnd, BOOL capture)
	{
		try
		{
			mouseCaptured = capture;
			// Regiseter high-definition mouse
			Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
			Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
			Rid[0].dwFlags = RIDEV_INPUTSINK;
			Rid[0].hwndTarget = hWnd;
			RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
			if (mouseCaptured)
				SetCapture(hWnd);
			// clear controllers state
			ZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
			checkControllers();
		}
		catch (...)
		{
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing input system");
		}
	}


	VOID Input::keyDown(WPARAM wParam) noexcept {
		if (wParam < inputKEYS::KEYS_ARRAY_LEN) {
			keysDown[wParam] = TRUE; // update keys was down
			keysPressed[wParam] = TRUE; // update keys was press
		}
	}

	VOID Input::keyUp(WPARAM wParam) noexcept {
		if (wParam < inputKEYS::KEYS_ARRAY_LEN)
			keysDown[wParam] = FALSE; // update keys was up
	}

	VOID Input::keyIn(WPARAM wParam) noexcept { // save input char in textIn string
		if (newLine) {
			textIn.clear();
			newLine = FALSE;
		}
		if (wParam == '\b') {	// backspace(delete last char)
			if (textIn.length() > 0)	// if character exist
				textIn.erase(textIn.length() - 1); // remove last character
		}
		else {
			textIn += (CHAR)wParam;
			charIn = (CHAR)wParam;
		}
		if ((CHAR)wParam == '\r')	// check 'return' enter
			newLine = TRUE;
	}

	BOOL Input::isKeyDown(UCHAR vkey) const {
		if (vkey < inputKEYS::KEYS_ARRAY_LEN)
			return keysDown[vkey];
		return FALSE;
	}

	BOOL Input::isKeyPressed(UCHAR vkey) const {
		if (vkey < inputKEYS::KEYS_ARRAY_LEN)
			return keysPressed[vkey];
		return FALSE;
	}

	BOOL Input::anyKeyPressed() const {	// return true if any key was presssed in the most recent frame
		for (const auto key : keysPressed) {
			if (key == TRUE)
				return TRUE;
		}
		return FALSE;
	}

	string Input::getTextIn() const {
		return textIn;
	}

	CHAR Input::getCharIn() const {
		return charIn;
	}

	VOID Input::clearKeyPress(UCHAR vkey) noexcept {
		if (vkey < inputKEYS::KEYS_ARRAY_LEN)
			keysPressed[vkey] = FALSE;
	}

	VOID Input::clearTextIn() noexcept {
		textIn.clear();
	}

	VOID Input::clearAll() noexcept {
		this->clear(inputKEYS::KEYS_MOUSE_TEXT);
	}

	VOID Input::clear(UCHAR _clearInputBuffer) noexcept { // clear specified input buffer
		if (_clearInputBuffer & inputKEYS::KEYS_DOWN) {
			for (auto& key : keysDown) 
				key = FALSE;
		}
		if (_clearInputBuffer & inputKEYS::KEYS_PRESSED) {
			for (auto& key : keysPressed)
				key = FALSE;
		}
		if (_clearInputBuffer & inputKEYS::MOUSE) {
			mouseX = 0;
			mouseY = 0;
			mouseRawX = 0;
			mouseRawY = 0;
		}
		if (_clearInputBuffer & inputKEYS::TEXT_IN)
			clearTextIn();
	}

	VOID Input::mouseIn(LPARAM lParam) noexcept {
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
	}

	
	VOID Input::mouseRawIn(LPARAM lParam) noexcept { // reads raw mouse data. Compatible with a high-definition mouse
		UINT dwSize = 40;
		static BYTE lpb[40];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)); // raw input from specified device func

		RAWINPUT* raw = (RAWINPUT *) lpb; // data structure with raw data

		if (raw->header.dwType == RIM_TYPEMOUSE) {
			mouseRawX = raw->data.mouse.lLastX;
			mouseRawY = raw->data.mouse.lLastY;
		}
	}

	VOID Input::setMouseLButton(BOOL state) noexcept {
		mouseLButton = state;
	}
	VOID Input::setMouseMButton(BOOL state) noexcept {
		mouseMButton = state;
	}
	VOID Input::setMouseRButton(BOOL state) noexcept {
		mouseRButton = state;
	}
	VOID Input::setMouseXButton(WPARAM wParam) noexcept {
		mouseX1Button = (wParam & MK_XBUTTON1) ? TRUE : FALSE;
		mouseX2Button = (wParam & MK_XBUTTON2) ? TRUE : FALSE;
	}

	INT32 Input::getMouseX() const {
		return mouseX;
	}
	INT32 Input::getMouseY() const {
		return mouseY;
	}
	INT32 Input::getMouseRawX() const {
		return mouseRawX;
	}
	INT32 Input::getMouseRawY() const {
		return mouseRawY;
	}

	BOOL Input::getMouseLButton() const {
		return mouseLButton;
	}
	BOOL Input::getMouseMButton() const {
		return mouseMButton;
	}
	BOOL Input::getMouseRButton() const {
		return mouseRButton;
	}
	BOOL Input::getMouseX1Button() const {
		return mouseX1Button;
	}
	BOOL Input::getMouseX2Button() const {
		return mouseX2Button;
	}


	VOID Input::checkControllers() noexcept // Check for connected controllers
	{
		for (DWORD num = 0; num < MAX_CONTROLLERS; num++)
		{
			DWORD result = XInputGetState(num, &controllers[num].state);
			controllers[num].connected = (result == ERROR_SUCCESS) ? TRUE : FALSE;
		}
	}

	VOID Input::readControllers() noexcept { // read state of connected controllers
		for (DWORD num = 0; num < MAX_CONTROLLERS; ++num) {
			if (controllers[num].connected) {
				DWORD result = XInputGetState(num, &controllers[num].state);
				if (result == ERROR_DEVICE_NOT_CONNECTED)
					controllers[num].connected = FALSE;
			}
		}
	}

	const ControllerState& Input::getControllerState(UINT n) { // get specified controller state structure
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n];
	}

	const WORD Input::getGamepadButtons(UINT n) const { // get button state on gamepad
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.wButtons;
	}

	BOOL Input::getGamepadConnected(UINT n) const // return gamepad connection state
	{
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].connected;
	}
	// section get gamepad buttons and triggers state
	BOOL Input::getGamepadDPadUp(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_UP) != FALSE);
	}
	BOOL Input::getGamepadDPadDown(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_DOWN) != FALSE);
	}
	BOOL Input::getGamepadDPadLeft(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_LEFT) != FALSE);
	}
	BOOL Input::getGamepadDPadRight(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_DPAD_RIGHT) != FALSE);
	}
	BOOL Input::getGamepadStart(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_START_BUTTON) != FALSE);
	}
	BOOL Input::getGamepadBack(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_BACK_BUTTON) != FALSE);
	}
	BOOL Input::getGamepadLeftThumb(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_LEFT_THUMB) != FALSE);
	}
	BOOL Input::getGamepadRightThumb(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_RIGHT_THUMB) != FALSE);
	}
	BOOL Input::getGamepadLeftShoulder(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_LEFT_SHOULDER) != FALSE);
	}
	BOOL Input::getGamepadRightShoulder(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_RIGHT_SHOULDER) != FALSE);
	}
	BOOL Input::getGamepadA(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_A) != FALSE);
	}
	BOOL Input::getGamepadB(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_B) != FALSE);
	}
	BOOL Input::getGamepadX(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_X) != FALSE);
	}
	BOOL Input::getGamepadY(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return ((controllers[n].state.Gamepad.wButtons & GAMEPAD_Y) != FALSE);
	}

	/*	Return value of controller n Left Trigger (0 through 255).
		Trigger movement less than GAMEPAD_TRIGGER_DEADZONE returns 0.
		Return value is scaled to 0 through 255
	 */
	BYTE Input::getGamepadLeftTrigger(UINT n) const { 
		BYTE value = getGamepadLeftTriggerUndead(n);
		if (value > triggerDeadzone)
			return ((value - triggerDeadzone) * 255) / (255 - triggerDeadzone); // scaled
		else
			return 0;
	}

	BYTE Input::getGamepadLeftTriggerUndead(UINT n) const { // Return value of controller n Left Trigger
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.bLeftTrigger;
	}

	/*	Return value of controller n RIGHT Trigger (0 through 255).
		Trigger movement less than GAMEPAD_TRIGGER_DEADZONE returns 0.
		Return value is scaled to 0 through 255
	 */
	BYTE Input::getGamepadRightTrigger(UINT n) const {
		BYTE value = getGamepadRightTriggerUndead(n);
		if (value > triggerDeadzone)
			return ((value - triggerDeadzone) * 255) / (255 - triggerDeadzone); //adjust magnitude relative to the end of the dead zone
		else
			return 0;
	}
	BYTE Input::getGamepadRightTriggerUndead(UINT n) const { // Return value of controller n Right Trigger
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.bRightTrigger;
	}

	/*	Return value of controller n Left Thumbstick X (-32767 through 32767).
		Stick movement less than thumbstickDeadzone returns 0.
		Return value is scaled to -32768 through 32767  */
	SHORT Input::getGamepadThumbLX(UINT n) const {
		INT32 x = getGamepadThumbLXUndead(n);
		if (x > thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(thumbstickDeadzone, 32767, x);
			x = SCALE_X<FLOAT, INT32>(0., 32767., norm);
		}
		else if (x < -thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(-32767, -thumbstickDeadzone, x);
			x = SCALE_X<FLOAT, INT32>(-32767., 0., norm);
		}	
		else
			x = 0;
		return static_cast<SHORT>(x);
	}
	SHORT Input::getGamepadThumbLXUndead(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.sThumbLX;
	}

	/*	Return value of controller n Left Thumbstick Y 
		Stick movement less than thumbstickDeadzone returns 0.
		Return value is scaled to -32768 through 32767  */
	SHORT Input::getGamepadThumbLY(UINT n) const {
		INT32 y = getGamepadThumbLYUndead(n);
		if (y > thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(thumbstickDeadzone, 32767, y);
			y = SCALE_X<FLOAT, INT32>(0., 32767., norm);
		}
		else if (y < -thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(-32767, -thumbstickDeadzone, y);
			y = SCALE_X<FLOAT, INT32>(-32767., 0., norm);
		}
		else
			y = 0;
		return static_cast<SHORT>(y);
	}
	SHORT Input::getGamepadThumbLYUndead(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.sThumbLY;
	}

	/*	Return value of controller n Right Thumbstick X 
		Stick movement less than thumbstickDeadzone returns 0.
		Return value is scaled to -32768 through 32767  */
	SHORT Input::getGamepadThumbRX(UINT n) const {
		INT32 x = getGamepadThumbRXUndead(n);
		if (x > thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(thumbstickDeadzone, 32767, x);
			x = SCALE_X<FLOAT, INT32>(0., 32767., norm);
		}
		else if (x < -thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(-32767, -thumbstickDeadzone, x);
			x = SCALE_X<FLOAT, INT32>(-32767., 0., norm);
		}
		else
			x = 0;
		return static_cast<SHORT>(x);
	}
	SHORT Input::getGamepadThumbRXUndead(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.sThumbRX;
	}

	/*	Return value of controller n Right Thumbstick Y
		Stick movement less than thumbstickDeadzone returns 0.
		Return value is scaled to -32768 through 32767  */
	SHORT Input::getGamepadThumbRY(UINT n) const {
		INT32 y = getGamepadThumbRYUndead(n);
		if (y > thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(thumbstickDeadzone, 32767, y);
			y = SCALE_X<FLOAT, INT32>(0., 32767., norm);
		}
		else if (y < -thumbstickDeadzone) {
			FLOAT norm = NORM_X<INT32>(-32767, -thumbstickDeadzone, y);
			y = SCALE_X<FLOAT, INT32>(-32767., 0., norm);
		}
		else
			y = 0;
		return static_cast<SHORT>(y);
	}
	SHORT Input::getGamepadThumbRYUndead(UINT n) const {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		return controllers[n].state.Gamepad.sThumbRY;
	}

	// // Vibration gamepad section
	/*	Vibrate controller n motor.		
		speed 0=off, 65536=100 percent
		sec is time to vibrate in seconds */
	// Is low frequency vibration.
	VOID Input::gamePadVibrateLeft(UINT n, WORD speed, FLOAT sec) {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		controllers[n].vibration.wLeftMotorSpeed = speed;
		controllers[n].vibrateTimeLeft = sec;
	}
	// Is high frequency vibration.
	VOID Input::gamePadVibrateRight(UINT n, WORD speed, FLOAT sec) {
		if (n >= MAX_CONTROLLERS)
			n = MAX_CONTROLLERS - 1;
		controllers[n].vibration.wRightMotorSpeed = speed;
		controllers[n].vibrateTimeRight = sec;
	}
	// Vibrates the connected controllers for the desired time.
	VOID Input::vibrateControllers(FLOAT frameTime) {
		for (INT32 num = 0; num < MAX_CONTROLLERS; ++num) {
			if (controllers[num].connected) {
				controllers[num].vibrateTimeLeft -= frameTime;
				if (controllers[num].vibrateTimeLeft < 0) {
					controllers[num].vibrateTimeLeft = 0;
					controllers[num].vibration.wLeftMotorSpeed = 0;
				}
				controllers[num].vibrateTimeRight -= frameTime;
				if (controllers[num].vibrateTimeRight < 0) {
					controllers[num].vibrateTimeRight = 0;
					controllers[num].vibration.wRightMotorSpeed = 0;
				}
				XInputSetState(num, &controllers[num].vibration);
			}
		}
	}
	// //
};
