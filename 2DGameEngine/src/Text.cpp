#include "Text.h"
#include <limits>

using std::numeric_limits;

namespace GameEngine {

	//=============================================================
	// FontParameters implementation
	//=============================================================
	FontParameters FontParameters::initFontParameters(INT32 FONT_BORDER, INT32 FONT_WIDTH, INT32 FONT_HEIGHT, INT32
											COLUMNS, INT32 ROWS, INT32 FRAMES, FLOAT ANIM_DELAY, INT32 MAX_FONT_HEIGHT, 
											INT32 PROPORTIONAL_SPACING, INT32 TAB_SIZE, INT32 BOLD_SIZE, CHAR SOLID)
	{
		FontParameters fp;
		fp.FONT_BORDER = FONT_BORDER;
		fp.FONT_WIDTH = FONT_WIDTH;
		fp.FONT_HEIGHT = FONT_HEIGHT;
		fp.GRID_WIDTH = FONT_WIDTH + FONT_BORDER;
		fp.GRID_HEIGHT = FONT_HEIGHT + FONT_BORDER;
		fp.COLUMNS = COLUMNS;
		fp.ROWS = ROWS;
		fp.FRAMES = FRAMES;
		fp.ANIM_DELAY = ANIM_DELAY;
		fp.MAX_FONT_HEIGHT = MAX_FONT_HEIGHT;
		fp.PROPORTIONAL_SPACING = PROPORTIONAL_SPACING;
		fp.SOLID = SOLID;
		fp.TAB_SIZE = TAB_SIZE;
		fp.BOLD_SIZE = BOLD_SIZE;
		return fp;
	}

	FontParameters::FontParameters(const FontParameters& fp)
	{
		FONT_BORDER = fp.FONT_BORDER;
		FONT_WIDTH = fp.FONT_WIDTH;
		FONT_HEIGHT = fp.FONT_HEIGHT;
		GRID_WIDTH = fp.FONT_WIDTH + fp.FONT_BORDER;
		GRID_HEIGHT = fp.FONT_HEIGHT + fp.FONT_BORDER ;
		COLUMNS = fp.COLUMNS;
		ROWS = fp.ROWS;
		FRAMES = fp.FRAMES;
		ANIM_DELAY = fp.ANIM_DELAY;
		MAX_FONT_HEIGHT = fp.MAX_FONT_HEIGHT;
		PROPORTIONAL_SPACING = fp.PROPORTIONAL_SPACING;
		SOLID = fp.SOLID;
		TAB_SIZE = fp.TAB_SIZE;
		BOLD_SIZE = fp.BOLD_SIZE;
	}

	FontParameters& FontParameters::operator=(const FontParameters& fp)
	{
		FONT_BORDER = fp.FONT_BORDER;
		FONT_WIDTH = fp.FONT_WIDTH;
		FONT_HEIGHT = fp.FONT_HEIGHT;
		GRID_WIDTH = fp.FONT_WIDTH + fp.FONT_BORDER;
		GRID_HEIGHT = fp.FONT_HEIGHT + fp.FONT_BORDER;
		COLUMNS = fp.COLUMNS;
		ROWS = fp.ROWS;
		FRAMES = fp.FRAMES;
		ANIM_DELAY = fp.ANIM_DELAY;
		MAX_FONT_HEIGHT = fp.MAX_FONT_HEIGHT;
		PROPORTIONAL_SPACING = fp.PROPORTIONAL_SPACING;
		SOLID = fp.SOLID;
		TAB_SIZE = fp.TAB_SIZE;
		BOLD_SIZE = fp.BOLD_SIZE;
		return *this;
	}




