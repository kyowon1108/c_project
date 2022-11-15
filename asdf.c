#include <stdio.h>
#include <time.h>

int IsLeafYear(int year) {
	if(year % 400 == 0)
		return 1;
	if((year % 100 != 0) && (year % 4 == 0))
		return 1;
	return 0;
}

int getDay(int year, int month) {
    if (month < 1 || month > 12) return 0;
    int day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4) == 0 && (year % 100) != 0 || (year % 400) == 0) day[1] = 29;
    return day[month - 1];
}

int getStartDay(int year, int month) {
	int past = 0;
	for(int y = 1; y < year; ++y)
		past = past + 365 + IsLeafYear(y);
	for(int m = 1; m < month; ++m)
		past = past + getDay(year, m);
	return (1 + past) % 7;
}

void printCalendar(int year, int month) {
    int day = getDay(year, month);
    int start_day = getStartDay(year, month);
    printf("           [ %d년 %d월 ]            \n\n", year, month);
    printf("   일   월   화   수   목   금   토\n\n");
    for(int i = 0; i < start_day; ++i) printf("     ");
    int count = start_day; // 7일이 찰때마다 주를 바꿔줘야 하기 때문에 count를 사용
    for (int j = 1; j <= day; ++j) {
        int check = 0;
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

int main() {
    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    printCalendar(t->tm_year + 1900, t->tm_mon + 1);
    return 0;

}