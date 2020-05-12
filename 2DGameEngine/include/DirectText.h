#pragma once

#include <string>
#include "DirectGraphic.h"
#include "GameMath.h"

using std::string;

template<typename _Type>
Vec2<_Type> DXVECTOR2toVec2(const VECTOR2& vec) {
	Vec2<_Type> _mvec;
	_mvec.x = vec.x;
	_mvec.y = vec.y;
	return _mvec;
}

template<typename _Type>
VECTOR2 Vec2toDXVECTOR2(const  Vec2<_Type>& vec) {
	VECTOR2 _mvec;
	_mvec.x = vec.x;
	_mvec.y = vec.y;
	return _mvec;
}

template<typename _Type>
Vec3<_Type> DXVECTOR23toVec3(const VECTOR3& vec) {
	Vec3<_Type> _mvec;
	_mvec.x = vec.x;
	_mvec.y = vec.y;
	_mvec.z = vec.z;
	return _mvec;
}

template<typename _Type>
VECTOR3 Vec2toDXVECTOR2(const  Vec3<_Type>& vec) {
	VECTOR3 _mvec;
	_mvec.x = vec.x;
	_mvec.y = vec.y;
	_mvec.z = vec.z;
	return _mvec;
}

template<typename _Type>
Mat4<_Type> DXMAT4toMat4(const MATRIX4& mat) {
	return Mat4<_Type>{
		mat._11, mat._12, mat._13, mat._14,
			mat._21, mat._22, mat._23, mat._24,
			mat._31, mat._32, mat._33, mat._34,
			mat._41, mat._42, mat._43, mat._44
	};
}

template<typename _Type>
MATRIX4 Mat4toDXMAT4(const Mat4 <_Type>& mat) {
	return MATRIX4(mat.m[0], mat.m[1], mat.m[2], mat.m[3],
		mat.m[4], mat.m[5], mat.m[6], mat.m[7],
		mat.m[8], mat.m[9], mat.m[10], mat.m[11],
		mat.m[12], mat.m[13], mat.m[14], mat.m[15]);
}


namespace GameEngine {

	class DirectText
	{
	private:
		DirectGraphic* graphicDevice;
		LP_DXFONT DXFont;
		RECT fontRect;		// text draw in rectangle 
		Mat4<FLOAT> textMatrix;
		FLOAT angle;
		COLOR_ARGB TextColor;
	public:
		DirectText();
		DirectText(INT32 GAME_WIDTH, INT32 GAME_HEIGHT);
		virtual ~DirectText();
		virtual BOOL initialize(DirectGraphic* pGraphicDevice, INT32 height, BOOL bold, BOOL italic, const string& fontname);
		virtual INT32 print(const string& str, INT32 X, int32 Y);
		virtual INT32 print(const string& str, RECT& drawrect, UINT format);
		virtual COLOR_ARGB getFontColor() const { return TextColor; }
		virtual FLOAT getAngleDeg() const { return RadianToDegree(angle); }
		virtual FLOAT getAngleRad() const { return angle; }
		virtual VOID setAngleDeg(FLOAT _angle) { angle = DegreeToRadian(_angle); }
		virtual VOID setAngleRad(FLOAT _angle) { angle = _angle; }
		virtual VOID setFontColor(COLOR_ARGB color) { TextColor = color; }
		virtual VOID setWidthHeight(INT32 GAME_WIDTH, INT32 GAME_HEIGHT){
			fontRect.right = GAME_WIDTH;
			fontRect.bottom = GAME_HEIGHT;
		}
		virtual VOID onLostDevice();
		virtual VOID onResetDevice();
	};



}