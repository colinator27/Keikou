#include "main_include.h"

#define CODE(x)             "code:" CODEFOR(x)
#define CREATIONFAILED(x)   "Failed to create " x ", "
#define FAILEDPROC(x)       "Failed to do procedure \"" x "\""

#define UNKNOWN_ERR         "Unknown Error; What did you do, what did you touch?! We told you not to touch things you shouldn't."

// definition for GetErrorFor(uint)
string GetErrorFor(uint action) {
    if (action == UNINITIALIZED_APP)
        return "App was not properly initialized, " CODE(UNINITIALIZED_APP);

    switch (action & ~CREATE) {
        // When class registry fails.
        case CLASS_REGISTRATION:
            return CREATIONFAILED("class registration")
                    CODE(CLASS_REGISTRATION);
        
        // When Window creation fails.
        case WINDOW:
            return CREATIONFAILED("window")
                    CODE(action);
        
        // When Direct2D factory fails to create.
        case D2DFACTORY:
            return CREATIONFAILED("Direct2D factory")
                    CODE(action);

        // DirectWrite factory fails to create.
        case DWRITEFACTORY:
            return CREATIONFAILED("DirectWrite factory")
                    CODE(action);

        // Text format for DirectWrite failed to create.
        case TEXTFORMAT:
            return CREATIONFAILED("Text Format")
                    CODE(action);

        // The window render target failed to create.
        case RENDER_TARGET:
            return CREATIONFAILED("window render target")
                    CODE(action);

        // When solid color brush for Direct2D fails to create.
        case SOLID_COLOR_BRUSH:
            return CREATIONFAILED("solid color brush")
                    CODE(action);
    }

    switch (action) {
        // Text alignment failed.
        case TEXTALIGNMENT:
            return FAILEDPROC("text alignment")
                    CODE(action);

        // Paragraph alignment failed.
        case PARAGRAPH_ALIGNMENT:
            return FAILEDPROC("paragraph alignment")
                    CODE(action);
    }

    return UNKNOWN_ERR;
}