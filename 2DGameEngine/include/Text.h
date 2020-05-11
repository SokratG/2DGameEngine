#pragma once
#include "Image.h"
#include "constants.h"
#include <vector>

using std::vector;

namespace GameEngine {


	enum Alignment { LEFT, RIGHT, CENTER, CENTER_MIDDLE, CENTER_BOTTOM, LEFT_BOTTOM, RIGHT_BOTTOM };

	using FontData = struct FontData
	{
		UINT left;
		UINT right;
	};
	// define const parameters of the fonts

	using FontParameters = struct FontParameters
	{
	public:
		INT32 FONT_BORDER;      // 1 pixel wide transparent border + visible cell border on right and bottom
		INT32 FONT_WIDTH;
		INT32 FONT_HEIGHT;     // font is 62 pixels high
		INT32 GRID_WIDTH;
		INT32 GRID_HEIGHT;
		INT32 COLUMNS;         // number of columns in font image
		INT32 ROWS;            // number of rows in font image
		INT32 FRAMES;           // how many frames of animation (1 = not animated)
		FLOAT ANIM_DELAY;  // delay between animation frames
		INT32 MAX_FONT_HEIGHT;
		const INT32 MIN_CHAR = 0x0020;    // minimum character code
		const INT32 MAX_CHAR = 0x00FF;    // maximum character code
		INT32 PROPORTIONAL_SPACING; // pixels between 1:1 scaled characters
		INT32 TAB_SIZE;
		const CHAR UNDERLINE = '_';
		CHAR SOLID;        // solid block used for fill character ASCII $7F
		INT32 BOLD_SIZE;        // pixel shift for bold display
		FontParameters() : GRID_WIDTH(0), GRID_HEIGHT(0), SOLID(0x7F) {}
		FontParameters(const FontParameters& fp);
		FontParameters& operator=(const FontParameters& fp);
		static FontParameters initFontParameters(INT32 FONT_BORDER, INT32 FONT_WIDTH, INT32 FONT_HEIGHT, INT32 COLUMNS, INT32 ROWS,
											INT32 FRAMES, FLOAT ANIM_DELAY, INT32 MAX_FONT_HEIGHT, INT32 PROPORTIONAL_SPACING, 
											 INT32 TAB_SIZE, INT32 BOLD_SIZE, CHAR SOLID = 0x7F);

	};

	class Text : public Image
	{
	private:
		TextureManager* fontTexture;
		LP_TEXTURE textureData;
		wstring File;				// name texture file with font
		DirectGraphic* graphicDevice;
		FontParameters fontParams;
		UINT width, height;			// width , height one character
		vector<vector<FontData>> fontData;
		COLOR_ARGB color;			// font color
		COLOR_ARGB backgroundColor; // backgound font color
		UINT fontHeight;			// font height in pixels
		UINT tabSize;				// tabulation size
		UINT proportionalSpacing;	// spacing in pixels
		BOOL proportional;
		BOOL underline;
		BOOL bold;
		Alignment textalign;
	public:
		Text();
		virtual ~Text();

		virtual BOOL initialize(DirectGraphic* pGraphicDevice, LPCWSTR filename, FontParameters& fontparam);
		FontParameters* getFontParameters() { return &fontParams; }
		vector<vector<FontData>>* getFontData() { return &fontData; }
		virtual VOID update(FLOAT dt) {}
		virtual VOID onLostDevice();
		virtual VOID onResetDevice();
		virtual VOID setXY(INT32 x, INT32 y);
		// printing method 
		virtual VOID print(const string& str);
		virtual VOID print(const string& str, INT32 x, INT32 y); // with pos
		virtual VOID print(const string& str, INT32 x, INT32 y, Alignment align); // with align
		virtual VOID doAlign(const std::string& str); // set sprite coord(x, y) with pass align
		virtual VOID drawChar(UCHAR ch);			// draw character with sprite color
		// getters
		virtual VOID getWidthHeight (const string& str, UINT& width, UINT& height);
		virtual COLOR_ARGB getFontColor() const { return color; }
		virtual COLOR_ARGB getBackColor() const { return backgroundColor; }
		virtual UINT getFontHeight() const { return fontHeight; }
		virtual BOOL getProportional() const { return proportional; }
		virtual UINT getProportionalSpacing() const { return proportionalSpacing; }
		virtual BOOL getUnderline() const { return underline; }
		virtual BOOL getBold() const { return bold; }
		virtual UINT getTabSize() const { return tabSize; }
		// setters
		virtual VOID setFontColor(COLOR_ARGB _c) { color = _c; }
		virtual VOID setBackColor(COLOR_ARGB bc) { backgroundColor = bc; }
		virtual VOID setFontHeight(UINT height);
		virtual VOID setProportional(BOOL p) { proportional = p; }
		virtual VOID setProportionalSpacing(UINT ps) { proportionalSpacing = ps; }
		virtual VOID setUnderline(BOOL ul) { underline = ul; }
		virtual VOID setBold(BOOL b) { bold = b; }
		virtual VOID setTabSize(UINT size){
			if (size == 0) return;
			tabSize = size;	
		}

	};









}