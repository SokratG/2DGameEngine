#include "DirectGraphic.h"
#include <ctime>


#ifdef D3D9
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")
#endif

#ifdef D3D9
namespace GameEngine {
	DirectGraphic::DirectGraphic() noexcept : d3dpp{ 0 }
	{
		this->direct3d = nullptr;
		this->device3d = nullptr;
		this->FullScreen = FALSE;
		this->Width = GAME_WIDTH;
		this->Height = GAME_HEIGHT;
		this->result = S_OK;
		this->pWnd = nullptr;
		this->sprite = nullptr;
		backgroundColor = GraphColor::BACK_COLOR;
	}

	DirectGraphic::~DirectGraphic()
	{
		releaseAll();
	}

	VOID DirectGraphic::releaseAll()
	{
		SAFE_RELEASE(sprite);
		SAFE_RELEASE(direct3d);
		SAFE_RELEASE(device3d);
	}

	VOID DirectGraphic::initialize(Window* _hWnd, INT32 _width, INT32 _height, BOOL _fullscreen)
	{
		this->pWnd = _hWnd;
		this->Width = _width;
		this->Height = _height;
		this->FullScreen = _fullscreen;

		// Init Direct3D
		direct3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (direct3d == nullptr)
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing Direct3D"));

		// Init D3D presentation parameters
		this->initD3Dpp();
		if (_fullscreen) {
			if (isAdapterCompatible()) // is the adapter compatible
				// Set the refresh rate with a compatible one
				d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
			else
				throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error the graphics device does not support the specifed resolution and format."));
		}

		// Determine if supports hardware texturing, lighting and vertex shaders
		D3DCAPS9 _caps;
		DWORD behavior;
		result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &_caps);

