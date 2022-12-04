#ifndef __STRUCT__
#define __STRUCT__

typedef struct User {
    int userIdx;
    int userName;
} user;

typedef struct Friend {
    int userIdx;
    int friendIdx;
} friend;

typedef struct Plan {
    int planIdx;
    int userIdx;
    char * planName[30];
    char * explain[100];
    int openlevel;
    char * createdAt[20];
    char * endAt[20];
} plan;

typedef struct Plandetail {
    int detailIdx;
    int planIdx;
    char startedAt[20];
    char endAt[20];
    char where[30];
    char createdAt[20];
} plandetail;

typedef struct Planreview {
    int reviewIdx;
    int planIdx;
    int userIdx;
    char content[100];
    int score;
    char createdAt[20];
} planreview;

typedef struct Deposit {
    int depositIdx;
    char depositName[20];
    int money;
    char createdAt[20];
    char endAt[20];
} deposit;

typedef struct Deposituser {
    int depositIdx;
    int userIdx;
} deposituser;

#endif
