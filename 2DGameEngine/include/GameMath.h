#pragma once
#include <cassert>
#include <limits>
#include <stdexcept>
#include <ostream>
#include <algorithm>
#include <iterator>
#ifdef STD_INT
#include <stdint.h>
#endif

// TODO redo implementation atan && atan2 !

using std::abs;
using std::endl;
using std::fabs;
using std::begin;
using std::end;
// alias data type
using real32 = float;
using real64 = double;
using longreal64 = long double;
using int32 = int;
using int64 = long long;
using uint32 = unsigned int;
using uint64 = unsigned long long;

constexpr real32 _PI = 3.141592653589793238462643383279502f;
constexpr real32 _PI_2 = 6.283185307179586476925286766559005f;
constexpr real32 _PI_HALF = _PI / 2;


#define EPSILON64 std::numeric_limits<real64>::epsilon()
#define EPSILON32 std::numeric_limits<real32>::epsilon()


// ------ literal section ------
// degree to radian
constexpr longreal64 operator""_deg(longreal64 aDeg)
{
	return aDeg * _PI / 180.f;
}

// radian to degree
constexpr longreal64 operator""_rad(longreal64 aRad)
{
	return aRad * 180.f / _PI;
}

real32 __fastcall DegreeToRadian(real32 _angle) noexcept;
real32 __fastcall RadianToDegree(real32 _rad) noexcept;
real64 __fastcall DegreeToRadian(real64 _angle) noexcept;
real64 __fastcall RadianToDegree(real64 _rad) noexcept;

// wrap [rad] angle to [-PI..PI)
real64 __fastcall WrapPosNegPI(real64 fAng) noexcept;
// wrap [rad] angle to [0.._PI_2)
real64 __fastcall WrapTwoPI(real64 fAng) noexcept;
// wrap [deg] angle to [-180..180)
real64 __fastcall WrapPosNeg180(real64 fAng) noexcept;
// wrap [deg] angle to [0..360)
real64 __fastcall Wrap360(real64 fAng) noexcept;
// wrap for real32
real32 __fastcall WrapPosNegPI(real32 fAng) noexcept;
real32 __fastcall WrapTwoPI(real32 fAng) noexcept;
real32 __fastcall WrapPosNeg180(real32 fAng) noexcept;
real32 __fastcall Wrap360(real32 fAng) noexcept;

// Newton method calcus square root for constant time
uint32 __fastcall sqrt_f(uint32 s) noexcept;
real32 __fastcall sqrt_f(real32 s) noexcept;
real64 __fastcall sqrt_f(real64 s) noexcept;

// Factorial runtime
uint64 __fastcall Factorial(uint64 _val) noexcept;

// Greatest common divisor
uint64 __fastcall gcd_fast(uint64 a, uint64 b) noexcept;

// Least common multiple
uint64 __fastcall lcm_fast(uint64 a, uint64 b) noexcept;

// sin calculate Taylor series
real32 __fastcall sin_fangle(real32 angle) noexcept;
real64 __fastcall sin_fangle(real64 angle) noexcept;
real32 __fastcall sin_rad(real32 rad) noexcept;
real64 __fastcall sin_rad(real64 rad) noexcept;
// cos calculate Taylor series
real32 __fastcall cos_fangle(real32 angle) noexcept;
real64 __fastcall cos_fangle(real64 angle) noexcept;
real32 __fastcall cos_rad(real32 rad) noexcept;
real64 __fastcall cos_rad(real64 rad) noexcept;


// atan calculate Taylor series
#ifdef OFF
real32 __fastcall atan_fast(real32 rad) noexcept;
real64 __fastcall atan_fast(real64 rad) noexcept;
real32 __fastcall atan2_fast(real32 x, real32 y) noexcept;
real64 __fastcall atan2_fast(real64 x, real64 y) noexcept;
#endif

// Factorial compile time
template<uint32 _number>
struct factorial {
	static constexpr int64 value = factorial<_number - 1>::value * _number;
};

template<>
struct factorial<0U> {
	static constexpr int64 value = 1;
};

// Greatest common divisor compile time
template<uint32 _number1, uint32 _number2, uint32 _module>
struct gcd_help
{
	static constexpr uint32 value = gcd_help< _number2, _module, (_number2 % _module)>::value;
};

template<uint32 _number1, uint32 _number2>
struct gcd_help< _number1, _number2, 0>
{
	static constexpr uint32 value = _number2;
};


template<uint32 _number1, uint32 _number2>
struct gcd_compile
{
	static constexpr uint32 value = gcd_help<_number1, _number2, (_number1 % _number2)>::value;
};

// Least common multiple compile time
template<uint32 _number1, uint32 _number2>
struct lcm_compile
{
	static constexpr uint32 value = (_number1 * _number2) / gcd_compile<_number1, _number2>::value;
};

// linear interpolation calc
template<typename Type>
constexpr Type __fastcall LinearInterp(const Type begin, const Type end, const Type delta) noexcept
{
	return (begin + delta * (end - begin));
}

template<typename Type>
constexpr Type __fastcall LinearInterpRate(const Type begin, const Type end, const Type time, const Type max_time) noexcept
{
	return (begin + (end - begin) * (time / max_time));
}


