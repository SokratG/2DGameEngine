#include "InputDialog.h"


namespace GameEngine {

	InputDialog::InputDialog()
	{
		TextBackgroundColor = INPUT_DIALOG_TEXT_BACK_COLOR;
		TextFontColor = INPUT_DIALOG_TEXT_COLOR;
		InputTextVerts = nullptr;
		ZeroMemory(&InputTextRect, sizeof(RECT));
	}


	InputDialog::~InputDialog()
	{
		onLostDevice();
	}



	VOID InputDialog::prepareVertices()
	{
		MessageDialog::prepareVertices();
		SAFE_RELEASE(InputTextVerts);

		// InputText top left
		MSDVert[0].x = MDP.X + MDP.BORDER * 2;
		MSDVert[0].y = MDP.Y + MDP.HEIGHT - MDP.BORDER - MDP.MARGIN - MDP.BUTTON_HEIGHT * 2.5f;
		MSDVert[0].z = 0.0f;
		MSDVert[0].rhw = 1.0f;
		MSDVert[0].color = TextBackgroundColor;
		// InputText top right
		MSDVert[1].x = MDP.X + MDP.WIDTH - MDP.BORDER * 2;
		MSDVert[1].y = MSDVert[0].y;
		MSDVert[1].z = 0.0f;
		MSDVert[1].rhw = 1.0f;
		MSDVert[1].color = TextBackgroundColor;
		// InputText bottom right
		MSDVert[2].x = MSDVert[1].x;
		MSDVert[2].y = MSDVert[0].y + MDP.BUTTON_HEIGHT;
		MSDVert[2].z = 0.0f;
		MSDVert[2].rhw = 1.0f;
		MSDVert[2].color = TextBackgroundColor;
		// InputText bottom left
		MSDVert[3].x = MSDVert[0].x;
		MSDVert[3].y = MSDVert[2].y;
		MSDVert[3].z = 0.0f;
		MSDVert[3].rhw = 1.0f;
		MSDVert[3].color = TextBackgroundColor;
		graphicDevice->createVertexBuffer(MSDVert, sizeof(MSDVert), InputTextVerts);

		// set InputTextRect
		InputTextRect.left = static_cast<LONG>(MSDVert[0].x);
		InputTextRect.right = static_cast<LONG>(MSDVert[1].x);
		InputTextRect.top = static_cast<LONG>(MSDVert[0].y);
		InputTextRect.bottom = static_cast<LONG>(MSDVert[2].y);
	}

	VOID InputDialog::draw()
	{
		if (!visible || !graphicDevice || !initialized)
			return;

		// draw quad msg dlg
		graphicDevice->drawQuad(BorderVerts);       
		graphicDevice->drawQuad(DialogVerts);     
		graphicDevice->drawQuad(ButtonVerts);        
		graphicDevice->drawQuad(Button2Verts);   
		// draw quad input dlg
		graphicDevice->drawQuad(InputTextVerts);        // draw input text area


		graphicDevice->spriteBeginScene();
		// text from msg dlg
		if (Text.empty())
			return;

		// display text on MessageDialog
		DXFont.setFontColor(MDP.FONT_COLOR);
		DXFont.print(Text, TextRect, DT_CENTER | DT_WORDBREAK);

		// display text on buttons
		DXFont.setFontColor(MDP.BUTTON_FONT_COLOR);
		DXFont.print(MDP.BUTTON1_TEXT[ButtonType], ButtonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DXFont.print(MDP.BUTTON2_TEXT[ButtonType], Button2Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		// display input text
		DXFont.setFontColor(TextFontColor);
		prevTextRect = InputTextRect;      // save
		// No text is printed with DT_CALDRECT option. It moves RECT.right
		DXFont.print(InputText, prevTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
		if (prevTextRect.right > InputTextRect.right)   // if text too long, right justify
			DXFont.print(InputText, InputTextRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
		else    // left justify
			DXFont.print(InputText, InputTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

		graphicDevice->spriteEndScene();
	}

	VOID InputDialog::print(const string& str)
	{
		if (!initialized || visible)    // if not initialized or already in use
			return;
		Text = str + "\n\n\n\n";   // leave some room for input text and buttons

		// Set textRect to text area of dialog
		TextRect.left = static_cast<LONG>(MDP.X + MDP.MARGIN);
		TextRect.right = static_cast<LONG>(MDP.X + MDP.WIDTH - MDP.MARGIN);
		TextRect.top = static_cast<LONG>(MDP.Y + MDP.MARGIN);
		TextRect.bottom = static_cast<LONG>(MDP.Y + MDP.HEIGHT - MDP.MARGIN);

		// Set textRect.bottom to precise height required for text
		// No text is printed with DT_CALDRECT option.
		DXFont.print(Text, TextRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
		MDP.HEIGHT = TextRect.bottom - (INT32)MDP.Y + MDP.BORDER + MDP.MARGIN;

		prepareVertices();                 // prepare the vertex buffers
		InputText = "";                    // clear old input
		input->clearTextIn();
		ButtonClicked = 0;              // clear buttonClicked
		visible = TRUE;
	}



	VOID InputDialog::update()
	{
		MessageDialog::update();
		if (!initialized || !visible) {
			if (ButtonClicked == 2)	// check cancel/No button clicked
				InputText = "";
			return;
		}
		InputText = input->getTextIn(); // save input text
	}

	VOID InputDialog::onLostDevice()
	{
		if (!initialized)
			return;
		MessageDialog::onLostDevice();
		SAFE_RELEASE(InputTextVerts);
	}

}