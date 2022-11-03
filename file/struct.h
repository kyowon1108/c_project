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

typedef struct PERSON {
    int idx;
    char name[10];
} person;

typedef struct CHALLENGE {
    int idx;
    char * name;
    unsigned int money;
    person joined[30];
    time_t startAt;
    time_t endAt;
} challenge;

#endif