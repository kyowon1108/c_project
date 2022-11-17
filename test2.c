#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "/usr/include/mysql/mysql.h"
#include "test2_struct.h"

#define HOST "localhost"
#define USER "kapr"
#define PASSWORD "kyowon1108"
#define DB "PFP"
#define CHOP(x) x[strlen(x) - 1] = ' '

MYSQL *connection = NULL, conn;
MYSQL_RES *sql_result;
MYSQL_ROW sql_row;
int query_stat;
char query[255]; // 입력할 mysql 쿼리문이 들어갈 변수

// [ 달력 출력 관련 함수]
int IsLeafYear(int year); // 윤년인지 체크
int getDay(int year, int month); // 월별 날짜 수가 몇인지 리턴
int getStartDay(int year, int month); // 달력에서 1일인 요일 리턴
void printCalendar(int year, int month); // 달력 출력

// [ 유저 관련 함수 ]
int SignUp(char name[]); // 회원가입
int CheckLastUserIdx();
int CheckUser(int userIdx);
int printUser(); // 유저 리스트 출력
int MakeFriend(int userIdx, int friendIdx); // 친구 추가
int IsFriend(int userIdx, int friendIdx); // 친구인지 확인

// [ 계획 관련 함수 ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // 계획 생성
int CheckLastPlanIdx();
int GetPlanLen(int userIdx); // 유저가 생성한 계획의 수 리턴
int GetPlanIdx(int userIdx, char ** arr); // 유저가 생성한 계획의 인덱스를 arr에 저장
int GetPlan(int userIdx); // 유저가 생성한 계획 리스트 출력
int GetFriendPlan(int userIdx, int friendIdx); // 친구가 생성한 계획 리스트 출력
void GetDayPlan(int * arr, int userIdx, char date[]); // 특정 날의 계획 리스트 출력 및 인덱스 리턴
void DeletePlan(int userIdx, int planIdx); // 계획 삭제
void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx); // 계획 수정

// [ 세부 계획 관련 함수 ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 생성
int CheckLastDetailIdx();
int GetPlanDetailLen(int planIdx); // 유저가 생성한 계획 디테일의 수 리턴
int GetPlanDetail(int planIdx); // 유저가 생성한 계획 디테일 리스트 출력
void DeletePlandetail(int detailIdx); // 계획 디테일 삭제
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 수정

// [ 계획 리뷰 관련 함수 ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score);
int GetPlanReview(int planreviewIdx);
void DeletePlanReview(int planreviewIdx);
void ModifyPlanReview(int planreviewIdx, char content[], int score);

// [ 세부 계획 리뷰 관련 함수]
int MakeDetailReview(int detailIdx, int userIdx, char content[], int score);
int GetDetailReview(int detailreviewIdx);
void DeleteDetailReview(int detailreviewIdx);
void ModifyDetailReview(int detailreviewIdx, char content[], int score);


int userIdx;

