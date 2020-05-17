#include "MessageDialog.h"




namespace GameEngine {

	MessageDialog::MessageDialog() : MDP()
	{
		graphicDevice = nullptr;
		input = nullptr;
		visible = FALSE;
		DialogVerts = nullptr;
		BorderVerts = nullptr;
		ButtonVerts = nullptr;
		Button2Verts = nullptr;
		hWnd = nullptr;
		ButtonType = MDParam::OK_CANCEL;
		ButtonClicked = 0;
		screenRatioX = 0.f;
		screenRatioY = 0.f;
		ZeroMemory(&MSDVert, sizeof(VertexC) * 4);
		ZeroMemory(&TextRect, sizeof(RECT));
		ZeroMemory(&ButtonRect, sizeof(RECT));
		ZeroMemory(&Button2Rect, sizeof(RECT));
		initialized = FALSE;
	}

	MessageDialog::~MessageDialog()
	{
		onLostDevice(); // call onLostDevice() for every graphics item
	}

	BOOL MessageDialog::initialize(DirectGraphic* pGraphicDevice, Input* pInput, HWND phWnd, const MDParam& MessageDialogParameters)
	{
		try
		{
			graphicDevice = pGraphicDevice;
            game_width = (graphicDevice != nullptr) ? graphicDevice->getGraphicWidth() : 0;
            game_height = (graphicDevice != nullptr) ? graphicDevice->getGraphicHeight() : 0;
			input = pInput;
			hWnd = phWnd;
			MDP = MessageDialogParameters;

            TextRect.bottom = MDP.Y + MDP.HEIGHT - MDP.MARGIN;
            TextRect.left = MDP.X + MDP.MARGIN;
            TextRect.right = MDP.X + MDP.WIDTH - MDP.MARGIN;
            TextRect.top = MDP.Y + MDP.MARGIN;

			if (!DXFont.initialize(graphicDevice, MDP.FONT_HEIGHT, FALSE, FALSE, MDP.FONT))
				return FALSE;
			DXFont.setFontColor(MDP.FONT_COLOR);
		}
		catch (...)
		{
			return FALSE;
		}
		initialized = TRUE;
		return TRUE;
	}


