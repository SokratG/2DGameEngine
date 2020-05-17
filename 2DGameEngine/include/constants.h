#pragma once
#ifndef _CONSTANT_H_
#define _CONSTANT_H_
// TODO - Save in data file constant
//-----------------------------------------------
//                  Constants
//-----------------------------------------------
#include <Windows.h>
#define KEYS_TABLE 256
#define SHIFTED (short)0x8000
#define COMPLETE 1
#define ERROR_APP -1
#define MAX_PATH 256
#define MAX_NAME 64
#define MAX_TYPE 2
#define RECT_VERT 4

// color console
#define CRED 12
#define CBLUE 9
#define CGREEN 10
#define CLBLUE 11
#define CYELLOW 14
#define CDEFUALT 7
#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255)  // transparent color (magenta)


// key mappings
// In this game simple constants are used for key mappings.
const UCHAR CONSOLE_KEY =   VK_OEM_3; // ~ key 
const UCHAR ESC_KEY =       VK_ESCAPE; // Escape key
const UCHAR ALT_KEY =       VK_MENU; // Alt key
const UCHAR ENTER_KEY =     VK_RETURN; // Enter key
const UCHAR _LEFT_KEY =     'A';
const UCHAR _RIGHT_KEY =    'D';
const UCHAR _FORWARD_KEY =  'W';
const UCHAR _BACK_KEY =     'S';
const UCHAR _LEFT_KEY2 =    VK_LEFT; // Left arrow
const UCHAR _RIGHT_KEY2 =   VK_RIGHT; // Right arrow
const UCHAR _FORWARD_KEY2 = VK_UP; // Up arrow
const UCHAR _BACK_KEY2 =    VK_DOWN; // Down arrow

// game settings
const INT32 GAME_WIDTH = 1280;
const INT32 GAME_HEIGHT = 720;
const FLOAT MIN_FRAME_RATE = 10.f; 
const FLOAT FRAME_RATE = 100.f; // the target frame rate (frames/sec)
const FLOAT MIN_FRAME_TIME = 1.f / FRAME_RATE; // minimum desired time for 1 frame
const FLOAT MAX_FRAME_TIME = 1.f / MIN_FRAME_RATE; // maximum time used in calculations

namespace GraphColor {
    enum DISPLAY_MODE { TOGGLE, FULLSCREEN, WINDOW };
}

const LPCWSTR CLASS_NAME = L"Game2DEngine";

//=============================================================================
// Function templates for safely dealing with pointer referenced items.
// The functions defined by these templates may be called using a normal
// function call syntax. The compiler will create a function that replaces T
// with the type of the calling parameter.
//=============================================================================
// Safely release pointer referenced item
template <typename _Type>
inline void safeRelease(_Type& ptr)
{
    if (ptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}
#define SAFE_RELEASE safeRelease            // for backward compatiblility

// Safely delete pointer referenced item
template <typename _Type>
inline void safeDelete(_Type& ptr)
{
    if (ptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}
#define SAFE_DELETE safeDelete              // for backward compatiblility

// Safely delete pointer referenced array
template <typename _Type>
inline void safeDeleteArray(_Type& ptr)
{
    if (ptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
};
#define SAFE_DELETE_ARRAY safeDeleteArray   // for backward compatiblility


// Safely call onLostDevice
template <typename _Type>
inline void safeOnLostDevice(_Type& ptr)
{
    if (ptr)
        ptr->onLostDevice();
}
#define SAFE_ON_LOST_DEVICE safeOnLostDevice    // for backward compatiblility


// Safely call onResetDevice
template <typename _Type>
inline void safeOnResetDevice(_Type& ptr)
{
    if (ptr)
        ptr->onResetDevice();
}
#define SAFE_ON_RESET_DEVICE safeOnResetDevice  // for backward compatiblility

#endif /* _CONSTANT_H_ */