int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // 서버와 통신 시작

    if (connection == NULL) {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }


    while (1) {
        printf("| 1. 로그인 | 2. 회원가입 |\n실행할 번호 : ");
        int func, a;
        scanf("%d", &func);
        switch (func) {
            case 1 :
                printf("로그인 할 userIdx : ");
                scanf("%d", &a);
                if (CheckUser(a)) {
                    userIdx = a;
                    printf("%d로 로그인했습니다.\n", userIdx);
                    break;
                }
                else {
                    printf("해당 아이디는 존재하지 않습니다. userIdx를 확인해주세요.\n");
                    continue;
                }

            case 2 :
                printf("이름을 입력해주세요 : ");
                char name[30];
                scanf("%s", name);
                int sign = SignUp(name);
                if (sign) {
                    int idx = CheckLastUserIdx();
                    printf("회원가입을 성공했습니다.\nuserIdx : %d (userIdx로 로그인해주세요)\n", idx);
                    continue;
                } else {
                    printf("오류가 발생했습니다.");
                    continue;
                }

            default :
                printf("번호를 확인해주세요.\n");
                continue;
        }
        break;
    }

    printf("\n\n");

    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    printCalendar(t->tm_year + 1900, t->tm_mon + 1);

    printf("\n\n");
    int isRoof = 1;
    int function;
    while(isRoof) {
        printf("| 1 : 계획 추가 | 2 : 계획 삭제 | 3 : 계획 수정 | 4 : 계획 확인 |\n| 5 : 평가 확인 | 6 : 친구 계획 확인 | 7 : 친구 추가 | 8 : 종료 |\n 실행할 번호 : ");
        scanf("%d", &function);
        if (function == 8) {
            printf("프로그램을 종료합니다.");
            break;
        }
        switch (function) {
            case 1 :
                printf("계획 추가를 선택했습니다.\n\n");
                char planName[20], explain[1024], endAt[20];
                int openLevel;
                printf("계획명을 입력해주세요(최대 20자) : ");
                scanf("%s", planName);
                printf("계획 설명을 입력해주세요(최대 1024자) : ");
                scanf("%s", explain);
                while (1) {
                    printf("계획을 오픈할 레벨을 입력해주세요(1 : 전체공개, 2 : 친구공개, 3 : 나만공개) : ");
                    scanf("%d", &openLevel);
                    if (openLevel > 0 && openLevel < 4) break;
                    else {
                        printf("1부터 3까지의 숫자만 입력해주세요.\n");
                        continue;
                    }
                }
                printf("계획 마감 날짜를 입력해주세요(형식 : yyyy-mm-dd) : ");
                scanf("%s", endAt);
                MakePlan(userIdx, planName, explain, openLevel, endAt);
                int planIdx = CheckLastPlanIdx();
                printf("계획을 저장했습니다.\n");
                while (1) {
                    printf("세부계획명을 입력해주세요(종료 : end) : ");
                    char input[20];
                    scanf("%s", input);
                    if (!strcmp(input, "end")) {
                        printf("세부계획 등록을 종료합니다.\n");
                        break;
                    }
                    char detailName[20], startedAt[20], endAt[20], where[20];
                    strcpy(detailName, input);
                    printf("시작할 날짜를 입력해주세요(형식 : yyyy-mm-dd) : ");
                    scanf("%s", startedAt);
                    printf("마감 날짜를 입력해주세요(형식 : yyyy-mm-dd) : ");
                    scanf("%s", endAt);
                    printf("실행할 장소를 입력해주세요 : ");
                    scanf("%s", where);
                    MakePlanDetail(planIdx, detailName, startedAt, endAt, where);
                    printf("세부계획을 추가했습니다.\n\n");
                    continue;
                }
                printf("%s에 %s가 추가되었습니다.", endAt, planName);
                break;
            case 2 :
                printf("계획 삭제를 선택했습니다.\n\n");
                int planLen = GetPlanLen(userIdx);
                char * idxArr = (char*)malloc(sizeof(char) * planLen);
                char * nameArr = (char*)malloc(sizeof(char) * planLen);
                GetPlanIdx(userIdx, idxArr, nameArr); 
                printf("삭제할 계획의 번호를 선택해주세요.\n--------------------------------------\n");
                for(int i = 0; i < planLen; ++i) {
                    printf("%s, %s\n", *(idxArr+i), *(nameArr+i));
                }
                printf("\n--------------------------------------\n");
                break;
            case 3 :
                printf("계획 수정을 선택했습니다.\n\n");

            case 4 :
                printf("계획 확인을 선택했습니다.\n\n");

            case 5 :
                printf("평가 확인을 선택했습니다.\n\n");

            case 6 : 
                printf("친구 계획 확인을 선택했습니다.\n\n");

            case 7 :
                printf("친구 추가를 선택했습니다.\n\n");

            default :
                printf("번호를 확인해주세요.");
                continue;
        }
        continue;
    }

    mysql_close(connection);

    return 0;
}


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


int SignUp(char name[]) {
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query);
    if (!query_stat) {
        return 1;
    } else {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}

int CheckLastUserIdx() {
    sprintf(query, "SELECT MAX(userIdx) FROM User");
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}

int CheckUser(int userIdx) {
    sprintf(query, "SELECT EXISTS (SELECT 1 FROM User WHERE userIdx = %d) AS cnt", userIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    char * a;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        a = sql_row[0];
    }
    return atoi(a);
}

int PrintUser() {
    sprintf(query, "SELECT * FROM User"); // User 테이블 안에 있는 모든 값을 가져오는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%s : %s\n", sql_row[0], sql_row[1]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}

int MakeFriend(int userIdx, int friendIdx) {
    sprintf(query, "INSERT INTO Friend VALUES (%d, %d), (%d, %d)", userIdx, friendIdx, friendIdx, userIdx);
    query_stat = mysql_query(connection, query);
    if (!query_stat) {
        return 1;
    } else {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}

int IsFriend(int userIdx, int friendIdx) {
    sprintf(query, "SELECT EXISTS (SELECT * FROM Friend WHERE userIdx = %d AND friendIdx = %d)", userIdx, friendIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        return atoi(sql_row[0]);
    }
}


int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]) {
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}

int CheckLastPlanIdx() {
    sprintf(query, "SELECT MAX(planIdx) FROM Plan");
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}

int GetPlanLen(int userIdx) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Plan WHERE userIdx = %d", userIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}

int GetPlanIdx(int userIdx, char * idxArr, char * nameArr) {
    sprintf(query, "SELECT planIdx, planName FROM Plan WHERE userIdx = %d", userIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        strcpy(*(idxArr+i), sql_row[0]), strcpy(*(nameArr+i), sql_row[1]);
        ++i;
    }
    return 1;
}

