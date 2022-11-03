#ifndef __FUNCTION__
#define __FUNCTION__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "struct.h"
#include "setting.h"

void setColor(int colorNum);
void textColor(int colorNum, const char * text);
void printBlank(int len);
int getDay(int year, int month);
void printCalendar(int year, int month, int start_day, bookmark * mark, int len);
/*
setColor : 특정 색깔로 텍스트를 출력하기 위한 함수
    colorNum : 입힐 색깔

textColor : 텍스트에 색깔을 입히는 함수
    ㄴ colorNum : 입힐 색깔
    ㄴ text : 색깔을 입힐 문자

printBlank : 주어진 수만큼 공백을 출력하는 함수
    ㄴ len : 출력할 공백의 수

getDay : 월별 일수를 계산하는 함수
    ㄴ year : 윤년 구분용 년도
    ㄴ month : 계산할 월

printCalendar : 특정 달의 달력을 출력하는 함수
    ㄴ month : 출력할 달
    ㄴ start_day : 1일이 되는 요일 (1 : 일요일, 2 : 월요일, 3 : 화요일, 4 : 수요일, 5 : 목요일, 6 : 금요일, 7 : 토요일)
    ㄴ mark : 색깔을 칠할 날짜 ( 구조체 )
    ㄴ len : mark의 개수
*/

void setColor(int colorNum) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorNum);
}

void textColor(int colorNum, const char * text) {
	setColor(colorNum);
    printf("%s", text);
    setColor(DEFAULT_COLOR);
}

void printBlank(int len) {
    for (int i = 0; i < len; ++i) printf(" ");
}



int getDay(int year, int month) {
    if (month < 1 || month > 12) return 0;
    int day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4) == 0 && (year % 100) != 0 || (year % 400) == 0) day[1] = 29;
    return day[month - 1];
}

void printCalendar(int year, int month, int start_day, bookmark * mark, int len) {
    int day = getDay(year, month);
    if (start_day < 1 || start_day > 7) return;
    printf("           [ %d년 %d월 ]            \n\n", year, month);
    printf("   일   월   화   수   목   금   토\n\n");
    for(int i = 0; i < start_day - 1; ++i) printBlank(5);
    int count = 0; // 7일이 찰때마다 주를 바꿔줘야 하기 때문에 count를 사용
    for (int j = 1; j <= day; ++j) {
        int check = 0;
        for (int k = 0; k < len; ++k) {
            if (mark[k].day == j) {
                printBlank((j > 9) ? 3 : 4); // 색깔을 칠할 날짜가 2자리면 3번 스페이스, 1자리면 4번 스페이스
                char buff[3]; // int형인 날짜를 char 형으로 바꿔줌
                sprintf(buff, "%d", j);
                textColor(mark[k].color, buff); // 특정 날짜를 특정 색깔로 바꿔줌
                count++, check = 1;
            }
        }
        if (check == 0) {
            printf("%5d", j);
            count++;
        }
        if (count == 7) { // 카운트가 7이 되는 순간 다음주로 넘어감
            printf("\n\n");
            count = 0;
        }
    }
}

#endif