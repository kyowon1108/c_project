#include <stdio.h>
#include <time.h>

void MakeLog(int userIdx, char content[]);
char* GetISOTime(struct tm *t);


void MakeLog(int userIdx, char content[]) {
    FILE* fp = fopen("log.txt","a");
    char log[1024];
    struct tm *t;
    time_t timer;
    timer = time(NULL);    // ���� �ð��� �� ������ ���
    t = localtime(&timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ�
    char time[40];
    sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec
            );
    sprintf(log, "[%s] %d : %s\n", time, userIdx, content);
    fputs(log, fp);
    fclose(fp);
}

char* GetISOTime(struct tm *t) {
    static char s[20];
    sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec
            );
    return s;
}

int main() {
    int userIdx = 1;
    MakeLog(userIdx, "asdf");
    return 0;
}