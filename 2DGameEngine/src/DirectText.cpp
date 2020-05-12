#include "DirectText.h"


namespace GameEngine {

	DirectText::DirectText()
	{
		DXFont = nullptr;
		graphicDevice = nullptr;
		angle = 0.f;
		TextColor = GraphColor::WHITE;
		ZeroMemory(&fontRect, sizeof(RECT));
	}

	DirectText::DirectText(INT32 GAME_WIDTH, INT32 GAME_HEIGHT)
	{
		DXFont = nullptr;
		graphicDevice = nullptr;
		angle = 0.f;
		TextColor = GraphColor::WHITE;
		fontRect.right = GAME_WIDTH;
		fontRect.bottom = GAME_HEIGHT;
		fontRect.top = 0;
		fontRect.left = 0;
	}

	DirectText::~DirectText()
	{
		SAFE_RELEASE(DXFont);
	}

	BOOL DirectText::initialize(DirectGraphic* pGraphicDevice, INT32 height, BOOL bold, BOOL italic, const string& fontname)
	{
		graphicDevice = pGraphicDevice;

		UINT weight = (bold) ? FW_BOLD : FW_NORMAL; // font weight
		if (FAILED(D3DXCreateFontA(graphicDevice->get3DDevice(), height, 0, weight, 1, italic,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontname.c_str(), &DXFont))) {
			return FALSE;
		}


		// create transformation matrix
		textMatrix.identity();

		return TRUE;
	}

	INT32 DirectText::print(const string& str, INT32 X, int32 Y)
	{
		if (!DXFont)
			return 0;

		fontRect.left = X;
		fontRect.top = Y;
		Vec2<FLOAT> RotateCenter((FLOAT)X, (FLOAT)Y);
		MATRIX4 mat = Mat4toDXMAT4(textMatrix);
		//setup matrix transformation
		D3DXMatrixTransformation2D(&mat, nullptr, 0, nullptr, &Vec2toDXVECTOR2(RotateCenter), angle, nullptr);

		// set matrix to sprite position
		graphicDevice->getSprite()->SetTransform(&mat);
		textMatrix = std::move(DXMAT4toMat4<FLOAT>(mat));
		return DXFont->DrawTextA(graphicDevice->getSprite(), str.c_str(), -1, &fontRect, DT_LEFT, TextColor);
	}

	INT32 DirectText::print(const string& str, RECT& drawrect, UINT format)
	{
		if (!DXFont)
			return 0;

		textMatrix.identity();

		graphicDevice->getSprite()->SetTransform(&Mat4toDXMAT4(textMatrix));
		return DXFont->DrawTextA(graphicDevice->getSprite(), str.c_str(), -1, &drawrect, format, TextColor);
	}
	


	VOID DirectText::onLostDevice()
	{
		if (!DXFont)
			return;
		DXFont->OnLostDevice();
	}
	VOID DirectText::onResetDevice()
	{
		if (!DXFont)
			return;
		DXFont->OnResetDevice();
	}



}