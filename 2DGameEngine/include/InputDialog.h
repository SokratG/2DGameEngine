#pragma once
#include "MessageDialog.h"


namespace GameEngine {


	static const COLOR_ARGB INPUT_DIALOG_TEXT_BACK_COLOR = GraphColor::WHITE;   // input text background
	static const COLOR_ARGB INPUT_DIALOG_TEXT_COLOR = GraphColor::BLACK;        // input text color


	class InputDialog : public MessageDialog
	{
	private:
		string InputText;
		RECT InputTextRect;
		RECT prevTextRect;
		COLOR_ARGB TextBackgroundColor;
		COLOR_ARGB TextFontColor;
		LP_VERTEXBUFFER InputTextVerts;
	public:
		InputDialog();
		virtual ~InputDialog();
		VOID prepareVertices();
		VOID draw();
		// getters
		string getText() {
			if (!visible)
				return InputText;
			else
				return string();
		}
		// setters
		VOID setTextFontColor(COLOR_ARGB FC) { TextFontColor = FC; }
		VOID setTextBackColor(COLOR_ARGB BC) { TextBackgroundColor = BC; }
		//
		VOID print(const string& str);
		VOID update();
		VOID onLostDevice();
	};




}