		// choose hardware or software graphics processing(vertex, shader)
		if ((_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
			_caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
			behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		else
			behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		result = direct3d->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			*pWnd->getHWND(),
			behavior,	// Reports Direct3D9 (Error) on system D3DCREATE_SOFTWARE_VERTEXPROCESSING
			// Does not work if graphics card lacks hardware vertex processing
			&d3dpp,
			&device3d);
		if (FAILED(result))
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating Direct3D device"));

		result = D3DXCreateSprite(device3d, &sprite);
		if (FAILED(result))
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating Direct3D sprite"));

		// Configure for alpha blend of primitives
		device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

#pragma warning(push)
#pragma warning(disable : 26812)	//off warning C26812 - about enum type unscope
	VOID DirectGraphic::initD3Dpp()
	{
		try
		{
			// fill zero to structure
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;

			d3dpp.BackBufferFormat = (FullScreen) ? COLOR24BIT : _D3DFORMAT::D3DFMT_UNKNOWN; // if fullscreen use 24-bit color else setting desktop

			d3dpp.BackBufferCount = 1;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.hDeviceWindow = *pWnd->getHWND();
			d3dpp.Windowed = (!FullScreen);
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error initializing D3D presentation parameters"));
		}
	}
#pragma warning(pop)

	HRESULT DirectGraphic::showBackBuffer() // for use flip page techic
	{
		result = E_FAIL; // default fail

		// display backbuffer to screen
		result = device3d->Present(nullptr, nullptr, nullptr, nullptr);
		return result;
	}

	DirectGraphic* DirectGraphic::CreateDirectGraphic()
	{
		DirectGraphic* drtx = nullptr;
		try
		{
			drtx = new DirectGraphic;
		}
		catch (const std::bad_alloc& alloc)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, alloc.what()));
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating direct graphic class"));
		}
		return drtx;
	}

	// Checks the adapter to see if it is compatible with the BackBuffer
	// height, width, and refresh rate specified in d3dpp. Fills in the pMode
	// structure with the format of the compatible mode, if found.
	BOOL DirectGraphic::isAdapterCompatible() noexcept
	{
		UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat);
		for (UINT i = 0; i < modes; ++i) {
			result = direct3d->EnumAdapterModes(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat, i, &this->pMode);
			if (pMode.Height == d3dpp.BackBufferHeight
				&& pMode.Width == d3dpp.BackBufferWidth
				&& pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
				return TRUE;
		}
		return FALSE;
	}

	HRESULT DirectGraphic::loadTexture(LPCWSTR filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture)
	{
		D3DXIMAGE_INFO info; // struct for read image file info
		result = E_FAIL;
		try
		{
			if (filename == nullptr) {
				texture = nullptr;
				return D3DERR_INVALIDCALL;
			}

			result = D3DXGetImageInfoFromFile(filename, &info);
			if (result != D3D_OK)
				return result;
			width = info.Width;
			height = info.Height;
			// create a new texture from loading file
			result = D3DXCreateTextureFromFileEx(
				device3d,
				filename,
				info.Width,
				info.Height,
				1,				// Mipmap levels(1 - no chains)
				0,				// Usage (0 - no usage)
				D3DFMT_UNKNOWN, // Sufrace forma(default)
				D3DPOOL_DEFAULT, // memory pool class for the texture
				D3DX_DEFAULT,   // image filter
				D3DX_DEFAULT,   // mipmap filter
				transcolor,		// color key for transparency
				&info,			// file info(from loaded file)
				nullptr,		// pointer to color palette
				&texture		// save destination texture
			);
		}
		catch (...)
		{
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error in loaded texture");
		}
		return result;
	}


	HRESULT DirectGraphic::loadTextureSystemMem(LPCWSTR filename, COLOR_ARGB transcolor, UINT& width, UINT& height, LP_TEXTURE& texture)
	{
		D3DXIMAGE_INFO info; // struct for read image file info
		result = E_FAIL;

		try
		{
			if (filename == nullptr) {
				texture = nullptr;
				return D3DERR_INVALIDCALL;
			}
			result = D3DXGetImageInfoFromFile(filename, &info);
			if (result != D3D_OK)
				return result;
			width = info.Width;
			height = info.Height;

			// create a new texture from loading file and allocate them in system memory
			result = D3DXCreateTextureFromFileEx(
				device3d,
				filename,
				info.Width,
				info.Height,
				1,				// Mipmap levels(1 - no chains)
				0,				// Usage (0 - no usage)
				D3DFMT_UNKNOWN, // Sufrace forma(default)
				D3DPOOL_SYSTEMMEM, // memory pool class for the texture
				D3DX_DEFAULT,   // image filter
				D3DX_DEFAULT,   // mipmap filter
				transcolor,		// color key for transparency
				&info,			// file info(from loaded file)
				nullptr,		// pointer to color palette
				&texture		// save destination texture
			);

		}
		catch (...)
		{
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error in loaded texture");
		}

		return result;
	}

	VOID DirectGraphic::drawSprite(const SpriteData& spritedata, COLOR_ARGB color) { 

		if(!spritedata.texture)
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error sprite texture draw");


		D3DXVECTOR2 spriteCenter(static_cast<FLOAT>(spritedata.width / 2 * spritedata.scale),	// find center sprite
								static_cast<FLOAT>(spritedata.height / 2 * spritedata.scale));

		D3DXVECTOR2 translate(spritedata.x, spritedata.y); // screen position of the sprite

		D3DXVECTOR2 scaling(spritedata.scale, spritedata.scale); // vector with scale data of the sprite
	
		if (spritedata.flipHorizontal) {
			scaling.x *= -1;	// negtive for scale to flip
			spriteCenter.x -= static_cast<FLOAT>(spritedata.width * spritedata.scale); // center of flipped image
			// flip occurs around left edge
			translate.x += static_cast<FLOAT>(spritedata.width * spritedata.scale); // translate right edge to put flipped image in same location as original.
		}
		if (spritedata.flipVertical) {
			scaling.y *= -1;	// neg y for scale flip
			spriteCenter.y -= static_cast<FLOAT>(spritedata.height * spritedata.scale);
			translate.y += static_cast<FLOAT>(spritedata.height * spritedata.scale);
		}
		
		// matrix to determine position the sprite in space
		D3DXMATRIX matrix;
		D3DXMatrixTransformation2D(&matrix,  // dest matrix
			nullptr,			// leep origin at left-top when scale
			0.f,				// no scale rotationg
			&scaling,			// value of scale
			&spriteCenter,		// rotate around center
			spritedata.angle,	// angle rotation
			&translate);		// x, y location

		sprite->SetTransform(&matrix); //apply matrix transformation for sprite

		// draw spite
		sprite->Draw(spritedata.texture, &spritedata.objrect, nullptr, nullptr, color);
	}


	HRESULT DirectGraphic::createVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER& vertexBuffer)
	{
		result = E_FAIL;
		// create vertex buffer
		result = device3d->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_DEFAULT, &vertexBuffer, nullptr);
		if (FAILED(result))
			return E_FAIL;
		// lock buffer before data can be transferred in
		PVOID ptr;
		result = vertexBuffer->Lock(0, size, (PVOID*)&ptr, 0);
		if (FAILED(result))
			return E_FAIL;
		CopyMemory(ptr, verts, size);
		vertexBuffer->Unlock();
		return result;
	}


	BOOL DirectGraphic::drawQuad(LP_VERTEXBUFFER vertexBuffer)
	{
		result = E_FAIL;
		if (!vertexBuffer)
			return FALSE;
		// enable alpha blend
		device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		// create stream source vertices
		device3d->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexC));
		// Set the current vertex stream declaration.
		device3d->SetFVF(D3DFVF_VERTEX);	//  position of a transformed vertex and diffuse color component.
		result = device3d->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		// alpha blend off
		device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		if (FAILED(result))
			return FALSE;
		return TRUE;
	}


	// get handle graphic device context(windows)
	HDC DirectGraphic::getGraphicDC() noexcept {
		return GetDC(*pWnd->getHWND());
	}

	// check full screen mode
	BOOL DirectGraphic::getFullScreen() const {
		return FullScreen;
	}

	// Check state of graphic device state on lost
	HRESULT DirectGraphic::getDeviceState() noexcept
	{
		result = E_FAIL;
		if (device3d == nullptr)
			return result;
		result = device3d->TestCooperativeLevel();
		return result;
	}

	HRESULT DirectGraphic::reset() noexcept
	{
		result = E_FAIL;
		initD3Dpp();
		sprite->OnLostDevice();
		result = device3d->Reset(&d3dpp);

		// configure alpha blend
		device3d->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		device3d->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		sprite->OnResetDevice();

		return result;
	}

	HRESULT DirectGraphic::beginScene() noexcept
	{
		result = E_FAIL;
		if (device3d == nullptr)
			return result;
		// clear backbuffer to backcolor
		device3d->Clear(0, NULL, D3DCLEAR_TARGET, backgroundColor, 1.0f, 0); // 5 - 0.f
		result = device3d->BeginScene();
		return result;
	}
	HRESULT DirectGraphic::endScene() noexcept
	{
		result = E_FAIL;
		if (device3d)
			result = device3d->EndScene();
		return result;
	}

	VOID DirectGraphic::spriteBeginScene() {
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
	}

	VOID DirectGraphic::spriteEndScene() {
		sprite->End();
	}

	LP_3D DirectGraphic::get3DDirect() noexcept
	{
		return direct3d;
	}

	LP_3DDEVICE DirectGraphic::get3DDevice() noexcept
	{
		return device3d;
	}

	LP_SPRITE DirectGraphic::getSprite() noexcept
	{
		return sprite;
	}

	VOID DirectGraphic::setBackColor(D3DCOLOR _color) noexcept
	{
		this->backgroundColor = _color;
	};

