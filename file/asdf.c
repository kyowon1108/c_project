#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "struct.h"
#include "function.h"
#include "setting.h"

int main() {
    setColor(DEFAULT_COLOR); // default color로 출력을 시작함
    bookmark res[5] = {{1, Aqua}, {3, Green}, {7, Red}, {20, Gray}, {18, Yellow}};
    int len = sizeof(res) / sizeof(bookmark);
    printCalendar(8, 1, res, len); // 8월달의 일요일이 1일인 달력을 출력
    return 0;
}