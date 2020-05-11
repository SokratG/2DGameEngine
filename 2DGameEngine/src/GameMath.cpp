#include <cmath>
#include "GameMath.h"


using std::numeric_limits;

#define ABS(VAL) (VAL = VAL < 0 ? -VAL : VAL)
#define FABS(VAL) (VAL = VAL < 0.f ? -VAL : VAL)



// -------------------------------

// gcd runtime
uint64 __fastcall gcd_fast(uint64 a, uint64 b) noexcept
{
    uint64 r = 1;
    while (r) {
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Factorial runtime
uint64 __fastcall Factorial(uint64 _val) noexcept
{
    assert(_val >= 0U);
    uint64 result = _val;
    while (_val > 1) {
        result *= --_val;
    }
    return result;
}

// lcm runtime
uint64 __fastcall lcm_fast(uint64 a, uint64 b) noexcept
{
    return ((a * b) / gcd_fast(a, b));
}


// --------- Square root section ------------
// Newton method
uint32 __fastcall sqrt_f(uint32 s) noexcept {
    assert(s >= 0U);
    uint32 t = (s >> 4) + 1;  // initial guess   // t*t will become s
    int32 n;
    for (n = 16; n; --n) { // will finish
        t = ((t * t + s) / t) >> 1;
    }
    return t;
}

real32 __fastcall sqrt_f(real32 s) noexcept {
    assert(s >= 0.);
    real32 t = (s / 16) + 1;  // initial guess 
    int32 n;
    for (n = 16; n; --n) { // will finish
        t = ((t * t + s) / t) / 2;
    }
    return t;
}


real64 __fastcall sqrt_f(real64 s) noexcept {
    assert(s >= 0.);
    real64 t = (s / 16) + 1;  // initial guess 
    int32 n;
    for (n = 16; n; --n) { // will finish
        t = ((t * t + s) / t) / 2;
    }
    return t;
}

/* this is square root function, uses binary search and halves mantisa */
#ifdef OFF
static real32 __fastcall __sqrtf32(real32 s) noexcept {
    assert(s >= 0.);

    real32 b = s;

    int32* x = (int32*)(&b); // interp real value how integer(hack?)

    int32 c = ((*x >> 23) & 255) - 127; 

    if (c < 0) c = -((-c) >> 1); 
                                
    else c >>= 1;               

    *x &= ~(255 << 23);

    *x |= (c + 127) << 23;
    // bsearch
    for (int32 i = 0; i < 5; i++) b = (b + s / b) / 2; // here normal square root approximation runs 5 times

    return b;
}
#endif
// -------------------------------------------

// --------- Trigonometry section ------------

real32 __fastcall DegreeToRadian(real32 _angle) noexcept
{
    return (_angle * (_PI / 180.f));
}

real32 __fastcall RadianToDegree(real32 _rad) noexcept
{
    return (_rad * (180.f / _PI));
}


real64 __fastcall DegreeToRadian(real64 _angle) noexcept
{
    return (_angle * (_PI / 180.));
}

real64 __fastcall RadianToDegree(real64 _rad) noexcept
{
    return (_rad * (180. / _PI));
}




// Normilize angle:
// https://stackoverflow.com/questions/4633177/c-how-to-wrap-a-float-to-the-interval-pi-pi/4635752
// wrap [rad] angle to [-PI..PI)
real64 __fastcall WrapPosNegPI(real64 fAng) noexcept
{
    return mod(fAng + _PI, (real64)_PI_2) - _PI;
}

// wrap [rad] angle to [0.._PI_2)
real64 __fastcall WrapTwoPI(real64 fAng) noexcept
{
    return mod(fAng, (real64)_PI_2);
}

// wrap [deg] angle to [-180..180)
real64 __fastcall WrapPosNeg180(real64 fAng) noexcept
{
    return mod(fAng + 180., 360.) - 180.;
}

// wrap [deg] angle to [0..360)
real64 __fastcall Wrap360(real64 fAng) noexcept
{
    return mod(fAng, 360.);
}


real32 __fastcall WrapPosNegPI(real32 fAng) noexcept
{
    return mod<real32>(fAng + _PI, _PI_2) - _PI;
}

// wrap [rad] angle to [0.._PI_2)
real32 __fastcall WrapTwoPI(real32 fAng) noexcept
{
    return mod<real32>(fAng, _PI_2);
}

// wrap [deg] angle to [-180..180)
real32 __fastcall WrapPosNeg180(real32 fAng) noexcept
{
    return mod<real32>(fAng + 180.f, 360.f) - 180.f;
}

// wrap [deg] angle to [0..360)
real32 __fastcall Wrap360(real32 fAng) noexcept
{
    return mod<real32>(fAng, 360.f);
}


// sine
static inline real64 __fastcall calc_series_sin(real64 rad) noexcept
{
    static uint64 factdenom[] = {
        factorial<3>::value,
        factorial<5>::value,
        factorial<7>::value,
        factorial<9>::value
    };
    real64 x = rad * rad * rad;
    real64 x_1 = x * rad * rad;
    real64 x_2 = x_1 * rad * rad;
    real64 x_3 = x_2 * rad * rad;
    return (rad - (x / factdenom[0]) + (x_1 / factdenom[1]) - (x_2 / factdenom[2]) + (x_3 / factdenom[3]));
}


static inline real32 __fastcall calc_series_sin(real32 rad) noexcept
{
    static uint32 factdenom[] = {
        factorial<3>::value,
        factorial<5>::value,
        factorial<7>::value,
        factorial<9>::value
    };
    real32 x = rad * rad * rad;
    real32 x_1 = x * rad * rad;
    real32 x_2 = x_1 * rad * rad;
    real32 x_3 = x_2 * rad * rad;
    return (rad - (x / factdenom[0]) + (x_1 / factdenom[1]) - (x_2 / factdenom[2]) + (x_3 / factdenom[3]));
}

#pragma warning(push)
#pragma warning(disable : 4244)
real32 __fastcall sin_fangle(real32 angle) noexcept
{
    real32 Normal_Angle = WrapPosNeg180(angle);
    real32 rad = DegreeToRadian(Normal_Angle);
    return calc_series_sin(rad);
}

real32 __fastcall sin_rad(real32 rad) noexcept
{
    real32 Normal_Angle = WrapPosNegPI(rad);
    return calc_series_sin(Normal_Angle);
}
#pragma warning(pop)

real64 __fastcall sin_fangle(real64 angle) noexcept
{
    real64 Normal_Angle = WrapPosNeg180(angle);
    real64 rad = DegreeToRadian(Normal_Angle);
    return calc_series_sin(rad);
}




real64 __fastcall sin_rad(real64 rad) noexcept
{
    real64 Normal_Angle = WrapPosNegPI(rad);
    return calc_series_sin(Normal_Angle);
}



// cosine
static inline real32 __fastcall calc_series_cos(real32 rad) noexcept
{
    static uint32 factdenom[] = {
        factorial<2>::value,
        factorial<4>::value,
        factorial<6>::value,
        factorial<8>::value
    };
    real32 x = rad * rad;
    real32 x_1 = x * rad * rad;
    real32 x_2 = x_1 * rad * rad;
    real32 x_3 = x_2 * rad * rad;
    return (1 - (x / factdenom[0]) + (x_1 / factdenom[1]) - (x_2 / factdenom[2]) + (x_3 / factdenom[3]));
}

static inline real64 __fastcall calc_series_cos(real64 rad) noexcept
{
    static uint64 factdenom[] = {
        factorial<2>::value,
        factorial<4>::value,
        factorial<6>::value,
        factorial<8>::value
    };
    real64 x = rad * rad;
    real64 x_1 = x * rad * rad;
    real64 x_2 = x_1 * rad * rad;
    real64 x_3 = x_2 * rad * rad;
    return (1 - (x / factdenom[0]) + (x_1 / factdenom[1]) - (x_2 / factdenom[2]) + (x_3 / factdenom[3]));
}

#pragma warning(push)
#pragma warning(disable : 4244)
real32 __fastcall cos_fangle(real32 angle) noexcept
{
    real32 Normal_Angle = WrapPosNeg180(angle);
    real32 rad = DegreeToRadian(Normal_Angle);
    return calc_series_cos(rad);
}

real32 __fastcall cos_rad(real32 rad) noexcept
{
    real32 Normal_Angle = WrapPosNegPI(rad);
    return calc_series_cos(Normal_Angle);
}
#pragma warning(pop)

real64 __fastcall cos_fangle(real64 angle) noexcept
{
    real64 Normal_Angle = WrapPosNeg180(angle);
    real64 rad = DegreeToRadian(Normal_Angle);
    return calc_series_cos(rad);
}



real64 __fastcall cos_rad(real64 rad) noexcept
{
    real64 Normal_Angle = WrapPosNegPI(rad);
    return calc_series_cos(Normal_Angle);
}



#ifdef OFF
// atan
real32 __fastcall atan_fast(real32 rad) noexcept
{
    const real32 num1 = (_PI / 4) * rad;
    const real32 num2 = rad * (ABS(rad) - 1);
    const real32 num3 = (0.2447f + 0.0663f * ABS(rad));
    return (num1 - num2 * num3);
}


real64 __fastcall atan_fast(real64 rad) noexcept
{   
    const real64 num1 = (_PI / 4) * rad;
    const real64 num2 = rad * (ABS(rad) - 1);
    const real64 num3 = (0.2447 + 0.0663 * ABS(rad));
    return (num1 - num2 * num3);
}

// atan2
real32 __fastcall atan2_fast(real32 x, real32 y) noexcept
{
    if (x != 0.f) {
        if (FABS(x) > FABS(y)) {
            const real32 _t = y / x;
            if (x > 0.f)
                return atan_fast(_t);
            else if (y >= 0.f)
                return atan_fast(_t) + _PI;
            else
                return atan_fast(_t) - _PI;
        }
        else {
            const real32 _t = x / y;
            if (y > 0.f)
                return -atan_fast(_t) + _PI_HALF;
            else
                return -atan_fast(_t) - _PI_HALF;
        }
    }
    else {
        if (y > 0.f)
            return _PI_HALF;
        else if (y < 0.f)
            return -_PI_HALF;
    }
    return 0.f;
}

real64 __fastcall atan2_fast(real64 x, real64 y) noexcept
{
    if (x != 0.) {
        if (FABS(x) < FABS(y)) {
            const real64 _t = y / x;
            if (x > 0.)
                return atan_fast(_t);
            else if (y >= 0.)
                return atan_fast(_t) + _PI;
            else
                return atan_fast(_t) - _PI;
        }
        else {
            const real64 _t = x / y;
            if (y > 0.)
                return -atan_fast(_t) + _PI_HALF;
            else
                return -atan_fast(_t) - _PI_HALF;
        }
    }
    else {
        if (y > 0.)
            return _PI_HALF;
        else if (y < 0.)
            return -_PI_HALF;
    }
    return 0.;
}

#endif