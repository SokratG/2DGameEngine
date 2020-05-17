#pragma once
#include "Window.h"
#include "Sprite.h"

#ifdef D3D9

#pragma warning(push)
#pragma warning(disable : 26495)
#include <d3d9.h>
#include <D3D9Types.h>
#include <d3dx9.h>
#pragma warning(pop)

#define LP_3DDEVICE LPDIRECT3DDEVICE9
#define LP_3D LPDIRECT3D9 
#define COLOR_ARGB DWORD
#define VECTOR2 D3DXVECTOR2
#define VECTOR3 D3DXVECTOR3
#define MATRIX4 D3DXMATRIX
#define LP_VERTEXBUFFER LPDIRECT3DVERTEXBUFFER9
#define LP_DXFONT LPD3DXFONT
#define SETCOLOR_ARGB(A,R,G,B) ((COLOR_ARGB) ( ( ( A & 0xFF) << 24) | ( ( R & 0xFF) << 16 ) | ( ( G & 0xFF) << 8) | ( B & 0xFF) ) )
#define COLOR24BIT _D3DFORMAT::D3DFMT_X8R8G8B8	
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)	// (D3DFVF_XYZ | D3DFVF_TEX1)
#define D3D_RGB(R,G,B) D3DCOLOR_XRGB(R, G, B)

#ifndef D3_ARGB_DEFAULT
#define D3_ARGB(A,R,G,B) SETCOLOR_ARGB(A,R,G,B)
#else
#define D3_ARGB(A,R,G,B) D3DCOLOR_ARGB(A,R,G,B)
#endif /* D3_ARGB_DEFAULT */

using LP_SPRITE = LPD3DXSPRITE; // directX sprite structure
using LP_TEXTURE = LPDIRECT3DTEXTURE9;  // directX texture structure

namespace GraphColor {
	// constant some color
	// ARGB - represent color in range from 0 to 255(ALPHA - 0 transparency, 255 - opaque)
	// 
	const COLOR_ARGB ORANGE  =	D3DCOLOR_ARGB(255, 255, 165, 0);
	const COLOR_ARGB MAGENTA =	D3DCOLOR_ARGB(255, 255, 0, 255);
	const COLOR_ARGB RED     =	D3DCOLOR_ARGB(255, 255, 0, 0);
	const COLOR_ARGB CYAN    =	D3DCOLOR_ARGB(255, 0, 255, 255);
	const COLOR_ARGB LIME    =	D3DCOLOR_ARGB(255, 0, 255, 0);
	const COLOR_ARGB PURPLE  =	D3DCOLOR_ARGB(255, 128, 0, 128);
	const COLOR_ARGB MAROON  =	D3DCOLOR_ARGB(255, 128, 0, 0);
	const COLOR_ARGB BLUE    =	D3DCOLOR_ARGB(255, 0, 0, 255);
	const COLOR_ARGB OLIVE   =	D3DCOLOR_ARGB(255, 128, 128, 0);
	const COLOR_ARGB NAVY    =	D3DCOLOR_ARGB(255, 0, 0, 128);
	const COLOR_ARGB BLACK   =	D3DCOLOR_ARGB(255, 0, 0, 0);
	const COLOR_ARGB BROWN   =	D3DCOLOR_ARGB(255, 139, 69, 19);
	const COLOR_ARGB TEAL    =	D3DCOLOR_ARGB(255, 0, 128, 128);
	const COLOR_ARGB GREEN   =	D3DCOLOR_ARGB(255, 0, 128, 0);
	const COLOR_ARGB LTGRAY  =	D3DCOLOR_ARGB(255, 192, 192, 192);
	const COLOR_ARGB GRAY    =	D3DCOLOR_ARGB(255, 128, 128, 128);
	const COLOR_ARGB WHITE   =	D3DCOLOR_ARGB(255, 255, 255, 255);
	const COLOR_ARGB YELLOW  =	D3DCOLOR_ARGB(255, 255, 255, 0);
	const COLOR_ARGB LBLUE	 =	D3DCOLOR_ARGB(255, 230, 230, 255);
	const COLOR_ARGB LGRAY	 =  D3DCOLOR_ARGB(192, 192, 192, 192);
	const COLOR_ARGB DBLUE	=	D3DCOLOR_ARGB(255, 100, 100, 192);
	//
	const COLOR_ARGB FILTER  =	D3DCOLOR_ARGB(0, 0, 0, 0);	// specify drawing for color filter
	const COLOR_ARGB ALPHA25 =	D3DCOLOR_ARGB(64, 255, 255, 255); // AND with color to get alpha channel 25%
	const COLOR_ARGB ALPHA50 =	D3DCOLOR_ARGB(128, 255, 255, 255);// AND with color to get alpha channel 50%
	const COLOR_ARGB BACK_COLOR = NAVY;						// backgroung color of game

}


