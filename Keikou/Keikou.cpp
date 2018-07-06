#include "Keikou.h"

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
static IDWriteFontCollection*   dwriteFontCollection_ptr;
static IDWriteFontFamily*       dwriteFontFamily_ptr;
static IDWriteLocalizedStrings* dwriteFamilyNames_ptr;
static ID2D1Factory*            d2dFactory_ptr;
static ID2D1HwndRenderTarget*   d2dRenderTarget_ptr;
static ID2D1SolidColorBrush*    d2dBrush_ptr;

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

    CheckSuccess(WINCREATE, result);

    // The Device Context of the window for referencing purposes:
    HDC deviceContext = GetDC(window);

    // Initialize Direct2D context and rendering pipeline:
    result = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &d2dFactory_ptr
    );

    CheckSuccess(D2DFCREATE, result);

    result = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&dwriteFactory_ptr)
    );

    CheckSuccess(DWFCREATE, result);

    // Default Windows font:
    const wchar_t defaultFontName[8] = L"Calibri";

    // Get System font information:
    // UINT32 familyCount = 0;
    // result = dwriteFactory_ptr->GetSystemFontCollection(&dwriteFontCollection_ptr);
    // 
    // if (!SUCCEEDED(result)) {
    //     MessageBox(NULL, "Could not collect System fonts.", "Error", MB_OK);
    //     return -1;
    // }
    // 
    // wchar_t localName[LOCALE_NAME_MAX_LENGTH];
    // 
    // familyCount = dwriteFontCollection_ptr->GetFontFamilyCount();
    // 
    //  if (familyCount == 0) {
    //      MessageBox(NULL, "No families exist!", "Fatal Error", MB_OK);
    //      return -1;
    //  }
    // 
    // int defaultLocaleSuccess = GetUserDefaultLocaleName(localName, LOCALE_NAME_MAX_LENGTH);
    // 
    // Pick a default font to display:
    // UINT32  index   = 0, length = 0;
    // BOOL    exists  = 0;
    // 
    // for (unsigned int i = 0; i < familyCount; i++) {
    //     result = dwriteFontCollection_ptr->GetFontFamily(i, &dwriteFontFamily_ptr);
    // 
    //     if (!SUCCEEDED(result)) {
    //         MessageBox(NULL, "Failed to get font families", "Error", MB_OK);
    //         return -1;
    //     }
    // 
    //     result = dwriteFontFamily_ptr->GetFamilyNames(&dwriteFamilyNames_ptr);
    // 
    //     if (!SUCCEEDED(result)) {
    //         MessageBox(NULL, "Failed to get family names", "Error", MB_OK);
    //         return -1;
    //     }
    // 
    //     while (exists) {
    //         if (defaultLocaleSuccess) {
    //             result = dwriteFamilyNames_ptr->FindLocaleName(localName, &index, &exists);
    //         }
    //         
    //         // Try one more time in English for English only
    //         if (SUCCEEDED(result) && !exists) {
    //             result = dwriteFamilyNames_ptr->FindLocaleName(L"en-us", &index, &exists);
    //         }
    // 
    //         if (!SUCCEEDED(result)) {
    //             MessageBox(NULL, "Could not find locales for families.", "Error", MB_OK);
    //             return -1;
    //         }
    // 
    //         if (!exists) { index = 0; break; }
    // 
    //         result = dwriteFamilyNames_ptr->GetStringLength(index, &length);
    //         wchar_t* name = new wchar_t[length + 1];
    // 
    //         if (name == NULL) result = E_OUTOFMEMORY;
    // 
    //         if (!SUCCEEDED(result)) {
    //             MessageBox(NULL, "No moar memry", "Err", MB_OK);
    //             return -1;
    //         }
    // 
    //         result = dwriteFamilyNames_ptr->GetString(index, name, length + 1);
    // 
    //         if (!SUCCEEDED(result)) {
    //             MessageBox(NULL, "Some F*cked up sh*t happened here.", "Error", MB_OK);
    //             return -1;
    //         }
    //     }
    // }

    // Create Text format to display:
    result = dwriteFactory_ptr->CreateTextFormat(
        defaultFontName,
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f,
        L"en-us",
        &dwriteTextFormat_ptr
    );

    CheckSuccess(TEXTFORMAT | CREATE, result);

    result = dwriteTextFormat_ptr->SetTextAlignment(
        DWRITE_TEXT_ALIGNMENT_CENTER
    );

    CheckSuccess(TEXTALIGNMENT, result);

    result =dwriteTextFormat_ptr->SetParagraphAlignment(
        DWRITE_PARAGRAPH_ALIGNMENT_CENTER
    );

    CheckSuccess(PARAGRAPH_ALIGNMENT, result);

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

    CheckSuccess(SCBCREATE, result);

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

        // Getting Direct2D rendering area:
        D2D1_SIZE_F renderTargetSize = d2dRenderTarget_ptr->GetSize();

        // Rendering area:
        D2D1_RECT_F layoutRect = D2D1::RectF(
            0,
            0,
            renderTargetSize.width,
            renderTargetSize.height
        );

        // size of font name:
        size_t fontNameLength = sizeof(defaultFontName) / sizeof(defaultFontName[0]);

        // Drawing Text:
        d2dRenderTarget_ptr->DrawText(
            defaultFontName,
            fontNameLength,
            dwriteTextFormat_ptr,
            layoutRect,
            d2dBrush_ptr
        );

        // Stop drawing to back buffer:
        d2dRenderTarget_ptr->EndDraw();

        // swap front and back buffers to display text:
        SwapBuffers(deviceContext);
    }

    // Release rendering and brush contexts:
    SafeRelease(&d2dRenderTarget_ptr);
    SafeRelease(&d2dBrush_ptr);

    return 0;
}

// Implementations of things needed.
template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
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