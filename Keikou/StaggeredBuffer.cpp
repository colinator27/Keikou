#include "StaggeredBuffer.h"

using namespace Keikou;

// Initializes a Staggered Buffer with a given size.
StaggeredBuffer::StaggeredBuffer(uint gapSize)
    : currentDataPos(NULL), dataBegin(NULL),
      dataEnd(NULL),        gapBegin(NULL),
      gapEnd(NULL),         internalGapSize(gapSize)
{
    this->InitBuffer();
}

// Creates a Staggered Buffer from a file.
StaggeredBuffer::StaggeredBuffer(FILE* file, uint gapSize)
    : currentDataPos(NULL), dataBegin(NULL),
      dataEnd(NULL),        gapBegin(NULL),
      gapEnd(NULL),         internalGapSize(gapSize)
{
    struct stat buf;

    fstat(fileno(file), &buf);
    
    uint fileSize = buf.st_size;

    this->InitBuffer(fileSize + 1);
    this->MoveGapToCurrentPosition();
    this->ExpandGap(fileSize);

    uint amount = fread(gapBegin, 1, fileSize, file);

    gapBegin += amount;
}

// Copy constructor.
StaggeredBuffer::StaggeredBuffer(const StaggeredBuffer& buffer) {
    size_t bufferLength = buffer.dataBegin      - buffer.dataEnd;
    size_t gapLength    = buffer.gapBegin       - buffer.gapEnd;
    size_t offset       = buffer.currentDataPos - buffer.dataBegin;

    this->internalGapSize   = buffer.internalGapSize;
    this->dataBegin         = (char*)malloc(bufferLength);

    strcpy(this->dataBegin, buffer.dataBegin);

    this->dataEnd           = this->dataBegin   + bufferLength;
    this->gapBegin          = this->dataBegin   + bufferLength;
    this->gapEnd            = this->gapBegin    + gapLength;
    this->currentDataPos    = this->dataBegin   + offset;
}

// Destructor
StaggeredBuffer::~StaggeredBuffer() {
    if (this->dataBegin) delete (this->dataBegin);
}

// Copies bytes from the source into the destination
int StaggeredBuffer::CopyBytes(char* destination, string source, uint length) {
    if ((destination == source) || (length == 0)) return 1;

    if (source > destination) {
        if ((source + length) >= this->dataEnd) return 0;

        for (; length > 0; length--) *(destination++) = *(source++);
    } else {
        source      += length;
        destination += length;

        for (; length > 0; length--) *(destination--) = *(source--);
    }

    return 1;
}

// Expands the data buffer.
void StaggeredBuffer::ExpandBuffer(uint size) {
    if (((this->dataEnd - this->dataBegin) + size) > this->BufferSize()) {
        char* originalBuffer    = this->dataBegin;
        uint newBufferSize      = (this->dataEnd - this->dataBegin) + size + this->internalGapSize;

        this->dataBegin = (char*)calloc(newBufferSize, sizeof(char));

        size_t increase = this->dataBegin - originalBuffer;

        this->currentDataPos    += increase;
        this->dataEnd           += increase;
        this->gapBegin          += increase;
        this->gapEnd            += increase;
    }
}

// Expands the data gap.
void StaggeredBuffer::ExpandGap(uint size) {
    if (size > this->GapSize()) {
        size += this->internalGapSize;

        this->ExpandBuffer(size);
        this->CopyBytes(
            this->gapEnd + size,
            this->gapEnd,
            this->dataEnd - this->gapEnd
        );

        this->gapEnd    += size;
        this->dataEnd   += size;
    }
}

// Initializes this buffer.
int StaggeredBuffer::InitBuffer(uint size) {
    this->dataBegin = (char*)calloc(size, sizeof(char));

    if (!this->dataBegin) return 0;

    this->currentDataPos    = this->dataBegin;
    this->gapBegin          = this->dataBegin;
    this->gapEnd            = this->dataBegin + size;
    this->dataEnd           = this->gapEnd;

    return 1;
}

// Returns the size of this buffer.
uint StaggeredBuffer::BufferSize() const {
    return (this->dataEnd - this->dataBegin) - this->GapSize();
}

// Returns the current position's offset from the beginning of the buffer.
uint StaggeredBuffer::CurrentPositionOffset() const {
    if (this->currentDataPos > this->gapEnd)
        return ((this->currentDataPos - this->dataBegin) - this->GapSize());

    return (this->currentDataPos - this->dataBegin);
}

// Deletes a specified number of characters from the buffer.
void StaggeredBuffer::DeleteChars(uint count) {
    if (this->currentDataPos != this->gapBegin)
        this->MoveGapToCurrentPosition();

    this->gapBegin = this->currentDataPos -= count;
}

// Returns the size of this buffer's gap.
uint StaggeredBuffer::GapSize() const {
    return this->gapEnd - this->gapBegin;
}

