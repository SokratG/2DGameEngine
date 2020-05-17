#pragma once
#include "Window.h"
#include "DirectGraphic.h"
#include "Input.h"
#include "DirectText.h"

namespace GameEngine {



	using MDParam = struct MDParam
	{
		UINT WIDTH;
		UINT HEIGHT;
		UINT BORDER;
        UINT MARGIN;             
        string FONT;
        INT32 FONT_HEIGHT;         
        COLOR_ARGB FONT_COLOR;     
        COLOR_ARGB BORDER_COLOR; 
        COLOR_ARGB BACK_COLOR;   
        UINT X; 
        UINT Y;
        UINT BUTTON_WIDTH;
        UINT BUTTON_HEIGHT;
        static const INT32 OK_CANCEL = 0;           
        static const INT32 YES_NO = 1;
        LPCSTR BUTTON1_TEXT[MAX_TYPE] = { "OK", "YES" };
        LPCSTR BUTTON2_TEXT[MAX_TYPE] = { "CANCEL", "NO" };
        static const BYTE DIALOG_CLOSE_KEY = VK_RETURN;    
        COLOR_ARGB BUTTON_COLOR;
        COLOR_ARGB BUTTON_FONT_COLOR;
        MDParam() : WIDTH(400), HEIGHT(100), BORDER(5), MARGIN(5), FONT("Arial"), FONT_HEIGHT(18), FONT_COLOR(GraphColor::WHITE),
            BORDER_COLOR(GraphColor::LGRAY), BACK_COLOR(GraphColor::DBLUE), X(0), Y(0),
            BUTTON_WIDTH(FONT_HEIGHT * 4), BUTTON_HEIGHT(FONT_HEIGHT + 4), BUTTON_COLOR(GraphColor::GRAY), BUTTON_FONT_COLOR(GraphColor::WHITE) {}
        MDParam(const MDParam& mdp) {
            WIDTH = mdp.WIDTH, HEIGHT = mdp.HEIGHT, BORDER = mdp.BORDER, MARGIN = mdp.MARGIN, FONT = mdp.FONT, FONT_HEIGHT = mdp.FONT_HEIGHT,
                FONT_COLOR = mdp.FONT_COLOR, BORDER_COLOR = mdp.BORDER_COLOR, BACK_COLOR = mdp.BACK_COLOR, X = mdp.X, Y = mdp.Y,
                BUTTON_WIDTH = mdp.BUTTON_WIDTH, BUTTON_HEIGHT = mdp.BUTTON_HEIGHT, BUTTON_COLOR = mdp.BUTTON_COLOR, BUTTON_FONT_COLOR = mdp.BUTTON_FONT_COLOR;
        }
        MDParam& operator=(const MDParam& mdp) {
            WIDTH = mdp.WIDTH, HEIGHT = mdp.HEIGHT, BORDER = mdp.BORDER, MARGIN = mdp.MARGIN, FONT = mdp.FONT, FONT_HEIGHT = mdp.FONT_HEIGHT,
                FONT_COLOR = mdp.FONT_COLOR, BORDER_COLOR = mdp.BORDER_COLOR, BACK_COLOR = mdp.BACK_COLOR, X = mdp.X, Y = mdp.Y,
                BUTTON_WIDTH = mdp.BUTTON_WIDTH, BUTTON_HEIGHT = mdp.BUTTON_HEIGHT, BUTTON_COLOR = mdp.BUTTON_COLOR, BUTTON_FONT_COLOR = mdp.BUTTON_FONT_COLOR;
            return *this;
        }
	};



	class MessageDialog
	{
    protected:
        DirectGraphic* graphicDevice;
        Input* input;
        DirectText DXFont;
        MDParam MDP;
        string Text;
        INT32 game_width, game_height;
        RECT TextRect;
        RECT ButtonRect;
        RECT Button2Rect;
        VertexC MSDVert[RECT_VERT];     // vertex data contain position 
        LP_VERTEXBUFFER DialogVerts;    // dialog vertex buffer
        LP_VERTEXBUFFER BorderVerts;    // border vertex buffer
        LP_VERTEXBUFFER ButtonVerts;    // button vertex buffer
        LP_VERTEXBUFFER Button2Verts;    // button2 vertex buffer
        INT32 ButtonClicked;
        INT32 ButtonType;
        HWND hWnd;
        BOOL visible;
        BOOL initialized;
        FLOAT screenRatioX, screenRatioY;
    public:
        MessageDialog();
        virtual ~MessageDialog();
        BOOL initialize(DirectGraphic* pGraphicDevice, Input* input, HWND hWnd, const MDParam& MessageDialogParameters);
        VOID prepareVerteces();
        VOID draw();
        // getters
        INT32 getButtonClicked() const { return ButtonClicked; }
        BOOL getVisible() const { return visible; }
        // setters
        VOID setMDParameter(const MDParam& mdp) { MDP = mdp; }
        VOID setFontColor(COLOR_ARGB FC) { MDP.FONT_COLOR = FC; }
        VOID setBorderColor(COLOR_ARGB BC) { MDP.BORDER_COLOR = BC; }
        VOID setBackgroundColor(COLOR_ARGB BC) { MDP.BACK_COLOR = BC; }
        VOID setButtonColor(COLOR_ARGB BC) { MDP.BUTTON_COLOR = BC; }
        VOID setButtonFontColor(COLOR_ARGB BFC) { MDP.BUTTON_FONT_COLOR = BFC; }
        VOID setVisible(BOOL V) { visible = V; }
        // Set button type 0 = "OK/CANCEL", 1 = "YES/NO"
        VOID setButtonType(INT32 t) {
            if (t < MAX_TYPE)
                ButtonType = t;
        }
        //
        VOID print(const string& str);
        VOID update();
        VOID onLostDevice();
        VOID onResetDevice();
	};



}