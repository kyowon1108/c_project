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
setColor : Ư�� ����� �ؽ�Ʈ�� ����ϱ� ���� �Լ�
    colorNum : ���� ����

textColor : �ؽ�Ʈ�� ������ ������ �Լ�
    �� colorNum : ���� ����
    �� text : ������ ���� ����

printBlank : �־��� ����ŭ ������ ����ϴ� �Լ�
    �� len : ����� ������ ��

getDay : ���� �ϼ��� ����ϴ� �Լ�
    �� year : ���� ���п� �⵵
    �� month : ����� ��

printCalendar : Ư�� ���� �޷��� ����ϴ� �Լ�
    �� month : ����� ��
    �� start_day : 1���� �Ǵ� ���� (1 : �Ͽ���, 2 : ������, 3 : ȭ����, 4 : ������, 5 : �����, 6 : �ݿ���, 7 : �����)
    �� mark : ������ ĥ�� ��¥ ( ����ü )
    �� len : mark�� ����
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
    printf("           [ %d�� %d�� ]            \n\n", year, month);
    printf("   ��   ��   ȭ   ��   ��   ��   ��\n\n");
    for(int i = 0; i < start_day - 1; ++i) printBlank(5);
    int count = 0; // 7���� �������� �ָ� �ٲ���� �ϱ� ������ count�� ���
    for (int j = 1; j <= day; ++j) {
        int check = 0;
        for (int k = 0; k < len; ++k) {
            if (mark[k].day == j) {
                printBlank((j > 9) ? 3 : 4); // ������ ĥ�� ��¥�� 2�ڸ��� 3�� �����̽�, 1�ڸ��� 4�� �����̽�
                char buff[3]; // int���� ��¥�� char ������ �ٲ���
                sprintf(buff, "%d", j);
                textColor(mark[k].color, buff); // Ư�� ��¥�� Ư�� ����� �ٲ���
                count++, check = 1;
            }
        }
        if (check == 0) {
            printf("%5d", j);
            count++;
        }
        if (count == 7) { // ī��Ʈ�� 7�� �Ǵ� ���� �����ַ� �Ѿ
            printf("\n\n");
            count = 0;
        }
    }
}

#endif