    VOID MessageDialog::prepareVerteces()
    {
        SAFE_RELEASE(DialogVerts);
        SAFE_RELEASE(BorderVerts);
        SAFE_RELEASE(ButtonVerts);
        SAFE_RELEASE(Button2Verts);

        // border top left
        MSDVert[0].x = MDP.X;
        MSDVert[0].y = MDP.Y;
        MSDVert[0].z = 0.0f;
        MSDVert[0].rhw = 1.0f;
        MSDVert[0].color = MDP.BORDER_COLOR;

        // border top right
        MSDVert[1].x = MDP.X + MDP.WIDTH;
        MSDVert[1].y = MDP.Y;
        MSDVert[1].z = 0.0f;
        MSDVert[1].rhw = 1.0f;
        MSDVert[1].color = MDP.BORDER_COLOR;

        // border bottom right
        MSDVert[2].x = MDP.X + MDP.WIDTH;
        MSDVert[2].y = MDP.Y + MDP.HEIGHT;
        MSDVert[2].z = 0.0f;
        MSDVert[2].rhw = 1.0f;
        MSDVert[2].color = MDP.BORDER_COLOR;

        // border bottom left
        MSDVert[3].x = MDP.X;
        MSDVert[3].y = MDP.Y + MDP.HEIGHT;
        MSDVert[3].z = 0.0f;
        MSDVert[3].rhw = 1.0f;
        MSDVert[3].color = MDP.BORDER_COLOR;

        graphicDevice->createVertexBuffer(MSDVert, sizeof(MSDVert), BorderVerts);

        // background top left
        MSDVert[0].x = MDP.X + MDP.BORDER;
        MSDVert[0].y = MDP.Y + MDP.BORDER;
        MSDVert[0].z = 0.0f;
        MSDVert[0].rhw = 1.0f;
        MSDVert[0].color = MDP.BACK_COLOR;

        // background top right
        MSDVert[1].x = MDP.X + MDP.WIDTH - MDP.BORDER;
        MSDVert[1].y = MDP.Y + MDP.BORDER;
        MSDVert[1].z = 0.0f;
        MSDVert[1].rhw = 1.0f;
        MSDVert[1].color = MDP.BACK_COLOR;

        // background bottom right
        MSDVert[2].x = MDP.X + MDP.WIDTH - MDP.BORDER;
        MSDVert[2].y = MDP.Y + MDP.HEIGHT - MDP.BORDER;
        MSDVert[2].z = 0.0f;
        MSDVert[2].rhw = 1.0f;
        MSDVert[2].color = MDP.BACK_COLOR;

        // background bottom left
        MSDVert[3].x = MDP.X + MDP.BORDER;
        MSDVert[3].y = MDP.Y + MDP.HEIGHT - MDP.BORDER;
        MSDVert[3].z = 0.0f;
        MSDVert[3].rhw = 1.0f;
        MSDVert[3].color = MDP.BACK_COLOR;

        graphicDevice->createVertexBuffer(MSDVert, sizeof(MSDVert), DialogVerts);

        // button top left
        MSDVert[0].x = MDP.X + MDP.WIDTH / 2.0f - MDP.BUTTON_WIDTH / 2.0f;
        MSDVert[0].y = MDP.Y + MDP.HEIGHT - MDP.BORDER - MDP.MARGIN - MDP.BUTTON_HEIGHT;
        MSDVert[0].z = 0.0f;
        MSDVert[0].rhw = 1.0f;
        MSDVert[0].color = MDP.BUTTON_COLOR;

        // button top right
        MSDVert[1].x = MDP.X + MDP.WIDTH / 2.0f + MDP.BUTTON_WIDTH / 2.0f;
        MSDVert[1].y = MSDVert[0].y;
        MSDVert[1].z = 0.0f;
        MSDVert[1].rhw = 1.0f;
        MSDVert[1].color = MDP.BUTTON_COLOR;

        // button bottom right
        MSDVert[2].x = MSDVert[1].x;
        MSDVert[2].y = MSDVert[0].y + MDP.BUTTON_HEIGHT;
        MSDVert[2].z = 0.0f;
        MSDVert[2].rhw = 1.0f;
        MSDVert[2].color = MDP.BUTTON_COLOR;

        // button bottom left
        MSDVert[3].x = MSDVert[0].x;
        MSDVert[3].y = MSDVert[2].y;
        MSDVert[3].z = 0.0f;
        MSDVert[3].rhw = 1.0f;
        MSDVert[3].color = MDP.BUTTON_COLOR;

        graphicDevice->createVertexBuffer(MSDVert, sizeof(MSDVert), ButtonVerts);

        // set buttonRect
        ButtonRect.left = (LONG)MSDVert[0].x;
        ButtonRect.right = (LONG)MSDVert[1].x;
        ButtonRect.top = (LONG)MSDVert[0].y;
        ButtonRect.bottom = (LONG)MSDVert[2].y;

        // button2 top left
        MSDVert[0].x = MDP.X + MDP.WIDTH - MDP.BUTTON_WIDTH * 1.2f;
        MSDVert[0].y = MDP.Y + MDP.HEIGHT - MDP.BORDER - MDP.MARGIN - MDP.BUTTON_HEIGHT;
        MSDVert[0].z = 0.0f;
        MSDVert[0].rhw = 1.0f;
        MSDVert[0].color = MDP.BUTTON_COLOR;
        // button2 top right
        MSDVert[1].x = MSDVert[0].x + MDP.BUTTON_WIDTH;
        MSDVert[1].y = MSDVert[0].y;
        MSDVert[1].z = 0.0f;
        MSDVert[1].rhw = 1.0f;
        MSDVert[1].color = MDP.BUTTON_COLOR;
        // button2 bottom right
        MSDVert[2].x = MSDVert[1].x;
        MSDVert[2].y = MSDVert[0].y + MDP.BUTTON_HEIGHT;
        MSDVert[2].z = 0.0f;
        MSDVert[2].rhw = 1.0f;
        MSDVert[2].color = MDP.BUTTON_COLOR;
        // button2 bottom left
        MSDVert[3].x = MSDVert[0].x;
        MSDVert[3].y = MSDVert[2].y;
        MSDVert[3].z = 0.0f;
        MSDVert[3].rhw = 1.0f;
        MSDVert[3].color = MDP.BUTTON_COLOR;
        graphicDevice->createVertexBuffer(MSDVert, sizeof(MSDVert), Button2Verts);

        // set button2Rect
        Button2Rect.left = (LONG)MSDVert[0].x;
        Button2Rect.right = (LONG)MSDVert[1].x;
        Button2Rect.top = (LONG)MSDVert[0].y;
        Button2Rect.bottom = (LONG)MSDVert[2].y;
    }


