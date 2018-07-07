#ifndef INITIALIZATIONS_HEADER
#define INITIALIZATIONS_HEADER

#include "main_include.h"

namespace Keikou {
    // Registers the default Window Class for this application.
    void RegisterWindowClass(HINSTANCE hInstance);

    // Initializes the Window class, can only be initialized once.
    void InitializeWindowClass();

    // Initializes everything necessary to run Keikou.
    void InitializeKeikou(HINSTANCE instance);
}

#endif