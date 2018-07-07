#include "Window.h"
#include "initializations.h"

using namespace Keikou;

// Generates a HWND for the given Instance.
HWND GenerateWindowHandle(wstring name);

// Creates a default Pixel Format Descriptor for the device context.
PIXELFORMATDESCRIPTOR CreatePFD();

// Creates a new Window.
Win_ptr Window::GenerateWindow(wstring name) {
    Win_ptr window = new Window;
    window->handle = GenerateWindowHandle(name);

    return window;
}

// Returns this Window's handle.
const HWND& Window::GetWindowHandle() const { return this->handle; }

// Sets a new extended window style for this Window.
void Window::SetExtendedStyle(DWORD exStyle) const {
    SetWindowLongW(this->handle, GWL_EXSTYLE, exStyle);
    SetWindowPos(
        this->handle, HWND_TOP,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOREPOSITION | SWP_DRAWFRAME
    );
}

// Sets a new standard window style for this Window.
void Window::SetStandardStyle(DWORD style) const {
    SetWindowLongW(this->handle, GWL_STYLE, style);
    SetWindowPos(
        this->handle, HWND_TOP,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOREPOSITION | SWP_DRAWFRAME
    );
}

// Gets the extende window style for this Window.
const DWORD& Window::GetExtendedStyle() const {
    return GetWindowLongW(this->handle, GWL_EXSTYLE);
}

// Gets the standard style for this Window.
const DWORD& Window::GetStandardStyle() const {
    return GetWindowLongW(this->handle, GWL_STYLE);
}

// Gets the device context for this Window.
const HDC& Window::GetDeviceContext() const {
    return GetDC(this->handle);
}

HWND GenerateWindowHandle(wstring name) {
    // A place to store results:
    HRESULT result = 0;

    // Register a Pixel Format Descriptor for the rendering context:
    PIXELFORMATDESCRIPTOR pfd = CreatePFD();

    // The Window styles:
    DWORD exStyle   = WS_EX_ACCEPTFILES | WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;
    DWORD style     = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_TABSTOP;

    CheckSuccess(WINCREATE, result);

    return CreateWindowExW(
        exStyle,
        CLASSNAME,
        name,
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

// Designed to Register a default Window Class style for all windows in the application.
void Keikou::RegisterWindowClass(HINSTANCE hInstance) {
    HRESULT result = 0;

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
    windowClass.lpszClassName   = CLASSNAME;
    // windowClass.lpszMenuName    = NULL;
    windowClass.style           = CS_DBLCLKS | CS_HREDRAW | CS_OWNDC | CS_VREDRAW;

    // Register window class:
    result = RegisterClassExW(&windowClass);
    
    CheckSuccess(CLSREGCREATE, result);
}

PIXELFORMATDESCRIPTOR CreatePFD() {
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

    return pfd;
}
