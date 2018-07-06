#include <cstdio>
#include <cstring>
#include <Windows.h>
#include <shellscalingapi.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#pragma comment(lib, "Shcore.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

// Naming scheme (anything but windows style):
//    • pascalCase for variables
//    • full caps for constants
//    • if classes are used, they are CamelCase
//    • class method, if any, are CamelCase
//    • class fields, if any, are prefixed with 'p_' if they are private
//    • class fields, if any, that are public, do not need a prefix
//    • static fields are stagger_cased and can have shortcuts
//    • use windows specific holders to keep track of things
//    • use typedefs for non-windows related types
//    • do not use shorcut names, unless the name would be extremely long
//
// TODO:
//    • Decouple windowing code into it's own file and functions for convenience
//    • Implement Direct2D text rendering
//      1. Create rendering factory
//      2. Collect all renderable fonts on system
//      3. Find a default text to render and set it as current
//    • Begin working on Staggered buffering (a.k.a. Emacs buffering)
//      1. Break into class of it's own to handle all input buffering
//      2. Start loading in data from a file and displaying it
//      3. Enable text rendering to transform the file contents to readable text
//      4. Transpose the buffering algorithm to text space, so that wherever the
//          the cursor is, the buffer is there upon typing.
//    • Start making the scene look like an actual text editor xd
//      1. Register a menu with the editor window
//      2. Start designing the layout
//        i.   Design the number panel
//        ii.  Design text fold region for editor folding
//        iii. Design the actual text editing area.
//      3. Provide MDI functionality
//        i.   Define what a panel is to the program; of which is a personal display
//              port for the file or text being displayed, these may also be known
//              as tabs
//        ii.  Allow tabs to be moved about, decoupled from the main window
//              essentially turning them into children
//        iii. Allow tabs to be opened side by side, one atop another, etc.
//        iv.  Provide hotkey, panel swapping, opening, and closing
//      4. Provide text highlighting
//        i.   Create a lexer which takes the contents of a file being rendered to
//              a panel, structure the data into tokens and provide specific
//              highlighting to those tokens via XML or JSON, or in app methods
//        ii.  Create a parser which stores and keeps track of currently registered
//              tokens for a file, but that also keeps a structure of these tokens
//              for hierarchical viewing that could be useful.
//      5. Provide more functional menu options
//        i.   Mainly taking care of editor layout options and methods as well as
//              editor folding, text highlighting, and others.
//      6. Provide project support and plugin support.

static IDWriteFactory*          dwriteFactory_ptr;
static IDWriteTextFormat*       dwriteTextFormat_ptr;
static ID2D1Factory*            d2dFactory_ptr;
static ID2D1HwndRenderTarget*   d2dRenderTarget_ptr;
static ID2D1SolidColorBrush*    d2dBrush_ptr;

// Handles all window events:
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// Window entrance:
int WINAPI WinMain(
    HINSTANCE hInstance,    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,        int nCmdShow
) {
    HRESULT result          =  0;
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

    if (result == 0) {
        MessageBox(NULL, "Failed to register class!", "Error", MB_OK);
        return -1;
    }

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

    // Create and store window temporarily:
    HWND window = CreateWindowExW(
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

    if (window == 0) {
        MessageBox(NULL, "Failed to create Window!", "Error", MB_OK);
        return -1;
    }

    // The Device Context of the window for referencing purposes:
    HDC deviceContext = GetDC(window);

    // Initialize Direct2D context and rendering pipeline:
    result = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &d2dFactory_ptr
    );

    if (SUCCEEDED(result)) {
        result = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dwriteFactory_ptr)
        );

        if (!SUCCEEDED(result)) {
            MessageBox(NULL, "Direct2D failed to create write factory.", "Error", MB_OK);
            return -1;
        }
    } else {
        MessageBox(NULL, "Direct2D failed to initialize.", "Error", MB_OK);
        return -1;
    }

    result = dwriteFactory_ptr->CreateTextFormat(
        L"Gabriola",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        72.0f,
        L"en-us",
        &dwriteTextFormat_ptr
    );

    if (!SUCCEEDED(result)) {
        MessageBox(NULL, "Failed to create Text Format", "Error", MB_OK);
        return -1;
    }

    result = dwriteTextFormat_ptr->SetTextAlignment(
        DWRITE_TEXT_ALIGNMENT_CENTER
    );

    if (!SUCCEEDED(result)) return -1;

    result =dwriteTextFormat_ptr->SetParagraphAlignment(
        DWRITE_PARAGRAPH_ALIGNMENT_CENTER
    );

    if (!SUCCEEDED(result)) return -1;

    // Collect system data and conform the window:
    int screenLength    = GetSystemMetrics(SM_CXSCREEN);
    int screenStature   = GetSystemMetrics(SM_CYSCREEN);

    // Set new size and position of Window before showing:
    SetWindowPos(
        window,
        NULL,
        screenLength    / 4,
        screenStature   / 4,
        screenLength    / 2,
        screenStature   / 2,
        SWP_NOREDRAW    // Does not need to redraw until UpdateWindow()
    );

    RECT clientRect;
    GetClientRect(window, &clientRect);

    // Get d2d1 size for client rendering target:
    D2D1_SIZE_U size = D2D1::SizeU(
        clientRect.right    - clientRect.left,
        clientRect.bottom   - clientRect.top
    );

    // Set Client rendering target:
    result = d2dFactory_ptr->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
            window,
            size
        ),
        &d2dRenderTarget_ptr
    );

    if (SUCCEEDED(result)) {
        result = d2dRenderTarget_ptr->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &d2dBrush_ptr
        );
    }

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    // Message holder:
    MSG msg;

    // Handle messages:
    while (GetMessageW(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        // Actually draw shit to window xd (FINALLY):
        d2dRenderTarget_ptr->BeginDraw();
        d2dRenderTarget_ptr->SetTransform(D2D1::IdentityMatrix());
        d2dRenderTarget_ptr->Clear(D2D1::ColorF(D2D1::ColorF::White));

        // Get monitor DPI:
        HMONITOR monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        UINT dpix, dpiy;

        GetDpiForMonitor(
            monitor,
            MDT_DEFAULT,
            &dpix, &dpiy
        );

        // Getting Direct2D rendering area:
        D2D1_RECT_F layoutRect = D2D1::RectF(
            (float)(clientRect.left / dpix),
            (float)(clientRect.top  / dpiy),
            (float)((clientRect.right   - clientRect.left)  / dpix),
            (float)((clientRect.bottom  - clientRect.top)   / dpiy)
        );

        // Drawing Text
        d2dRenderTarget_ptr->DrawText(
            L"Some really cool text to render",
            32,
            dwriteTextFormat_ptr,
            layoutRect,
            d2dBrush_ptr
        );

        d2dRenderTarget_ptr->EndDraw();

        SwapBuffers(deviceContext);
    }

    // Release rendering and brush contexts
    d2dRenderTarget_ptr->Release();
    d2dBrush_ptr->Release();
    d2dRenderTarget_ptr = NULL;
    d2dBrush_ptr = NULL;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
///////////                Windows Procedure                /////////////////
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    LRESULT result = 0;
    
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            result = DefWindowProcW(hwnd, msg, wparam, lparam);
            break;
    }

    return result;
}