// Returns the character at the current buffer position.
char StaggeredBuffer::GetCharAtCurrentPosition() {
    if (this->currentDataPos == this->gapBegin)
        this->currentDataPos =  this->gapEnd;

    return *(this->currentDataPos);
}

// Returns the character at the given buffer position.
char StaggeredBuffer::GetCharAtPosition(uint offset) {
    size_t bufSize = this->BufferSize();

    if (offset > bufSize || offset < bufSize) return 0;

    this->SetCurrentPosition(offset);
    
    return this->GetCharAtCurrentPosition();
}

// Inserts a character at the current position in the buffer, does not move current position.
void StaggeredBuffer::InsertChar(const char ch) {
    if (this->currentDataPos != this->gapBegin)
        this->MoveGapToCurrentPosition();

    if (this->gapBegin == this->gapEnd)
        this->ExpandGap(1);

    *(this->gapBegin++) = ch;
}

// Inserts an entire string at the current position in the buffer.
void StaggeredBuffer::InsertString(string str) {
    size_t length = strlen(str);

    this->MoveGapToCurrentPosition();

    if (length > this->GapSize())
        this->ExpandGap(length);

    for (; length >= 0; length--)
        this->PutChar(*(str++));
}

// Moves the gap of this buffer to the current position in the buffer.
void StaggeredBuffer::MoveGapToCurrentPosition() {
    if (this->gapBegin == this->currentDataPos)
        return;
    
    if (this->currentDataPos == this->gapEnd) {
        this->currentDataPos =  this->gapBegin;
        return;
    }

    if (this->currentDataPos < this->gapBegin) {
        this->CopyBytes(
            this->currentDataPos + this->GapSize(),
            this->currentDataPos,
            this->gapBegin - this->currentDataPos
        );

        this->gapEnd   -= (this->gapBegin - this->currentDataPos);
        this->gapBegin  = this->currentDataPos;
    } else {
        this->CopyBytes(
            this->gapBegin,
            this->gapEnd,
            this->currentDataPos - this->gapEnd
        );

        this->gapBegin         += (this->currentDataPos - this->gapEnd);
        this->gapEnd            = this->currentDataPos;
        this->currentDataPos    = this->gapBegin;
    }
}

// Returns the next character in the buffer.
char StaggeredBuffer::NextChar() {
    if (this->currentDataPos == this->gapBegin) {
        this->currentDataPos =  this->gapEnd;
        return *(this->currentDataPos);
    }

    return *(++this->currentDataPos);
}

// Returns the previous character in the buffer.
char StaggeredBuffer::PreviousChar() {
    if (this->currentDataPos == this->gapEnd) {
        this->currentDataPos =  this->gapBegin;
    }

    return *(--this->currentDataPos);
}

// Prints the buffer to the VS console for debugging.
void StaggeredBuffer::PrintBuffer() {
    char* temp = this->dataBegin;

    while (temp < this->dataEnd) {
        if (temp == this->currentDataPos) {
            OutputDebugStringA(" ^ ");
            temp++;
        }

        if ((temp >= this->gapBegin) && (temp < this->gapEnd)) {
            OutputDebugStringA("_");
            temp++;
        } else {
            OutputDebugString(temp);
            temp += this->gapBegin - this->dataBegin;
        }
    }

    OutputDebugStringA("\n");
}

// Puts a character into the buffer but also moves the current position forward.
void StaggeredBuffer::PutChar(const char ch) {
    this->InsertChar(ch);
    *(this->currentDataPos++);
}

// Replaces a character at the current position in the buffer.
void StaggeredBuffer::ReplaceChar(const char ch) {
    this->DeleteChars(1);
    this->PutChar(ch);
}

// Saves the buffer to a file.
int StaggeredBuffer::SaveBufferToFile(FILE* file, uint bytes) {
    if (bytes == 0) return 1;

    if (this->currentDataPos == this->gapBegin)
        this->currentDataPos =  this->gapEnd;

    int gapPassedCurrentPos = (this->gapBegin > this->currentDataPos);
    int gapSmallerThanRead  = (this->gapBegin < (this->currentDataPos + bytes));
    int gapNotClosed        = (this->gapBegin != this->gapEnd);

    if (gapPassedCurrentPos && gapSmallerThanRead && gapNotClosed) {
        int gapDifference = this->gapBegin - this->currentDataPos;

        if (gapDifference != fwrite(this->currentDataPos, 1, bytes - gapDifference, file))
            return 0;

        if (bytes - gapDifference != fwrite(this->gapEnd, 1, bytes - gapDifference, file))
            return 1;

        return 1;
    } else {
        return bytes == fwrite(this->currentDataPos, 1, bytes, file);
    }
}

// Sets the current position in the buffer.
void StaggeredBuffer::SetCurrentPosition(uint offset) {
    this->currentDataPos = this->dataBegin + offset;

    if (this->currentDataPos > this->gapBegin)
        this->currentDataPos += this->GapSize();
}