struct VertexC              // Vertex with Color
{
	FLOAT x, y, z;          // vertex location
	FLOAT rhw;              // reciprocal homogeneous W (set to 1)
	ULONG color;			// vertex color
};

namespace GameEngine {

	class DirectGraphic
	{
	private:
		// DirectX pointers
		LP_3D direct3d;
		LP_3DDEVICE device3d;
		D3DPRESENT_PARAMETERS d3dpp;
		D3DDISPLAYMODE pMode;
		LP_SPRITE sprite;
		//
		HRESULT result; // for return Windows code
		// HWND hWnd;
		Window* pWnd;
		BOOL FullScreen;
		INT32 Width;
		INT32 Height;
		COLOR_ARGB backgroundColor;	// D3DCOLOR
		// Initialize D3D presentation paramters
		VOID initD3Dpp();
		DirectGraphic() noexcept;
		BOOL isAdapterCompatible() noexcept;
	public:
		DirectGraphic(const DirectGraphic&) = delete;
		DirectGraphic& operator=(const DirectGraphic&) = delete;
		virtual ~DirectGraphic();
		VOID initialize(Window* hwnd, INT32 width, INT32 height, BOOL fullscreen = FALSE);
		VOID releaseAll();	// release all resource device
		HRESULT showBackBuffer();	// display the offscreen backbuffer to the screen(page flipping)
		HDC getGraphicDC() noexcept;
		VOID setBackColor(D3DCOLOR color) noexcept;
		HRESULT getDeviceState() noexcept;
		HRESULT reset() noexcept;
		HRESULT beginScene() noexcept;
		HRESULT endScene() noexcept;
		BOOL getFullScreen() const;
		LP_3D get3DDirect() noexcept;
		LP_3DDEVICE get3DDevice() noexcept;
		LP_SPRITE getSprite() noexcept;
		HRESULT loadTexture(LPCWSTR filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture); // load texture from resource file
		HRESULT loadTextureSystemMem(LPCWSTR filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture); // load the texture into system memory(system memory is lockable)
		HRESULT createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer);
		BOOL drawQuad(LP_VERTEXBUFFER vertexBuffer);
		VOID spriteBeginScene();	// sprite begin to draw in scene
		VOID spriteEndScene();		// sprite end to draw in scene
		VOID drawSprite(const SpriteData& spritedata, COLOR_ARGB color);
		// change display mode for window
		// created D3DPOOL_DEFAULT surfaces are freed. all surface recreated
		VOID changeDisplayMode(GraphColor::DISPLAY_MODE mode = GraphColor::TOGGLE); 
		INT32 getGraphicWidth() const;
		INT32 getGraphicHeight() const;
		static DirectGraphic* CreateDirectGraphic();	// fabric function member
	};

	FLOAT Vector2Length(const VECTOR2* _V);
	FLOAT Vector2Dot(const VECTOR2* _V1, const VECTOR2* _V2);
	VOID Vector2Normalize(VECTOR2* _V);
	// Transform vector v with matrix m.
	VECTOR2* Vector2Transform(VECTOR2* v, D3DXMATRIX* m);
};
#endif /* D3D9 */
