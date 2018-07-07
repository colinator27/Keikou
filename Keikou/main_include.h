#ifndef KEIKOU_MAIN_INLCUDE_HEADER
#define KEIKOU_MAIN_INLCUDE_HEADER

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

#define CODEFOR(x) #x

#define CREATE              0x00000001
#define UNINITIALIZED_APP   0x10000000
#define CLASS_REGISTRATION  0x10001000
#define CLSREGCREATE        CLASS_REGISTRATION | CREATE
#define WINDOW              0x10002000
#define WINCREATE           WINDOW | CREATE
#define D2DFACTORY          0x10003000
#define D2DFCREATE          D2DFACTORY | CREATE
#define DWRITEFACTORY       0x10004000
#define DWFCREATE           DWRITEFACTORY | CREATE
#define TEXTFORMAT          0x10005000
#define TEXTALIGNMENT       0x10006000
#define PARAGRAPH_ALIGNMENT 0x10007000
#define RENDER_TARGET       0x10008000
#define HWNDRTCREATE        RENDER_TARGET | CREATE
#define SOLID_COLOR_BRUSH   0x10009000
#define SCBCREATE           SOLID_COLOR_BRUSH | CREATE

typedef signed char     sbyte;
typedef unsigned int    uint;
typedef const char*     string;
typedef const wchar_t*  wstring;

// Tells if the application is initialized.
extern sbyte initialized;

// The application instance.
extern HINSTANCE hInstance;

// Forward declaration of error retrieval.
string GetErrorFor(uint action);

#define CheckSuccess(action, result)                            \
    if (result < 0)  {                                          \
        MessageBox(NULL, GetErrorFor(action), "Error", MB_OK);  \
        exit(-1);                                               \
    }

#define AppIsInitialized()                                                  \
    if (initialized == -1) {                                                \
        MessageBox(NULL, GetErrorFor(UNINITIALIZED_APP), "Error", MB_OK);   \
        exit(-1);                                                           \
    }

// Concantenate strings
#define STRCAT(str1, str2) (const char*)strcat(#str1, #str2)

#endif