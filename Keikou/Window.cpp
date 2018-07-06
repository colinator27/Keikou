#include "Keikou.h"

HWND GenerateWindow(HINSTANCE hInstance) {
    // place to store results:
    HRESULT result;
    
    // Window class:
    WNDCLASSEXW windowClass = {0};

    // windowClass.cbClsExtra      = 0;
    windowClass.cbSize          = sizeof(windowClass);
    // windowClass.cbWndExtra      = 0;
    windowClass.hbrBackground   = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.hCursor         = LoadCursor(NULL, IDC_ARROW);
    windowClass.hIcon           = LoadIcon(NULL, IDI_APPLICATION);
    // windowClass.hIconSm         = 0;
    windowClass.hInstance       = hInstance;
    windowClass.lpfnWndProc     = WinProc;
    windowClass.lpszClassName   = L"SORA::KEIKOU";
    // windowClass.lpszMenuName    = NULL;
    windowClass.style           = CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW;

    // Register window class:
    result = RegisterClassExW(&windowClass);
    
    CheckSuccess(CLSREGCREATE, result);

    // Register an OpenGL rendering context:
    PIXELFORMATDESCRIPTOR pfd = {0};

    // pfd.bReserved = 0;
    pfd.cAccumAlphaBits = 8;
    pfd.cAccumBits      = 32;
    pfd.cAccumBlueBits  = 8;
    pfd.cAccumGreenBits = 8;
    pfd.cAccumRedBits   = 8;
    pfd.cAlphaBits      = 8;
    // pfd.cAlphaShift     = 0;
    // pfd.cAuxBuffers     = 0;
    pfd.cBlueBits       = 8;
    // pfd.cBlueShift      = 0;
    pfd.cColorBits      = 32;
    pfd.cDepthBits      = 24;
    pfd.cGreenBits      = 8;
    // pfd.cGreenShift     = 0;
    pfd.cRedBits        = 8;
    // pfd.cRedShift       = 0;
    pfd.cStencilBits    = 8;
    // pfd.dwDamageMask    = 0;
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    // pfd.dwLayerMask     = 0;
    // pfd.dwVisibleMask   = 0;
    pfd.iLayerType      = PFD_MAIN_PLANE;
    pfd.iPixelType      = PFD_TYPE_RGBA;
    pfd.nSize           = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion        = 1;

    // The Window styles:
    DWORD exStyle   = WS_EX_ACCEPTFILES | WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
    DWORD style     = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_TABSTOP;

    return CreateWindowExW(
        exStyle,
        L"SORA::KEIKOU",
        L"Keikou",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
}