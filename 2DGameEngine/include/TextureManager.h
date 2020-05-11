#pragma once
#include "DirectGraphic.h"
#include <unordered_map>
#include <string>

using std::string;
using std::wstring;
//

namespace GameEngine {
	
	class TextureManager {
	private:
		UINT height; // height texture in pixels
		UINT width; // width texture 
		LP_TEXTURE pTexture; // pointer to texture
		wstring file;	// file contain texture
		BOOL initialized;		// check init class
		HRESULT result;
		DirectGraphic* graphicDevice;	// graphic device
		TextureManager();
	public:	
		virtual ~TextureManager();
		virtual BOOL initialize(DirectGraphic* pGDevice, LPCWSTR filename);
		LP_TEXTURE getTexture() const {
			return pTexture;
		}
		UINT getWidth() const {return width;}
		UINT getHeight() const {return height;}
		VOID setWidth(UINT width) noexcept {this->width = width;}
		VOID setHeight(UINT height) noexcept {this->height = height;}
		// event resotre resource
		virtual VOID onResetDevice();
		// event release resource 
		virtual VOID onLostDevice();
		static TextureManager* CreateTextureManager();
	};

};