    VOID MessageDialog::draw()
    {
        if (!visible || !graphicDevice || !initialized)
            return;

        graphicDevice->drawQuad(BorderVerts);        // draw border
        graphicDevice->drawQuad(DialogVerts);        // draw backdrop
        graphicDevice->drawQuad(ButtonVerts);        // draw button
        graphicDevice->drawQuad(Button2Verts);       // draw button2

        graphicDevice->spriteBeginScene();                

        if (Text.size() == 0)
            return;
        // display text on MessageDialog
        DXFont.setFontColor(MDP.FONT_COLOR);
        DXFont.print(Text, TextRect, DT_CENTER | DT_WORDBREAK);

        // display text on buttons
        DXFont.setFontColor(MDP.BUTTON_FONT_COLOR);
        DXFont.print(MDP.BUTTON1_TEXT[ButtonType], ButtonRect,
            DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        DXFont.print(MDP.BUTTON2_TEXT[ButtonType], Button2Rect,
            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        graphicDevice->spriteEndScene();   

       
    }


	VOID MessageDialog::print(const string& str)
	{
		if (!initialized || visible)    // if not initialized or already in use
			return;
		Text = str + "\n\n\n";        // leave some room for buttons

		// Set textRect to text area of dialog
		TextRect.left = (LONG)(MDP.X + MDP.MARGIN);
		TextRect.right = (LONG)(MDP.X + MDP.WIDTH - MDP.MARGIN);
		TextRect.top = (LONG)(MDP.Y + MDP.MARGIN);
		TextRect.bottom = (LONG)(MDP.Y + MDP.HEIGHT - MDP.MARGIN);

		// Set textRect.bottom to precise height required for text
		// No text is printed with DT_CALDRECT option.
		DXFont.print(Text, TextRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
		MDP.HEIGHT = TextRect.bottom - (INT32)MDP.Y + MDP.BORDER + MDP.MARGIN;

		prepareVerteces();                 // prepare the vertex buffers
		ButtonClicked = 0;              // clear buttonClicked
		visible = TRUE;
	}


    VOID MessageDialog::update()
    {
        if (!initialized || !visible)
            return;
        if (input->isKeyPressed(MDP.DIALOG_CLOSE_KEY))
        {
            visible = FALSE;
            ButtonClicked = 1;              // button1 was clicked
            return;
        }

        if (graphicDevice->getFullScreen() == FALSE) // if windowed
        {           
            // calculate screen ratios incase window was resized
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            screenRatioX = (FLOAT)game_width / clientRect.right;
            screenRatioY = (FLOAT)game_height / clientRect.bottom;
        }

        if (input->getMouseLButton())       // if mouse left button
        {
            // if mouse clicked inside button1 (OK)
            if (input->getMouseX() * screenRatioX >= ButtonRect.left &&
                input->getMouseX() * screenRatioX <= ButtonRect.right &&
                input->getMouseY() * screenRatioY >= ButtonRect.top &&
                input->getMouseY() * screenRatioY <= ButtonRect.bottom)
            {
                visible = FALSE;            // hide message dialog
                ButtonClicked = 1;          // button1 was clicked
                return;
            }

            // if mouse clicked inside button2 (cancel)
            if (input->getMouseX() * screenRatioX >= Button2Rect.left &&
                input->getMouseX() * screenRatioX <= Button2Rect.right &&
                input->getMouseY() * screenRatioY >= Button2Rect.top &&
                input->getMouseY() * screenRatioY <= Button2Rect.bottom)
            {
                visible = FALSE;            // hide message dialog
                ButtonClicked = 2;          // button2 was clicked
            }
        }
    }


	VOID MessageDialog::onResetDevice()
	{
		if (!initialized)
			return;
		prepareVerteces();
		DXFont.onResetDevice();
	}

	VOID MessageDialog::onLostDevice()
	{
		if (!initialized)
			return;
		DXFont.onLostDevice();
		SAFE_RELEASE(DialogVerts);
		SAFE_RELEASE(BorderVerts);
		SAFE_RELEASE(ButtonVerts);
		SAFE_RELEASE(Button2Verts);
	}
}