	//=============================================================
	// Text implementation
	//=============================================================
	Text::Text() : fontTexture(nullptr), textureData(nullptr), graphicDevice(nullptr), width(0), height(0),
					fontHeight(0), tabSize(0), proportionalSpacing(0)
	{
		color = GraphColor::WHITE;
		backgroundColor = TRANSCOLOR;
		textalign = Alignment::LEFT;
		spriteData.width = 0;
		spriteData.height = 0;
		spriteData.objrect.bottom = 1;
		spriteData.objrect.right = 1;
		proportional = FALSE;
		underline = FALSE;
		bold = FALSE;
	}

	Text::~Text() {
		if (fontTexture)
			SAFE_DELETE(fontTexture);
		fontTexture = nullptr;
		Image::~Image(); //
	}



	BOOL Text::initialize(DirectGraphic* pGraphicDevice, LPCWSTR filename, FontParameters& fontparam)
	{
		HRESULT result = E_FAIL;
		try
		{
			graphicDevice = pGraphicDevice;
			File = { filename };
			this->fontParams = fontparam;

			width = fontParams.GRID_WIDTH - 3;
			height = fontParams.GRID_HEIGHT - 3;
			fontHeight = fontParams.FONT_HEIGHT;
			proportionalSpacing = fontParams.PROPORTIONAL_SPACING;
			tabSize = fontParams.TAB_SIZE;

			// try optimize?
			// ---
			fontData.resize(fontParams.ROWS);

			for (size_t row = 0; row < fontParams.ROWS; ++row) {
				fontData[row].resize(fontParams.COLUMNS);
				for (size_t column = 0; column < fontParams.COLUMNS; ++column) {
					fontData[row][column].left = numeric_limits<INT32>::max();
					fontData[row][column].right = 0;
				}
			}
			// -- 
			UINT w, h;
			result = graphicDevice->loadTextureSystemMem(filename, TRANSCOLOR, w, h, textureData);
			if (FAILED(result)) {
				SAFE_RELEASE(textureData);
				return FALSE;
			}

			// textureData.width and textureData.height contain size of entire font texture
			// Each character has a 1 pixel wide border, There are ROWS * COLS characters

			// Lock the font texture, required to access the pixel data
			D3DLOCKED_RECT rt;
			result = textureData->LockRect(0, &rt, nullptr, D3DLOCK_READONLY);
			if (FAILED(result)) {
				SAFE_RELEASE(textureData);
				return FALSE;
			}
			for (size_t row = 0; row < fontParams.ROWS; ++row) {
				for (size_t column = 0; column < fontParams.COLUMNS; ++column) {
					// Process each character pixel by pixel    
					// for y = top pixel to bottom pixel;
					for (DWORD y = row * fontparam.GRID_HEIGHT + 1; y < (row + 1) * fontparam.GRID_HEIGHT - 1; ++y) {
						// Get a pointer to the start of this scanline in the texture
						DWORD* pBits = (DWORD*)((BYTE*)rt.pBits + y * rt.Pitch); // ?
						for (DWORD x = column * fontParams.GRID_WIDTH + 1; x < (column + 1) * fontParams.GRID_WIDTH - 1; ++x)
						{
							// Get this pixel
							DWORD dwPixel = pBits[x];
							
							if ((dwPixel & 0xFF000000) != 0x00) {				// check the alpha is not transparent
								if (x < fontData[row][column].left)     // check this pixel is more left
									fontData[row][column].left = x;		// save as left edge of character
								if (x > fontData[row][column].right)    // check this pixel is more right
									fontData[row][column].right = x;	// save right edge of character

							}
						}
					}
				}
			}

			textureData->UnlockRect(0);  // release this font texture, we just needed it to get font spacing

			SAFE_RELEASE(textureData);

			fontTexture = TextureManager::CreateTextureManager();

			// load the font image texture
			if (!fontTexture->initialize(pGraphicDevice, filename))
				return FALSE;
			// the font image
			if (!Image::initialize(pGraphicDevice, fontparam.FONT_WIDTH, fontparam.FONT_HEIGHT, 0, fontTexture))
				return FALSE;

		}
		catch (...)
		{
			return FALSE;
		}

		return TRUE;
	}


