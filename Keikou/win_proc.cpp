#include "Keikou.h"

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    LRESULT result = 0;
    
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE: {
            // RECT clientRect;
            // GetClientRect(hwnd, &clientRect);
            // 
            // // Get d2d1 size for client rendering target:
            // D2D1_SIZE_U size = D2D1::SizeU(
            //     clientRect.right    - clientRect.left,
            //     clientRect.bottom   - clientRect.top
            // );
            // 
            // if (d2dRenderTarget_ptr) SafeRelease(&d2dRenderTarget_ptr);
            // 
            // // Set Client rendering target:
            // result = d2dFactory_ptr->CreateHwndRenderTarget(
            //     D2D1::RenderTargetProperties(),
            //     D2D1::HwndRenderTargetProperties(
            //         hwnd,
            //         size
            //     ),
            //     &d2dRenderTarget_ptr
            // );
            // 
            // CheckSuccess(HWNDRTCREATE, result);
            // 
            // result = d2dRenderTarget_ptr->CreateSolidColorBrush(
            //     D2D1::ColorF(D2D1::ColorF::Black),
            //     &d2dBrush_ptr
            // );
            // 
            // CheckSuccess(SCBCREATE, result);
            break;
        }
        case WM_PAINT: {
            break;
        }
        default:
            result = DefWindowProcW(hwnd, msg, wparam, lparam);
            break;
    }

    return result;
}