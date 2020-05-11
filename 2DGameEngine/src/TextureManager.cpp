#include "TextureManager.h"
#include <Windows.h>


namespace GameEngine {

	TextureManager::TextureManager() : graphicDevice(nullptr), initialized(FALSE), width(0U), height(0U), 
									pTexture(nullptr), result(E_FAIL)
	{
	}

	TextureManager::~TextureManager()
	{	
		SAFE_RELEASE(pTexture);
	}


	BOOL TextureManager::initialize(DirectGraphic* pGDevice, LPCWSTR filename) // load texture file from disk
	{
		try
		{
			graphicDevice = pGDevice;
			file = { filename };
			result = pGDevice->loadTexture(file.c_str(), TRANSCOLOR, width, height, pTexture);
			if (FAILED(result)) {
				SAFE_RELEASE(pTexture);
				return FALSE;
			}
		}
		catch (...)
		{
			return FALSE;
		}

		initialized = TRUE;
		return TRUE;
	}


	
	VOID TextureManager::onLostDevice() // call when graphic device lost
	{
		if (!initialized)
			return;
		SAFE_RELEASE(pTexture);
	}
	
	VOID TextureManager::onResetDevice()
	{
		if (!initialized)
			return;
		SAFE_RELEASE(pTexture);
	}


	TextureManager* TextureManager::CreateTextureManager()
	{
		TextureManager* txmngr = nullptr;
		try
		{
			txmngr = new TextureManager;
		}
		catch (const std::bad_alloc& alloc)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, alloc.what()));
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating window class"));
		}
		return txmngr;
	}

}