#include "initializations.h"
#include "main_include.h"

sbyte       initialized = -1;
HINSTANCE   hInstance   = 0;

void Keikou::InitializeWindowClass() { Keikou::RegisterWindowClass(hInstance); }


void Keikou::InitializeKeikou(HINSTANCE instance) {
    hInstance = instance;
    initialized = 1;
    Keikou::InitializeWindowClass();
}