template<typename T>
T mod(T x, T y)
{
    static_assert(!std::numeric_limits<T>::is_exact, "_mod: floating-point type expected");

    if (0. == y) return x;
    real64 _mod = x - y * static_cast<int64>(x / y);
    if (y > 0){
        if (_mod >= y) return 0;  
        if (_mod < 0){
            if (y + _mod == y)
                return 0; 
            else
                return y + _mod;
        }
    }
    else {
        if (_mod <= y) return 0;   
        if (_mod > 0){
            if (y + _mod == y)
                return 0;
            else
                return y + _mod;
        }
    }
    return _mod;
}

// compare float-pointing value
constexpr bool __fastcall cmpr64(const real64 a, const real64 b) noexcept
{
	const auto diff = a - b;
	return (diff < EPSILON64) && (-diff < EPSILON64);
}

constexpr bool __fastcall cmpr32(const real32 a, const real32 b) noexcept
{
	const auto diff = a - b;
	return (diff < EPSILON32) && (-diff < EPSILON32);
}


// in range value
template<typename Type>
constexpr bool __fastcall inRange(const Type val, const Type min, const Type max) noexcept
{
	return ((val >= min) && (val <= max));
}

// clamp value
template<typename Type>
constexpr Type __fastcall clamp(const Type val, const Type min, const Type max) noexcept
{
	const Type temp = val < min ? min : val;
	return temp > max ? max : temp;
};

// normilize function 
template<typename _Type = int32>
constexpr real32 NORM_X(_Type min, _Type max, _Type value) {
	if (value <= min)
		value = min;
	if (value >= max)
		value = max;
	_Type denom = max - min;
	if (denom == 0)
		throw std::overflow_error("Divide by zero! Denom in NORM_X");
	return static_cast<real32>(value - min) / denom;
}

#pragma warning(push)
#pragma warning(disable : 4244)
// scaling function, where (0.0 <= value <= 1.0)
template<typename _Type = int32, typename _OutType>
constexpr _OutType SCALE_X(_Type min, _Type max, real32 value) {
	if (value <= 0.)
		value = 0.;
	if (value >= 1.)
		value = 1.;
	return value * static_cast<_OutType>(max - min) + min;
}
#pragma warning(pop)



// --------------------------------------------------
// cpu math 
#ifdef CPU_X64_FUNC
extern "C" float sqrt__cpu(float x);
extern "C" float sin__cpu(float x);
extern "C" float cos__cpu(float x);
extern "C" float tan__cpu(float x);
extern "C" float atan__cpu(float x, const float y = _PI);
extern "C" float atan2__cpu(float x, float y);
extern "C" float log2__cpu(float x);
extern "C" float log10__cpu(float x);
extern "C" float ln__cpu(float x);
#endif

// --------------------------------------------------
// vector and matrix 
template<typename _Type>
class Mat4;