#pragma warning(push)
#pragma warning(disable : 26812)
	VOID DirectGraphic::changeDisplayMode(GraphColor::DISPLAY_MODE mode)
	{
		try
		{
			switch (mode)
			{
			case GraphColor::FULLSCREEN:
				if (FullScreen)
					return;
				FullScreen = TRUE;
				break;
			case GraphColor::WINDOW:
				if (!FullScreen)
					return;
				FullScreen = FALSE;
				break;
			case GraphColor::TOGGLE:
				FullScreen = !FullScreen;
				break;
			}
			reset();
			pWnd->changeDisplayMode(mode);
		}
		catch (...)
		{
			throw CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error change display mode in graphics");
		}
		

	}
#pragma warning(pop)

	inline FLOAT Vector2Length(const VECTOR2* _V) {
		return D3DXVec2Length(_V);
	}

	inline FLOAT Vector2Dot(const VECTOR2* _V1, const VECTOR2* _V2) {
		return D3DXVec2Dot(_V1, _V2);
	}

	inline VOID Vector2Normalize(VECTOR2* _V) {
		D3DXVec2Normalize(_V, _V);
	}

	inline VECTOR2* Vector2Transform(VECTOR2* v, D3DXMATRIX* m){
		return D3DXVec2TransformCoord(v, v, m); 
	}

};

#endif /* D3D9 */