	VOID Text::onLostDevice()
	{
		try {
			if (!Image::initialized)
				return;
			fontTexture->onLostDevice();
		}
		catch (...) {}
	}

	VOID Text::onResetDevice()
	{
		try {
			if (!Image::initialized)
				return;
			fontTexture->onResetDevice();
		}
		catch (...) {}
	}


	VOID Text::setXY(INT32 X, INT32 Y) {
		if (X < 0 || Y < 0)
			return;
		spriteData.x = static_cast<FLOAT>(X);
		spriteData.y = static_cast<FLOAT>(Y);
	}

	VOID Text::getWidthHeight(const string& str, UINT& w, UINT& h)
	{
		if (!spriteData.texture)
			return;
		UCHAR ch = 0, chN = 0;
		width = fontParams.FONT_WIDTH;
		INT32 scaledWidth = static_cast<INT32>(width * spriteData.scale);
		INT32 strW = 0;
		h = 0;
		INT32 stringWidth = 0;


		for (size_t i = 0; i < str.length(); i++)
		{
			ch = str.at(i);
			// if displayable character
			if (ch > fontParams.MIN_CHAR && ch <= fontParams.MAX_CHAR)
			{
				chN = ch - fontParams.MIN_CHAR;    // make min_char index 0
				if (proportional)
				{
					spriteData.objrect.left = fontData[chN / fontParams.COLUMNS][chN % fontParams.COLUMNS].left;
					// +1 for DirectX sprite width
					spriteData.objrect.right = fontData[chN / fontParams.COLUMNS][chN % fontParams.COLUMNS].right + 1;
					width = spriteData.objrect.right - spriteData.objrect.left + proportionalSpacing;
					scaledWidth = static_cast<int>(width * spriteData.scale);
				}
				else    // fixed pitch
				{
					width = fontParams.FONT_WIDTH;
					spriteData.objrect.left = chN % fontParams.COLUMNS * fontParams.GRID_WIDTH + 1;
					spriteData.objrect.right = spriteData.objrect.left + fontParams.FONT_WIDTH;
				}
				stringWidth += scaledWidth;
			}
			else    // else, non displayable character
			{
				switch (ch)
				{
				case ' ':   
					if (proportional)
					{
						width = (fontParams.FONT_WIDTH) / 2;
						scaledWidth = static_cast<INT32>(width * spriteData.scale);
					}
					stringWidth += scaledWidth;
					break;
				case '\n': 
					if (strW == 0)
						strW = stringWidth;
					stringWidth = 0;
					h += static_cast<INT32>(height * spriteData.scale);
					break;
				case '\r':  
					if (strW == 0)
						strW = stringWidth;
					stringWidth = 0;
					break;
				case '\t':
				{
					width = fontParams.FONT_WIDTH;
					scaledWidth = static_cast<INT32>(width * spriteData.scale);
					INT32 tabX = static_cast<INT32>(spriteData.x) / (scaledWidth * tabSize);
					tabX = (tabX + 1) * scaledWidth * tabSize;
					INT32 tabW = tabX - static_cast<INT32>(spriteData.x);
					while (tabW > 0)
					{
						if (tabW >= scaledWidth)
							stringWidth += scaledWidth;
						else
						{
							// fractional part of character to align with tab stop
							width = tabW;
							stringWidth += tabW;
						}
						tabW -= scaledWidth;
					}
				}
				break;
				case '\b':      
					stringWidth -= scaledWidth;
					if (stringWidth < 0)
						stringWidth = 0;
					break;
				case 0x01:      // special
					stringWidth += scaledWidth;
					break;
				}
			}
		}
		if (strW == 0)
			strW = stringWidth;
		w = strW;
	}




	VOID Text::setFontHeight(UINT height)
	{
		if (height == 0 || height > fontParams.MAX_FONT_HEIGHT)  // check invalid size
			return;
		fontHeight = height;
		spriteData.scale = static_cast<FLOAT>(height) / fontParams.FONT_HEIGHT;
	}

