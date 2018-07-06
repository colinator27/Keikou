#pragma once

#define CODEFOR(x) #x

#define CREATE              0x00000001
#define CLASS_REGISTRATION  0x10001000
#define CLSREGCREATE        CLASS_REGISTRATION | CREATE
#define WINDOW              0x10002000
#define WINCREATE           WINDOW | CREATE
#define D2DFACTORY          0x10003000
#define D2DFCREATE          D2DFACTORY | CREATE
#define DWRITEFACTORY       0x10004000
#define DWFCREATE           DWRITEFACTORY | CREATE
#define TEXTFORMAT          0x10005000
#define TEXTALIGNMENT       0x10006000
#define PARAGRAPH_ALIGNMENT 0x10007000
#define SOLID_COLOR_BRUSH   0x10008000
#define SCBCREATE           SOLID_COLOR_BRUSH | CREATE