template<typename _Type>
class Vec2
{
public:
	_Type x;
	_Type y;
	_Type w; // for matrix transformation
public:
	Vec2() : x(0), y(0), w(1) {}
	Vec2(_Type _x, _Type _y, _Type _w = 1) : x(_x), y(_y), w(_w) {}
	Vec2(const Vec2& _v) noexcept { x = _v.x; y = _v.y; w = _v.w; }
	Vec2& getVec() noexcept { return *this; }
	Vec2& identity() { x = 1; y = 1; return *this; }
	Vec2& zero() { x = 0; y = 0; return *this;}
	Vec2& neg() { x = -x; y = -y; return *this;}
	Vec2& operator=(const Vec2& _v) {
		x = _v.x; y = _v.y; w = _v.w; return *this;
	}
	Vec2 operator+(const Vec2& _v)  {
		return Vec2(x + _v.x, y + _v.y); 
	}
	Vec2 operator+(_Type _val)  {
		return Vec2(x + _val, y + _val);
	}
	Vec2& operator+=(const Vec2& _v)  {
		x += _v.x; y += _v.y; return *this;
	}
	Vec2 operator-(const Vec2& _v)  {
		return Vec2(x - _v.x, y - _v.y);
	}
	Vec2 operator-(_Type _val)  {
		return Vec2(x - _val, y - _val);
	}
	Vec2& operator-=(const Vec2& _v) {
		x -= _v.x; y -= _v.y; return *this;
	}
	Vec2 operator*(const Vec2& _v)  {
		return Vec2(x * _v.x, y * _v.y);
	}
	Vec2 operator*(_Type _val)  {
		return Vec2(x * _val, y * _val);
	}
	Vec2& operator*=(const Vec2& _v)  {
		x *= _v.x; y *= _v.y; return *this;
	}
	Vec2 operator/(const Vec2& _v) {
		assert(_v.x != 0 && _v.y != 0); 
		return Vec2(x / _v.x, y / _v.y);
	}
	Vec2 operator/(_Type _val)  {
		assert(_val != 0);
		return Vec2(x / _val, y / _val);
	}
	Vec2& operator/=(const Vec2& _v)  {
		assert(_v.x != 0 && _v.y != 0);
		x /= _v.x; y /= _v.y; return *this;
	}
	bool operator==(const Vec2& _v) const {
		return (x == _v.x && y == _v.y);
	}
	bool operator!=(const Vec2& _v) const {
		return (x != _v.x || y != _v.y);
	}
	bool operator>(const Vec2& _v) const {
		return (x > _v.x || x == _v.x && y > _v.y);
	}
	bool operator<(const Vec2& _v) const {
		return (x < _v.x || x == _v.x && y < _v.y);
	}
	_Type length() const{ return sqrt_f((x * x) + (y * y)); }
	_Type dot(const Vec2& _v) { return (x * _v.x + y * _v.y); }
	_Type distance(const Vec2& _v) const {
		return sqrt_f((x - _v.x) * (x - _v.x) + (y - _v.y) * (y - _v.y));
	}
	_Type angleVec(const Vec2& _v) const {
		_Type res =  (RadianToDegree(atan2(_v.y - y, _v.x - x)));
		return (res < 0) ? res + 360. : res;
	}
	Vec2 normalize() {
		return (*this) * (1 / this->length());
	}
	Vec2& rotate(real32 angle) {
		real32 rad = DegreeToRadian(angle);
		x = x * cos_rad(rad) - y * sin_rad(rad);
		y = x * sin_rad(rad) + y * cos_rad(rad);
		return *this;
	}
	Vec2& move(const Vec2& _v, real32 dist) {
		assert(dist != 0);
		_Type dx = _v.x - x; _Type dy = _v.y - y;
		_Type res = (dx * dx) + (dy * dy);

		if (res == 0 || ((dist >= 0) && (res <= dist * dist)))
			return *this;

		_Type _d = sqrt_f(res);

		x += dx / (_d * dist);
		y += dy / (_d * dist);

		return *this;
	}
	Vec2 Project(Vec2& _v) {
		Vec2 res = _v.normalize() * dot(_v.normalize());
		return res;
	}
	Vec2 Reject(Vec2& _v)  {
		Vec2 res = *this - Project(_v);
		return res;
	}
	Vec2 Reflect(Vec2& _v, real32 coeff = 2)  {		// correction factor
		Vec2 res = *this - (Project(_v) * coeff);  //*this - (Reject(_v) * 2);
		return res;
	}
public:
	static Vec2 LinearInterp(const Vec2& _v1, const Vec2& _v2, real32 delta); // calculate linear interpolation 
	static Vec2 rotateX(real32 angle) noexcept {
		angle = WrapPosNegPI(angle);

		real32 _sin = sin_rad(angle);
		real32 _cos = cos_rad(angle);

		return Vec2(_cos, _sin);
	}
	static Vec2 rotateY(real32 angle) noexcept {
		angle = WrapPosNegPI(angle);

		real32 _cos = cos_rad(angle);
		real32 _sin = sin_rad(angle);

		return Vec2(-_sin, _cos);
	}
};

template<typename _Type>
Vec2<_Type> Vec2<_Type>::LinearInterp(const Vec2<_Type>& _v1, const Vec2<_Type>& _v2, real32 delta) {
	_Type x = _v1.x + (_v2.x - _v1.x) * delta; 
	_Type y = _v1.y + (_v2.y - _v1.y) * delta;
	return Vec2<_Type>(x, y);
}

template<>
bool Vec2<real32>::operator==(const Vec2& _v) const {
	return (cmpr32(x, _v.x) && cmpr32(y,_v.y));
}
template<>
bool Vec2<real64>::operator==(const Vec2& _v) const {
	return (cmpr64(x, _v.x) && cmpr64(y, _v.y));
}

template<>
bool Vec2<real32>::operator!=(const Vec2& _v) const {
	return (!cmpr32(x, _v.x) || !cmpr32(y, _v.y));
}
template<>
bool Vec2<real64>::operator!=(const Vec2& _v) const {
	return (!cmpr64(x, _v.x) || !cmpr64(y, _v.y));
}

template<>
bool Vec2<real32>::operator>(const Vec2& _v) const {
	return (x > _v.x || cmpr32(x,_v.x) && y > _v.y);
}

template<>
bool Vec2<real64>::operator>(const Vec2& _v) const {
	return (x > _v.x || cmpr64(x, _v.x) && y > _v.y);
}

template<>
bool Vec2<real32>::operator<(const Vec2& _v) const {
	return (x < _v.x || cmpr32(x, _v.x) && y < _v.y);
}

template<>
bool Vec2<real64>::operator<(const Vec2& _v) const {
	return (x < _v.x || cmpr64(x, _v.x) && y < _v.y);
}


