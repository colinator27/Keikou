#ifndef KEIKOU_WINDOW_HEADER
#define KEIKOU_WINDOW_HEADER

#include "main_include.h"

#define CLASSNAME L"SORA::KEIKOU"

typedef PIXELFORMATDESCRIPTOR   PFD;
typedef WNDCLASSEXW             WindowClass;

// Handles all window events:
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// The Keikou base namespace.
namespace Keikou {
    // Forward reference.
    typedef struct Window Window, *Win_ptr;

    // Class to handle all Window related things.
    struct Window final {
    private:
        HWND handle;

    private:
        Window() : handle(0) { }
        ~Window() { }

    public:
        // Generates a window.
        static Win_ptr GenerateWindow(wstring name);
        
        // Gets the handle for this Window.
        const HWND& GetWindowHandle() const;

        // Sets the extended style of this Window.
        void SetExtendedStyle(DWORD exStyle) const;

        // Sets the standard style of this Window.
        void SetStandardStyle(DWORD style) const;

        // Gets the extended style of this Window.
        const DWORD& GetExtendedStyle() const;

        // Gets the standard style of this Window.
        const DWORD& GetStandardStyle() const;

        // Gets this Window's device context.
        const HDC& GetDeviceContext() const;
    };
}

#endif