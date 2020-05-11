#pragma once
#include <windows.h>
#include <d3dx9.h>

using LP_TEXTURE = LPDIRECT3DTEXTURE9;  // directX texture structure

namespace GameEngine {



	struct SpriteData
	{
		INT32 width;
		INT32 height;
		FLOAT x;			// positon x, y
		FLOAT y;
		FLOAT scale;		// scaled this sprite
		FLOAT angle;		// rotation by angle sprite in radians
		RECT objrect;		// used to select an image from a larger texture
		LP_TEXTURE texture;	// pointer to texture
		BOOL flipHorizontal; // flip sprite to horizontally (mirror)
		BOOL flipVertical;	// flit to vertically
	};


};