template<typename _Type>
class Vec3
{
public:
	_Type x;
	_Type y;
	_Type z;
	_Type w; // for matrix transformation
public:
	Vec3(_Type _x, _Type _y, _Type _z, _Type _w = 1) : x(_x), y(_y), z(_z), w(_w) {}
	Vec3(const Vec3& _v) { x = _v.x; y = _v.y; z = _v.z; w = _v.w; }
	Vec3& getVec() noexcept { return *this; }
	Vec3& identity() { x = 1; y = 1; z = 1; return *this; }
	Vec3& zero() { x = 0; y = 0; z = 0; return *this; }
	Vec3& neg() { x = -x; y = -y; z = -z; return *this; }
	Vec3& operator=(const Vec3& _v) {
		x = _v.x; y = _v.y; z = _v.z; w = _v.w; return *this;
	}
	Vec3 operator+(const Vec3& _v) noexcept {
		return Vec3(x + _v.x, y + _v.y, z + _v.z);
	}
	Vec3 operator+(_Type _val) noexcept {
		return Vec3(x + _val, y + _val, z + _val);
	}
	Vec3& operator+=(const Vec3& _v) noexcept {
		x += _v.x; y += _v.y; z += _v.z; return *this;
	}
	Vec3 operator-(const Vec3& _v) noexcept {
		return Vec3(x - _v.x, y - _v.y, z - _v.z);
	}
	Vec3 operator-(_Type _val) noexcept {
		return Vec3(x - _val, y - _val, z - _val);
	}
	Vec3& operator-=(const Vec3& _v) noexcept {
		x -= _v.x; y -= _v.y; z -= _v.z; return *this;
	}
	Vec3 operator*(const Vec3& _v) noexcept {
		return Vec3(x * _v.x, y * _v.y, z * _v.z);
	}
	Vec3 operator*(_Type _val) noexcept {
		return Vec3(x * _val, y * _val, z * _val);
	}
	Vec3& operator*=(const Vec3& _v) noexcept {
		x *= _v.x; y *= _v.y; z *= _v.z; return *this;
	}
	Vec3 operator/(const Vec3& _v) {
		assert(_v.x != 0 && _v.y != 0 && _v.z != 0);
		return Vec3(x / _v.x, y / _v.y, z / _v.z);
	}
	Vec3 operator/(_Type _val) {
		assert(_val != 0);
		return Vec3(x / _val, y / _val, z / _val);
	}
	Vec3& operator/=(const Vec3& _v) {
		assert(_v.x != 0 && _v.y != 0 && _v.z != 0);
		x /= _v.x; y /= _v.y; z /= _v.z; return *this;
	}
	bool operator==(const Vec3& _v) const {
		return (x == _v.x && y == _v.y && z == _v.z);
	}
	bool operator!=(const Vec3& _v) const {
		return (x != _v.x || y != _v.y || z != _v.z);
	}
	_Type dot(const Vec3& _v) const {
		return (x * _v.x + y * _v.y + z * _v.z);
	}
	_Type distance(const Vec3 _v) const {
		return sqrt_f((x - _v.x) * (x - _v.x) + (y - _v.y) * (y - _v.y) + (z - _v.z) * (z - _v.z));
	}
	_Type length() const {
		return sqrt_f((x * x) + (y * y) + (z * z));
	}
	Vec3 cross(const Vec3& _v) const {
		return Vec3((y*_v.z - z*_v.y), (z*_v.x - x*_v.z), (x*_v.y - y*_v.x));
	}
	Vec3 getPerpendicular() const {
		Vec3 cAxis(static_cast<_Type>(1), static_cast<_Type>(0), static_cast<_Type>(0));

		// find min coord
		_Type _min = x;
		if (abs(y) < _min) {
			_min = y;
			cAxis.x = static_cast<_Type>(0);
			cAxis.z = static_cast<_Type>(0);
			cAxis.y = static_cast<_Type>(1);
		}
		if (abs(z) < _min) {
			cAxis.x = static_cast<_Type>(0);
			cAxis.y = static_cast<_Type>(0);
			cAxis.z = static_cast<_Type>(1);
		}

		return cross(cAxis);
	}
	Vec3 normalize() const {
		_Type len = length();
		if (len == 0)
			len = 1;
		return this->operator*(1 / len);
	}
	Vec3 Transform(const Mat4<_Type>& M) const {
		_Type _x = M.m[0] * x + M.m[4] * y + M.m[8] * z + M.m[12] * w;
		_Type _y = M.m[1] * x + M.m[5] * y + M.m[9] * z + M.m[13] * w;
		_Type _z = M.m[2] * x + M.m[6] * y + M.m[10] * z + M.m[14] * w;
		_Type _w = M.m[3] * x + M.m[7] * y + M.m[11] * z + M.m[15] * w;
		return Vec3(_x, _y, _z, _w);
	}
	Mat4<_Type> multByColumn(const Vec3& _v) const {
		return Mat4<_Type>{
				x* _v.x, x* _v.y, x* _v.z, x* _v.w,
				y* _v.x, y* _v.y, y* _v.z, y* _v.w,
				z* _v.x, z* _v.y, z* _v.z, z* _v.w,
				w* _v.x, w* _v.y, w* _v.z, w* _v.w
		};
	}
	Vec3 Project(Vec3& _v) {
		Vec3 res = _v.normalize() * dot(_v.normalize());
		return res;
	}
	Vec3 Reject(Vec3& _v) {
		Vec3 res = *this - Project(_v);
		return res;
	}
	Vec3 Reflect(Vec3& _v, real32 coeff = 2) {
		Vec3 res = *this - (Project(_v) * coeff); // (Reject(_v) * 2) - this; 
		return res;
	}
public:	
	static void OrthoNormilize(Vec3& _v1, Vec3& _v2) noexcept; // Gram-Schmidt
	static Vec3 LinearInterp(const Vec3& _v1, const Vec3& _v2, real32 delta); // calculate linear interpolation 
	// calcucate barycentric coordinates
	static Vec3 Barycentric(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& P); // P must belong to a triangle
	static Vec3 Barycentric(const Vec2<_Type>& A, const Vec2<_Type>& B, const Vec2<_Type>& C, const Vec2<_Type>& P); // P must belong to a triangle
}; 