	VOID Text::print(const string& str)
	{
		print(str, static_cast<INT32>(spriteData.x), static_cast<INT32>(spriteData.y));
	}

	VOID Text::print(const string& str, INT32 x, INT32 y, Alignment align)
	{
		textalign = align;
		print(str, x, y);
		textalign = Alignment::LEFT;
	}

	VOID Text::print(const string& str, INT32 x, INT32 y)
	{
		UCHAR ch = 0, chN = 0;
		string strtemp;
		width = fontParams.FONT_WIDTH;
		INT32 scaledWidth = static_cast<INT32>(fontParams.FONT_WIDTH * spriteData.scale);
		FLOAT _Y = 0.f;
		INT32 tabX = 0, tabW = 0;
		spriteData.x = static_cast<FLOAT>(x);
		spriteData.y = static_cast<FLOAT>(y);
		doAlign(str);


		for (size_t i = 0; i < str.size(); ++i) {
			ch = str.at(i);

			if (ch > fontParams.MIN_CHAR && ch <= fontParams.MAX_CHAR) { // check character in range ASCII valid char
				chN = ch - fontParams.MIN_CHAR;
				spriteData.objrect.top = chN / fontParams.COLUMNS * fontParams.GRID_HEIGHT + 1;
				spriteData.objrect.bottom = spriteData.objrect.top + fontParams.FONT_HEIGHT;
				if (proportional) {
					spriteData.objrect.left = fontData[chN / fontParams.COLUMNS][chN % fontParams.COLUMNS].left;
					spriteData.objrect.right = fontData[chN / fontParams.COLUMNS][chN % fontParams.COLUMNS].right + 1; //offset by 1
					width = spriteData.objrect.right - spriteData.objrect.left;
					// if full width character do not add spacing
					if (width >= fontParams.FONT_WIDTH) {
						width = fontParams.FONT_WIDTH;
						spriteData.objrect.left = chN % fontParams.COLUMNS * fontParams.GRID_WIDTH + 1;
						spriteData.objrect.right = spriteData.objrect.left + fontParams.FONT_WIDTH;
					}
					else
						width += proportionalSpacing;
					scaledWidth = static_cast<INT32>(width * spriteData.scale);
					drawChar(ch);
				}
				else {
					width = fontParams.FONT_WIDTH;
					spriteData.objrect.left = chN % fontParams.COLUMNS * fontParams.GRID_WIDTH + 1;
					spriteData.objrect.right = spriteData.objrect.left + fontParams.FONT_WIDTH;
					drawChar(ch);
				}
				spriteData.x += scaledWidth;
			}
			else { // if char in non ASCII valid char range
				switch (ch)
				{
				case ' ':
					if (proportional) {
						width = fontParams.FONT_WIDTH / 2;
						scaledWidth = static_cast<INT32>(width * spriteData.scale);
					}
					drawChar(' ');
					spriteData.x += scaledWidth;
					break;
				 case '\n':                            // newline
					 spriteData.x = (float)x;
					 spriteData.y += static_cast<int>(height * spriteData.scale);
					 _Y = spriteData.y;
					 strtemp = str.substr(i, str.length());
					 doAlign(strtemp);
					 spriteData.y = _Y;
					 break;
				 case '\r':                            // return to starting x position
					 spriteData.x = (float)x;
					 strtemp = str.substr(i, str.length());
					 doAlign(strtemp);
					 break;
				 case '\t':                            // tab
					 width = fontParams.FONT_WIDTH;
					 scaledWidth = static_cast<int>(width * spriteData.scale);
					 tabX = static_cast<int>(spriteData.x) / (scaledWidth * tabSize);
					 tabX = (tabX + 1) * scaledWidth * tabSize;
					 tabW = tabX - static_cast<int>(spriteData.x);
					 while (tabW > 0)
					 {
						 if (tabW >= scaledWidth)
						 {
							 drawChar(' ');
							 spriteData.x += scaledWidth;
						 }
						 else
						 {
							 width = tabW;        // fractional part of character to align with tab stop
							 drawChar(' ');
							 spriteData.x += tabW;
						 }
						 tabW -= scaledWidth;
					 }
					 break;
				 case '\b':                            // backspace
					 spriteData.x -= scaledWidth;
					 if (spriteData.x < 0)
						 spriteData.x = 0;
					 break;
				 case '\v':                            // vertical tab
					 spriteData.y += static_cast<int>(height * spriteData.scale);
					 break;
				 case 0x01:                            // font signature character ?
					 spriteData.objrect.top = 1;
					 spriteData.objrect.bottom = 1 + fontParams.FONT_HEIGHT;
					 spriteData.objrect.left = 1;
					 spriteData.objrect.right = 1 + fontParams.FONT_WIDTH;
					 draw(spriteData);
					 spriteData.x += scaledWidth;
					 break;
				}
			}
		}
	
	}