int GetPlan(int userIdx) {
    sprintf(query, "SELECT * FROM Plan WHERE userIdx = %d", userIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {

        printf("%s | %s | %s | %s | %s | %s | %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}

int GetFriendPlan(int userIdx, int friendIdx) {
    int access = IsFriend(userIdx, friendIdx);
    if (!access) return 0;
    /*
    1 : 전체 공개
    2 : 친구 공개
    3 : 나만 공개
    */
    sprintf(query, "SELECT * FROM Plan WHERE userIdx = %d AND openLevel < 3", friendIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%s | %s | %s | %s | %s | %s | %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}

void GetDayPlan(int * arr, int userIdx, char date[]) {
    sprintf(query, "SELECT planIdx, planName, createdAt, endAt FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("--------------------------------------\n");
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("No.%d - | %s | %s | %s | %s |\n", i+1, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
        *(arr + i) = atoi(sql_row[0]); // int* 배열에 result에 관한 planIdx를 담음 > 해당 planIdx를 선택한 후 다른 기능에 사용하기 위함
        ++i;
    }
    printf("--------------------------------------\n\n");
}

void DeletePlan(int userIdx, int planIdx) {
    sprintf(query, "DELETE FROM Plan WHERE userIdx = %d AND planIdx = %d", userIdx, planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully deleted.");
}

void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx) {
    sprintf(query, "UPDATE Plan SET planName = %s, explain = %s, endAt = %s WHERE planIdx = %d", planName, explain, endAt, planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}


int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]) {
    sprintf(query, "INSERT INTO Plandetail VALUES (0, %d, '%s', '%s', '%s', '%s', now())", planIdx, detailName, startedAt, endAt, where);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}

int CheckLastDetailIdx() {
    sprintf(query, "SELECT MAX(detailIdx) FROM Plandetail");
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}

int GetPlanDetailLen(int planIdx) {
    sprintf(query, "SELECT COUNT(planIdx) FROM Plandetail WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}

int GetPlanDetail(int planIdx) {
    sprintf(query, "SELECT detailIdx, detailName, startedAt, endAt, `where` FROM Plandetail WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%d번 - | %s | %s | %s | %s | %s |\n", i+1, sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);

        ++i;
    }
    printf("--------------------------------------\n\n");
    return 1;
}

void DeletePlandetail(int detailIdx) {
    sprintf(query, "DELETE FROM Plandetail WHERE detailIdx = %d", detailIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully deleted.");
}

void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]) {
    sprintf(query, "UPDATE Plandetail SET detailName = '%s', startedAt = '%s', endAt = '%s', `where` = '%s' WHERE detailIdx = %d", detailName, startedAt, endAt, where, detailIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}


//reviewIdx, planIdx, userIdx, content, score, createdAt
int MakePlanReview(int planIdx, int userIdx, char content[], int score) {
    sprintf(query, "INSERT INTO Planreview (planIdx, userIdx, content, score) VALUES (%d, %d, '%s', %d)", planIdx, userIdx, content, score);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}

int GetPlanReview(int planreviewIdx) {
    sprintf(query, "SELECT * FROM Planreview WHERE planreviewIdx = %d", planreviewIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%s | %s | %s | %s | %s | %s | %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}

void DeletePlanReview(int planreviewIdx) {
    sprintf(query, "DELETE FROM Planreview WHERE planreviewIdx = %d", planreviewIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully deleted.");
}

void ModifyPlanReview(int planreviewIdx, char content[], int score) {
    sprintf(query, "UPDATE Planreview SET content = %s, score = %d WHERE planreviewIdx = %d", content, score, planreviewIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}


//detailreviewIdx, detailIdx, userIdx, content, score, createdAt
int MakeDetailReview(int detailIdx, int userIdx, char content[], int score) {
    sprintf(query, "INSERT INTO Plandetailreview (detailIdx, userIdx, content, score) VALUES (%d, %d, '%s', %d)", detailIdx, userIdx, content, score);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}

int GetDetailReview(int detailreviewIdx) {
    sprintf(query, "SELECT * FROM Plandetailreview WHERE detailreviewIdx = %d", detailreviewIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%s | %s | %s | %s | %s | %s | %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}

void DeleteDetailReview(int detailreviewIdx) {
    sprintf(query, "DELETE FROM Plandetailreview WHERE detailreviewIdx = %d", detailreviewIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully deleted.");
}

void ModifyDetailReview(int detailreviewIdx, char content[], int score) {
    sprintf(query, "UPDATE Plandetailreview SET content = %s, score = %d WHERE planreviewIdx = %d", content, score, detailreviewIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}