template<typename _Type>
Vec3<_Type> Vec3<_Type>::LinearInterp(const Vec3& _v1, const Vec3& _v2, real32 delta) {
	_Type x = _v1.x + (_v2.x - _v1.x) * delta;
	_Type y = _v1.y + (_v2.y - _v1.y) * delta;
	_Type z = _v1.z + (_v2.z - _v1.z) * delta;
	return Vec3<_Type>(x, y, z);
}

template<typename _Type>
void Vec3<_Type>::OrthoNormilize(Vec3& _v1, Vec3& _v2) noexcept {
	_v1 = _v1.normalize();
	Vec3 _cn = _v1.cross(_v2).normalize();
	_v2 = _cn.cross(_v1);
}

template<typename _Type>
Vec3<_Type> Vec3<_Type>::Barycentric(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& P)
{
	Vec3 _s[2];
	_s[0].x = C.x - A.x; _s[0].y = B.x - A.x; _s[0].z = A.x - P.x;
	_s[1].x = C.y - A.y; _s[1].y = B.y - A.y; _s[1].z = A.y - P.y;
	Vec3 u = _s[0].cross(_s[1]);
	if (u.z == 0) // if z is 0 it's degenerate triangle ABC
		return Vec3(-1, 1, 1);

	return Vec3(static_cast<_Type>(1) - ((u.x + u.y) / u.z), u.y / u.z, u.x / u.z);
}

template<typename _Type>
Vec3<_Type> Vec3<_Type>::Barycentric(const Vec2<_Type>& A, const Vec2<_Type>& B, const Vec2<_Type>& C, const Vec2<_Type>& P)
{
	Vec3 _s[2];
	_s[0].x = C.x - A.x; _s[0].y = B.x - A.x; _s[0].z = A.x - P.x;
	_s[1].x = C.y - A.y; _s[1].y = B.y - A.y; _s[1].z = A.y - P.y;
	Vec3 u = _s[0].cross(_s[1]);
	if (u.z == 0) // if z is 0 it's degenerate triangle ABC
		return Vec3(-1, 1, 1);

	return Vec3(static_cast<_Type>(1) - ((u.x + u.y) / u.z), u.y / u.z, u.x / u.z);
}

template<>
Vec3<real32> Vec3<real32>::normalize() const {
	real32 len = length();
	if (cmpr32(len, 0.f))
		len = 1.f;
	real32 res = 1.f / len;
	return Vec3(x * res, y * res, z * res);
}

template<>
Vec3<real64> Vec3<real64>::normalize() const {
	real64 len = length();
	if (cmpr64(len, 0.))
		len = 1.;
	real64 res = 1. / len;
	return Vec3(x * res, y * res, z * res);
}

template<>
bool Vec3<real32>::operator==(const Vec3& _v) const {
	return (cmpr32(x,_v.x) && cmpr32(y, _v.y) && cmpr32(z, _v.z));
}

template<>
bool Vec3<real64>::operator==(const Vec3& _v) const {
	return (cmpr64(x, _v.x) && cmpr64(y, _v.y) && cmpr64(z, _v.z));
}

template<>
bool Vec3<real32>::operator!=(const Vec3& _v) const {
	return (!cmpr32(x, _v.x) || !cmpr32(y, _v.y) || !cmpr32(z, _v.z));
}

template<>
bool Vec3<real64>::operator!=(const Vec3& _v) const {
	return (!cmpr64(x, _v.x) || !cmpr64(y, _v.y) || !cmpr64(z, _v.z));
}




