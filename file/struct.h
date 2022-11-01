#ifndef __STRUCT__
#define __STRUCT__

typedef enum ColorType {
    Black, Blue, Green, Aqua, Red, Purple, Yellow, White, Gray, LightBlue,
    LightGreen, LightAqua, LightRed, LightPurple, LightYellow, BrightWhite
} COLOR;

typedef struct Bookmark {
    int day;
    COLOR color;
} bookmark;

#endif