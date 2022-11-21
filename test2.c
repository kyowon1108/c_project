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
int GetPlanIdx(int userIdx, int * idxArr, char ** nameArr, char ** endArr); // 유저가 생성한 계획의 인덱스를 arr에 저장
int GetPlan(char ** arr, int userIdx); // 유저가 생성한 계획 리스트 출력
int GetFriendPlan(int userIdx, int friendIdx); // 친구가 생성한 계획 리스트 출력
int GetDayPlanLen(char date[]);
int GetDayPlan(int userIdx, char date[], int * idxArr, char ** nameArr, char ** explainArr); // 특정 날의 계획 리스트 출력 및 인덱스 리턴
int DeletePlan(int userIdx, int planIdx); // 계획 삭제
int ModifyPlan(int planIdx, char planName[], char explain[], char endAt[]); // 계획 수정

// [ 세부 계획 관련 함수 ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 생성
int CheckLastDetailIdx();
int GetPlanDetailLen(int planIdx); // 유저가 생성한 계획 디테일의 수 리턴
int GetPlanDetail(int planIdx); // 유저가 생성한 계획 디테일 리스트 출력
int DeletAllePlandetail(int planIdx);
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
        printf("| 1. SignIn | 2. SignUp |\nNumber to Execute : ");
        int func, a;
        scanf("%d", &func);
        switch (func) {
            case 1 :
                printf("userIdx to signin : ");
                scanf("%d", &a);
                if (CheckUser(a)) {
                    userIdx = a;
                    printf("Success Signin userIdx : %d\n", userIdx);
                    break;
                }
                else {
                    printf("This userIdx does not exist. Please check userIdx.\n");
                    continue;
                }

            case 2 :
                printf("Please enter a name : ");
                char name[30];
                scanf("%s", name);
                int sign = SignUp(name);
                if (sign) {
                    int idx = CheckLastUserIdx();
                    printf("successfully registered.\nuserIdx : %d (SignIn to userIdx)\n", idx);
                    continue;
                } else {
                    printf("error");
                    continue;
                }

            default :
                printf("Please check the number.\n");
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
    int planLen;
    int * idxArr;
    char ** nameArr;
    char ** endArr;
    char ** explainArr;

    while(isRoof) {
        printf("| 1 : Add plan | 2 : Delete plan | 3 : Modify plan | 4 : Check plan |\n| 5 : Check review | 6 : Check friend review | 7 : Add friend | 8 : End |\nNumber to execute : ");
        scanf("%d", &function);
        if (function == 8) {
            printf("Bye.");
            break;
        }
        switch (function) {
            case 1 :
                printf("Selected Add Plan.\n\n");
                char planName[20], explain[1024], endAt[20];
                int openLevel;
                printf("Please enter a plan name(20 char maximum) : ");
                scanf("%s", planName);
                printf("Please enter a description of the plan (1024 char maximum) : ");
                scanf("%s", explain);
                while (1) {
                    printf("Please enter the openLevel of the plan (1 : Full disclosure, 2 : Friend disclosure, 3 : Only me) : ");
                    scanf("%d", &openLevel);
                    if (openLevel > 0 && openLevel < 4) break;
                    else {
                        printf("Please enter only numbers from 1 to 3.\n");
                        continue;
                    }
                }
                printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                scanf("%s", endAt);
                MakePlan(userIdx, planName, explain, openLevel, endAt);
                int planIdx = CheckLastPlanIdx();
                printf("Plan has been saved.\n");
                while (1) {
                    printf("Please enter a detail plan name(if want end enter 'end') : ");
                    char input[20];
                    scanf("%s", input);
                    if (!strcmp(input, "end")) {
                        printf("Ends detail plan registration.\n");
                        break;
                    }
                    char detailName[20], startedAt[20], endAt[20], where[20];
                    strcpy(detailName, input);
                    printf("Please enter a date to start (format: yyyy-mm-dd) : ");
                    scanf("%s", startedAt);
                    printf("Please enter the deadline (format: yyyy-mm-dd) : ");
                    scanf("%s", endAt);
                    printf("Please enter a place to run : ");
                    scanf("%s", where);
                    MakePlanDetail(planIdx, detailName, startedAt, endAt, where);
                    printf("successfully added a detail plan.\n\n");
                    continue;
                }
                printf("%s has been added to %s.\n\n", planName, endAt);
                break;
            case 2 :
                printf("Selected Delete plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) {
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                idxArr = (int*)malloc(sizeof(int) * planLen);
                nameArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(nameArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                endArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(endArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                GetPlanIdx(userIdx, idxArr, nameArr, endArr); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                for(int i = 0; i < planLen; ++i) {
                    printf("No.%d : %s | %s\n", i + 1, *(nameArr + i), *(endArr + i));
                }
                printf("\n--------------------------------------\nnumber(cancel : 0) : ");
                while (1) {
                    int num;
                    scanf("%d", &num);
                    if (num == 0) {
                        printf("delete canceled.\n\n");
                        break;
                    }
                    if (num < 0 || num > planLen) {
                        printf("Please enter a valid number.\nNumber of plan to delete : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num);
                    DeletAllePlandetail(planIdx);
                    DeletePlan(userIdx, planIdx);
                    printf("Plan has been deleted.\n\n");
                    break;
                }
                break;

            case 3 :
                printf("Selected Modify Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) {
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                idxArr = (int*)malloc(sizeof(int) * planLen);
                nameArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(nameArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                endArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(endArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                GetPlanIdx(userIdx, idxArr, nameArr, endArr); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                for(int i = 0; i < planLen; ++i) {
                    printf("No.%d : %s | %s\n", i + 1, *(nameArr + i), *(endArr + i));
                }
                printf("\n--------------------------------------\nnumber(cancel : 0) : ");
                while (1) {
                    int num;
                    scanf("%d", &num);
                    if (num == 0) {
                        printf("modify canceled.\n\n");
                        break;
                    }
                    if (num < 0 || num > planLen) {
                        printf("Please enter a valid number.\nNumber of plan to modify : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num);
                    char ** arr = (char**)malloc(sizeof(char*) * 7);
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    }
                    GetPlan(arr, planIdx);
                    char planName[20], explain[1024], endAt[20];
                    strcpy(planName, *(arr + 2)), strcpy(explain, *(arr + 3)), strcpy(endAt, *(arr + 6));
                    printf("selected %d. Choose you wan to modify.\n| 1 : planName | 2 : explain | 3 : deadline | : ", num);
                    int modifyFunc;
                    scanf("%d", &modifyFunc);
                    switch (modifyFunc) {
                        case 1 :
                            printf("Please enter a plan name(20 char maximum) : ");
                            scanf("%s", planName);
                            break;
                        case 2 :
                            printf("Please enter a description of the plan (1024 char maximum) : ");
                            scanf("%s", planName);
                            break;
                        case 3 :
                            printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                            scanf("%s", planName);
                            break;
                        default :
                            printf("Please check your number.\n");       
                    }
                    
                    ModifyPlan(planIdx, planName, explain, endAt);
                    printf("Plan has been modified.\n\n");
                    break;
                }
                break;
            case 4 :
                printf("Selected Check Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) {
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                char date[20];
                printf("Please select the date of the plan to check (format : yyyy-mm-dd) : ");
                scanf("%s", date);
                planLen = GetDayPlanLen(date);
                if (!planLen) {
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                idxArr = (int*)malloc(sizeof(int) * planLen);
                nameArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(nameArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                explainArr = (char**)malloc(sizeof(char*) * planLen);
                for(int i = 0; i < planLen; ++i) {
                    *(explainArr+i) = (char*)malloc(sizeof(char) * 20);
                }
                GetDayPlan(userIdx, date, idxArr, nameArr, explainArr);
                for (int i = 0; i < planLen; ++i) {
                    int planIdx = *(idxArr + i);
                    char planName[20], explain[1024];
                    strcpy(planName, *(nameArr + i)), strcpy(explain, *(explainArr + i));
                    printf("\n--------------------------------------\n");
                    printf("No.%d\nplanName :  %s\nexplain : %s\n", i + 1, planName, explain);
                    if (GetPlanDetailLen(planIdx)) GetPlanDetail(planIdx);
                    printf("--------------------------------------\n");
                }
                break;
            case 5 :
                printf("Seleted Check Review.\n\n");

                break;
            case 6 : 
                printf("Seleted Check FriendReview.\n\n");

                break;
            case 7 :
                printf("Selected Add Friend.\n\n");
                int friendIdx;
                printf("Enter friend's userIdx : ");
                scanf("%d", &friendIdx);
                int check = CheckUser(friendIdx);
                if (!check) {
                    printf("userIdx not exists.\n\n");
                    break;
                }
                MakeFriend(userIdx, friendIdx);
                printf("Successfully friend added.\n\n");
                break;
            default :
                printf("Please check your number.\n\n");
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

int GetPlanIdx(int userIdx, int * idxArr, char ** nameArr, char ** endArr) {
    sprintf(query, "SELECT planIdx, planName, endAt FROM Plan WHERE userIdx = %d", userIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        char *ptr = strtok(sql_row[2], " ");
        idxArr[i] = atoi(sql_row[0]), *(nameArr+i) = sql_row[1], *(endArr+i) = ptr;
        ++i;
    }
    return 1;
}

int GetPlan(char ** arr, int planIdx) {
    sprintf(query, "SELECT * FROM Plan WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        for (int i = 0; i < 7; ++i) {
            *(arr + i) = sql_row[i];
        }
    }
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

int GetDayPlanLen(char date[]) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Plan WHERE DATE(endAt) = '%s'", date);
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

int GetDayPlan(int userIdx, char date[], int * idxArr, char ** nameArr, char ** explainArr) {
    sprintf(query, "SELECT planIdx, planName, `explain` FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        idxArr[i] = atoi(sql_row[0]), strcpy(*(nameArr+i),sql_row[1]), strcpy(*(explainArr+i), sql_row[2]);
        ++i;
    }
    return 1;
}

int DeletePlan(int userIdx, int planIdx) {
    sprintf(query, "DELETE FROM Plan WHERE userIdx = %d AND planIdx = %d", userIdx, planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    return 1;
}

int ModifyPlan(int planIdx, char planName[], char explain[], char endAt[]) {
    sprintf(query, "UPDATE Plan SET planName = '%s', `explain` = '%s', endAt = '%s' WHERE planIdx = %d", planName, explain, endAt, planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    return 1;
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
    sprintf(query, "SELECT detailName, startedAt, endAt, `where` FROM Plandetail WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("\n[detail %d]\ndetailName : %s\nstarted date : %s\ndeadline : %s\nplace : %s\n", i + 1, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
        ++i;
    }
    return 1;
}

int DeletAllePlandetail(int planIdx) {
    sprintf(query, "DELETE FROM Plandetail WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
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