template<typename _Type>
class Mat4
{
	//todo add fill(clear) with value
public:
	static const size_t size = 16;
	_Type m[size];
public:
	Mat4() : m{ 0 } {}
	Mat4(std::initializer_list<_Type> mat) {
		assert(mat.size() == size);
		std::copy(mat.begin(), mat.end(), begin(m));
	}
	Mat4(const Mat4& mat) {
		std::copy(begin(mat.m), end(mat.m), begin(m));
	}
	Mat4& identity() {
		zero();
		m[0] = 1; m[5] = 1; m[10] = 1; m[15] = 1;
		return *this;
	}
	Mat4& zero() noexcept {
		std::fill(begin(m), end(m), 0);
		return *this;
	}
	Mat4& clear(_Type val) noexcept {
		std::fill(begin(m), end(m), val);
		return *this;
	}
	Mat4& operator=(const Mat4& mat) noexcept {
		std::copy(begin(mat.m), end(mat.m), begin(m));
		return *this;
	}
	Mat4 neg() const {
		return Mat4{ -m[0], -m[1], -m[2], -m[3],
					 -m[4], -m[5], -m[6], -m[7],
					 -m[8], -m[9], -m[10], -m[11],
					 -m[12], -m[13], -m[14], -m[15] };
	}
	Mat4 operator+(const Mat4& mat) const {
		return Mat4{ m[0] + mat.m[0], m[1] + mat.m[1], m[2] + mat.m[2], m[3] + mat.m[3],
					 m[4] + mat.m[4], m[5] + mat.m[5], m[6] + mat.m[6], m[7] + mat.m[7],
					 m[8] + mat.m[8], m[9] + mat.m[9], m[10] + mat.m[10], m[11] + mat.m[11],
					 m[12] + mat.m[12], m[13] + mat.m[13], m[14] + mat.m[14], m[15] + mat.m[15] };			
	}
	Mat4 operator-(const Mat4& mat) const {
		return Mat4{ m[0] - mat.m[0], m[1] - mat.m[1], m[2] - mat.m[2], m[3] - mat.m[3],
					 m[4] - mat.m[4], m[5] - mat.m[5], m[6] - mat.m[6], m[7] - mat.m[7],
					 m[8] - mat.m[8], m[9] - mat.m[9], m[10] - mat.m[10], m[11] - mat.m[11],
					 m[12] - mat.m[12], m[13] - mat.m[13], m[14] - mat.m[14], m[15] - mat.m[15] };
	}
	Mat4 operator*(const Mat4& mat) const;
	Vec3<_Type> operator*(const Vec3<_Type>& vec) const;
	_Type det() const;
	Mat4 transpose() const;
	_Type trace() const;
	Mat4 normalize() const;
	Mat4 invert() const;
	Mat4 rotate(const Vec3<_Type>& axis, real32 AngleRad) const; // return rotation matrix from axis and angle
	Mat4 rotateX(real32 AngleRad) const; // return rotation matrix by X axis
	Mat4 rotateY(real32 AngleRad) const; // return rotation matrix by Y axis
	Mat4 rotateZ(real32 AngleRad) const; // return rotation matrix by Z axis
	Mat4 rotateXYZ(const Vec3<_Type>& AngleVec) const; // return rotation matrix by XYZ axis
public:
	template <typename _Type_>
	friend std::ostream& operator<<(std::ostream& out, const Mat4<_Type_>& m);
	static Mat4 translate(_Type x, _Type y, _Type z);
	static Mat4 translate(Vec3<_Type> _v);
	static Mat4 scale(_Type x, _Type y, _Type z);
	static Mat4 scale(Vec3<_Type> _v);
};


