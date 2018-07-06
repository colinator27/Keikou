#ifndef KEIKOU_MAIN_HEADER
#define KEIKOU_MAIN_HEADER

#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

#include "api_actions.h"

// Forward declaration of error retrieval.
const char* GetErrorFor(unsigned int action);

// Handles all window events:
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// Releases memory:
template<class T> void SafeRelease(T **ppT);

// Generates a window a proper window to use.
HWND GenerateWindow(HINSTANCE hInstance);

#define CheckSuccess(action, result)                            \
    if (result < 0)  {                                          \
        MessageBox(NULL, GetErrorFor(action), "Error", MB_OK);  \
        system("EXIT");                                         \
    }

// Concantenate strings
#define STRCAT(str1, str2) (const char*)strcat(#str1, #str2)

#endif