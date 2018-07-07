#ifndef KEIKOU_STAGGERED_BUFFER_HEADER
#define KEIKOU_STAGGERED_BUFFER_HEADER

#include "main_include.h"

#define DEFAULT_GAP_SIZE        30

namespace Keikou {
    typedef struct StaggeredBuffer StaggeredBuffer, StagBuf, *StagBuf_ptr;

    // Handles variable length, random access text input.
    struct StaggeredBuffer final {
    private:
        char* currentDataPos;
        char* dataBegin;
        char* dataEnd;
        char* gapBegin;
        char* gapEnd;

        uint internalGapSize;

    private:
        int     CopyBytes   (char* destination, string source, uint length);
        void    ExpandBuffer(uint size = DEFAULT_GAP_SIZE);
        void    ExpandGap   (uint size = DEFAULT_GAP_SIZE);
        int     InitBuffer  (uint size = DEFAULT_GAP_SIZE);

    public:
        StaggeredBuffer (uint gapSize = DEFAULT_GAP_SIZE);
        StaggeredBuffer (FILE* file, uint gapSize = DEFAULT_GAP_SIZE);
        StaggeredBuffer (const StaggeredBuffer& buffer);
        ~StaggeredBuffer();

        uint    BufferSize              ()              const;
        uint    CurrentPositionOffset   ()              const;
        void    DeleteChars             (uint count);
        uint    GapSize                 ()              const;
        char    GetCharAtCurrentPosition();
        char    GetCharAtPosition       (uint offset);
        void    InsertChar              (const char ch);
        void    InsertString            (string str);
        void    MoveGapToCurrentPosition();
        char    NextChar                ();
        char    PreviousChar            ();
        void    PrintBuffer             ();
        void    PutChar                 (const char ch);
        void    ReplaceChar             (const char ch);
        int     SaveBufferToFile        (FILE* file, uint bytes);
        void    SetCurrentPosition      (uint offset);
    };
}

#endif