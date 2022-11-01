/*
첼린지 이름
첼린지 구분 아이디
모금액
참여한 사람
기간(시작시간, 마감시간)
*/
#ifndef __CHALLENGE__
#define __CHALLENGE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "struct.h"
#include "setting.h"
#include "function.h"

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

void createChallenge(char name[], unsigned int money, person * joined, time_t startAt, time_t endAt) {
    if (strlen(name) > 30) {
        printf("name length is too long!");
        return;
    } // 만약 첼린지 이름이 30글자가 넘는다면 리턴함
    challenge asdf;
    asdf.idx = 1; // 임시로 적어놓은거 이후에 함수를 추가해서 idx를 구분하거나 새로운 idx를 생성할것임
    asdf.name = name;
    asdf.money = money;
    asdf.startAt = startAt;
    asdf.endAt = endAt;
    
    
}

#endif