	VOID Text::doAlign(const string& str)
	{
		if (!spriteData.texture)
			return;

		UINT w, h;
		switch (textalign)
		{
		case GameEngine::RIGHT:
			getWidthHeight(str, w, h);
			spriteData.x -= w;
			break;
		case GameEngine::CENTER:
			getWidthHeight(str, w, h);
			spriteData.x -= w / 2;
			break;
			break;
		case GameEngine::CENTER_MIDDLE:
			getWidthHeight(str, w, h);
			spriteData.x -= w / 2;
			spriteData.y -= h / 2;
			break;
		case GameEngine::CENTER_BOTTOM:
			getWidthHeight(str, w, h);
			spriteData.x -= w / 2;
			spriteData.y -= h;
			break;
		case GameEngine::LEFT_BOTTOM:
			getWidthHeight(str, w, h);
			spriteData.y -= h;
			break;
		case GameEngine::RIGHT_BOTTOM:
			getWidthHeight(str, w, h);
			spriteData.x -= w;
			spriteData.y -= h;
			break;
		}
	}

	VOID Text::drawChar(UCHAR ch)
	{
		SpriteData temp = spriteData; // 

		if (backgroundColor != TRANSCOLOR) {
			spriteData.objrect.top = (fontParams.SOLID - fontParams.MIN_CHAR) / fontParams.COLUMNS * fontParams.GRID_HEIGHT + 1;
			spriteData.objrect.bottom = spriteData.objrect.top + fontParams.GRID_HEIGHT - 2;
			spriteData.objrect.left = (fontParams.SOLID - fontParams.MIN_CHAR) % fontParams.COLUMNS * fontParams.GRID_WIDTH + 1;
			spriteData.objrect.right = spriteData.objrect.left + width;
			draw(backgroundColor);				// draw backColor
			spriteData.objrect = temp.objrect;     // restore character rect
		}

		if (underline){
			spriteData.objrect.top = (fontParams.UNDERLINE - fontParams.MIN_CHAR) / fontParams.COLUMNS * fontParams.GRID_HEIGHT + 1;
			spriteData.objrect.bottom = spriteData.objrect.top + fontParams.GRID_HEIGHT - 2;
			spriteData.objrect.left = (fontParams.UNDERLINE - fontParams.MIN_CHAR) % fontParams.COLUMNS * fontParams.GRID_WIDTH + 1;
			spriteData.objrect.right = spriteData.objrect.left + width;
			draw(color);
			spriteData.objrect = temp.objrect;     // restore character rect
		}

		// draw char to display
		if (ch > fontParams.MIN_CHAR && ch <= fontParams.MAX_CHAR) // if displayable character
		{
			draw(spriteData, color);
			if (bold)   // bold is done by displaying the character twice with offset x
			{
				spriteData.x += fontParams.BOLD_SIZE * spriteData.scale;
				draw(spriteData, color);
				spriteData.x = temp.x;
			}
		}
	}
}