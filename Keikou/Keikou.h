#ifndef KEIKOU_MAIN_HEADER
#define KEIKOU_MAIN_HEADER

#include "main_include.h"
#include "util.h"
#include "Window.h"
#include "initializations.h"

// Default Windows font:
wstring defaultFontName = L"Calibri";

// size of font name:
size_t fontNameLength = wcslen(defaultFontName);

// Static references, will be apart of classes soon.
static HWND                     window;
static HDC                      deviceContext;
static IDWriteFactory*          dwriteFactory_ptr;
static IDWriteLocalizedStrings* dwriteFamilyNames_ptr;
static IDWriteFontCollection*   dwriteFontCollection_ptr;
static IDWriteFontFamily*       dwriteFontFamily_ptr;
static IDWriteTextFormat*       dwriteTextFormat_ptr;
static ID2D1Factory*            d2dFactory_ptr;
static ID2D1HwndRenderTarget*   d2dRenderTarget_ptr;
static ID2D1SolidColorBrush*    d2dBrush_ptr;

#endif