template<typename _Type>
inline std::ostream& operator<<(std::ostream& out, const Mat4<_Type>& mat) {
	for (size_t it = 0; it < Mat4<_Type>::size; ++it) {
		out << mat.m[it] << " ";
		if (!((it+1) % 4)) // next line
			out << std::endl;	
	}	
	return out;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::operator*(const Mat4& mat) const {

	_Type x00 = m[0] * mat.m[0] + m[1] * mat.m[4] + m[2] * mat.m[8] + m[3] * mat.m[12];		// 1
	_Type x01 = m[0] * mat.m[1] + m[1] * mat.m[5] + m[2] * mat.m[9] + m[3] * mat.m[13];		// 2
	_Type x02 = m[0] * mat.m[2] + m[1] * mat.m[6] + m[2] * mat.m[10] + m[3] * mat.m[14];	// 3
	_Type x03 = m[0] * mat.m[3] + m[1] * mat.m[7] + m[2] * mat.m[11] + m[3] * mat.m[15];	// 4
	_Type x04 = m[4] * mat.m[0] + m[5] * mat.m[4] + m[6] * mat.m[8] + m[7] * mat.m[12];		// 5
	_Type x05 = m[4] * mat.m[1] + m[5] * mat.m[5] + m[6] * mat.m[9] + m[7] * mat.m[13];		// 6
	_Type x06 = m[4] * mat.m[2] + m[5] * mat.m[6] + m[6] * mat.m[10] + m[7] * mat.m[14];	// 7
	_Type x07 = m[4] * mat.m[3] + m[5] * mat.m[7] + m[6] * mat.m[11] + m[7] * mat.m[15];	// 8
	_Type x08 = m[8] * mat.m[0] + m[9] * mat.m[4] + m[10] * mat.m[8] + m[11] * mat.m[12];	// 9
	_Type x09 = m[8] * mat.m[1] + m[9] * mat.m[5] + m[10] * mat.m[9] + m[11] * mat.m[13];	// 10
	_Type x10 = m[8] * mat.m[2] + m[9] * mat.m[6] + m[10] * mat.m[10] + m[11] * mat.m[14];	// 11
	_Type x11 = m[8] * mat.m[3] + m[9] * mat.m[7] + m[10] * mat.m[11] + m[11] * mat.m[15];	// 12
	_Type x12 = m[12] * mat.m[0] + m[13] * mat.m[4] + m[14] * mat.m[8] + m[15] * mat.m[12];	// 13
	_Type x13 = m[12] * mat.m[1] + m[13] * mat.m[5] + m[14] * mat.m[9] + m[15] * mat.m[13];	// 14
	_Type x14 = m[12] * mat.m[2] + m[13] * mat.m[6] + m[14] * mat.m[10] + m[15] * mat.m[14];// 15
	_Type x15 = m[12] * mat.m[3] + m[13] * mat.m[7] + m[14] * mat.m[11] + m[15] * mat.m[15];// 16

	return Mat4
	{
		x00, x01, x02, x03,
		x04, x05, x06, x07,
		x08, x09, x10, x11,
		x12, x13, x14, x15		
	};
}

template<typename _Type>
Vec3<_Type> Mat4<_Type>::operator*(const Vec3<_Type>& vec) const
{
	_Type x11 = m[0] * vec.x + m[1] * vec.y + m[2] * vec.z + m[3] * vec.w;
	_Type x21 = m[4] * vec.x + m[5] * vec.y + m[6] * vec.z + m[7] * vec.w;
	_Type x31 = m[8] * vec.x + m[9] * vec.y + m[10] * vec.z + m[11] * vec.w;
	_Type x41 = m[12] * vec.x + m[13] * vec.y + m[14] * vec.z + m[15] * vec.w;
	return Vec3<_Type>( x11, x21, x31, x41 );
}



template<typename _Type>
_Type Mat4<_Type>::det() const {
	
	// cache value for optimization
	_Type x00 = m[0], x01 = m[1], x02 = m[2], x03 = m[3];
	_Type x10 = m[4], x11 = m[5], x12 = m[6], x13 = m[7];
	_Type x20 = m[8], x21 = m[9], x22 = m[10], x23 = m[11];
	_Type x30 = m[12], x31 = m[13], x32 = m[14], x33 = m[15];

	return	   (x03 * x12 * x21 * x30 - x03 * x12 * x20 * x31 - x03 * x11 * x22 * x30 + x03 * x10 * x22 * x31 +
				x03 * x11 * x20 * x32 - x03 * x10 * x21 * x32 - x02 * x13 * x21 * x30 + x02 * x13 * x20 * x31 +
				x01 * x13 * x22 * x30 - x00 * x13 * x22 * x31 - x01 * x13 * x20 * x32 + x00 * x13 * x21 * x32 +
				x02 * x11 * x23 * x30 - x02 * x10 * x23 * x31 - x01 * x12 * x23 * x30 + x00 * x12 * x23 * x31 +
				x01 * x10 * x23 * x32 - x00 * x11 * x23 * x32 - x02 * x11 * x20 * x33 + x02 * x10 * x21 * x33 +
 				x01 * x12 * x20 * x33 - x00 * x12 * x21 * x33 - x01 * x10 * x22 * x33 + x00 * x11 * x22 * x33 );
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::transpose() const {
	return Mat4{
		m[0], m[4], m[8], m[12],
		m[1], m[5], m[9], m[13],
		m[2], m[6], m[10], m[14],
		m[3], m[7], m[11], m[15]
	};
}

template<typename _Type>
_Type Mat4<_Type>::trace() const {
	return (m[0] + m[5] + m[10] + m[15]);
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::normalize() const {

	_Type det = det();

	if (det == 0)
		throw std::overflow_error("Divide by zero! Determinant in normalize matrix");

	return Mat4{
		m[0] / det,  m[1] / det,  m[2] / det,  m[3] / det,
		m[4] / det,  m[5] / det,  m[6] / det,  m[7] / det,
		m[8] / det,  m[9] / det,  m[10] / det, m[11] / det,
		m[12] / det, m[13] / det, m[14] / det, m[15] / det,
	};
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::invert() const {
	// cache value for optimization
	_Type x00 = m[0], x01 = m[1], x02 = m[2], x03 = m[3];
	_Type x10 = m[4], x11 = m[5], x12 = m[6], x13 = m[7];
	_Type x20 = m[8], x21 = m[9], x22 = m[10], x23 = m[11];
	_Type x30 = m[12], x31 = m[13], x32 = m[14], x33 = m[15];

	_Type y00 = x00 * x11 - x01 * x10; 
	_Type y01 = x00 * x12 - x02 * x10;	
	_Type y02 = x00 * x13 - x03 * x10;	
	_Type y03 = x01 * x12 - x02 * x11;
	_Type y04 = x01 * x13 - x03 * x11;	
	_Type y05 = x02 * x13 - x03 * x12;	
	_Type y06 = x20 * x31 - x21 * x30;	
	_Type y07 = x20 * x32 - x22 * x30;	
	_Type y08 = x20 * x33 - x23 * x30;
	_Type y09 = x21 * x32 - x22 * x31;	
	_Type y10 = x21 * x33 - x23 * x31;	
	_Type y11 = x22 * x33 - x23 * x32;

	_Type invDenom = (y00 * y11 - y01 * y10 + y02 * y09 + y03 * y08 - y04 * y07 + y05 * y06);

	if (invDenom == 0)
		throw std::overflow_error("Divide by zero! inv Denom in inverse matrix");

	real64 invertDet = 1. / invDenom;

	_Type n00 = (x11 * y11 - x12 * y10 + x13 * y09)  * invertDet;
	_Type n01 = (-x01 * y11 + x02 * y10 - x03 * y09) * invertDet;
	_Type n02 = (x31 * y05 - x32 * y04 + x33 * y03)  * invertDet;
	_Type n03 = (-x21 * y05 + x22 * y04 - x23 * y03) * invertDet;
	_Type n04 = (-x10 * y11 + x12 * y08 - x13 * y07) * invertDet;
	_Type n05 = (x00 * y11 - x02 * y08 + x03 * y07)  * invertDet;
	_Type n06 = (-x30 * y05 + x32 * y02 - x33 * y01) * invertDet;
	_Type n07 = (x20 * y05 - x22 * y02 + x23 * y01)  * invertDet;
	_Type n08 = (x10 * y10 - x11 * y08 + x13 * y06)  * invertDet;
	_Type n09 = (-x00 * y10 + x01 * y08 - x03 * y06) * invertDet;
	_Type n10 = (x30 * y04 - x31 * y02 + x33 * y00)  * invertDet;
	_Type n11 = (-x20 * y04 + x21 * y02 - x23 * y00) * invertDet;
	_Type n12 = (-x10 * y09 + x11 * y07 - x12 * y06) * invertDet;
	_Type n13 = (x00 * y09 - x01 * y07 + x02 * y06)  * invertDet;
	_Type n14 = (-x30 * y03 + x31 * y01 - x32 * y00) * invertDet;
	_Type n15 = (x20 * y03 - x21 * y01 + x22 * y00)  * invertDet;

	return Mat4{
		n00, n01, n02, n03,
		n04, n05, n06, n07,
		n08, n09, n10, n11,
		n12, n13, n14, n15
	};
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::translate(_Type x, _Type y, _Type z) {
	Mat4<_Type> mat;
	mat.identity();
	mat.m[3] = x; mat.m[7] = y; mat.m[11] = z;
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::translate(Vec3<_Type> _v) {
	Mat4<_Type> mat;
	mat.identity();
	mat.m[3] = _v.x; mat.m[7] = _v.y; mat.m[11] = _v.z;
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::scale(_Type x, _Type y, _Type z) {
	Mat4<_Type> mat;
	mat.m[0] = x; mat.m[5] = y; mat.m[10] = z; mat.m[15] = static_cast<_Type>(1);
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::scale(Vec3<_Type> _v) {
	Mat4<_Type> mat;
	mat.m[0] = _v.x; mat.m[5] = _v.y; mat.m[10] = _v.z; mat.m[15] = static_cast<_Type>(1);
	return mat;
}


template<typename _Type>
Mat4<_Type> Mat4<_Type>::rotate(const Vec3<_Type>& axis, real32 AngleRad) const {

	real32 len = axis.length();
	_Type X = axis.x, Y = axis.y, Z = axis.z;
	if (!cmpr32(len, 0.f) && !cmpr32(len, 1.f)) {
		len = 1. / len;
		X *= len; Y *= len; Z *= len;
	}

	real32 angle = WrapPosNegPI(AngleRad);
	real32 _cos = cos_rad(angle);
	real32 _sin = 1.f - (_cos * _cos);
	real32 _res = 1.f - _cos;

	Mat4 mat;

	mat.m[0] = X * X * _res + _cos;
	mat.m[1] = Y * X * _res + Z * _sin;
	mat.m[2] = Z * X * _res - Y * _sin;
	mat.m[4] = X * Y * _res - Z * _sin;
	mat.m[5] = Y * Y * _res + _cos;
	mat.m[6] = Z * Y * _res + X * _sin;
	mat.m[8] = X * Z * _res + Y * _sin;
	mat.m[9] = Y * Z * _res - X * _sin;
	mat.m[10] = Z * Z * _res + _cos;
	mat.m[14] = static_cast<_Type>(1);

	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::rotateX(real32 AngleRad) const {
	Mat4 mat = Mat4::identity();
	real32 angle = WrapPosNegPI(AngleRad);

	real32 _sin = sin_rad(angle);
	real32 _cos = cos_rad(angle);

	mat.m[5] = _cos; mat.m[6] = -_sin;
	mat.m[9] = _sin; mat.m[10] = _cos;
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::rotateY(real32 AngleRad) const {
	Mat4 mat = Mat4::identity();
	real32 angle = WrapPosNegPI(AngleRad);

	real32 _sin = sin_rad(angle);
	real32 _cos = cos_rad(angle);

	mat.m[0] = _cos; mat.m[2] = _sin;
	mat.m[8] = _sin; mat.m[10] = _cos;
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::rotateZ(real32 AngleRad) const {
	Mat4 mat = Mat4::identity();
	real32 angle = WrapPosNegPI(AngleRad);

	real32 _sin = sin_rad(angle);
	real32 _cos = cos_rad(angle);

	mat.m[0] = _cos; mat.m[1] = -_sin;
	mat.m[4] = _sin; mat.m[5] = _cos;
	return mat;
}

template<typename _Type>
Mat4<_Type> Mat4<_Type>::rotateXYZ(const Vec3<_Type>& AngleVec) const {
	Mat4 mat = Mat4::identity();

	real32 _cosX = cos_rad(-AngleVec.x);
	real32 _cosY = cos_rad(-AngleVec.y);
	real32 _cosZ = cos_rad(-AngleVec.z);
	real32 _sinX = sin_rad(-AngleVec.x);
	real32 _sinY = sin_rad(-AngleVec.y);
	real32 _sinZ = sin_rad(-AngleVec.z);

	Mat4 mat;
	
	mat.m[0] = _cosY * _cosZ; mat.m[4] = (_sinX * _sinY * _cosZ) - (_sinZ * _cosX);
	mat.m[8] = (_cosX * _sinY * _cosZ) + (_sinZ * _sinX);
	mat.m[1] = _sinZ * _cosY; mat.m[5] = (_sinX * _sinY * _sinZ) + (_cosZ * _cosX);
	mat.m[9] = (_cosX * _sinY * _sinZ) - (_cosZ * _sinX);
	mat.m[2] = -_sinY; mat.m[6] = _cosY * _sinX;
	mat.m[10] = _cosY * _cosX;

	return mat;
}


using Matrix = Mat4<real32>;