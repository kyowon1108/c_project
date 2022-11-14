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

int SignUp(char name[]); // 회원가입
int printUser(); // 유저 리스트 출력
int MakeFriend(int userIdx, int friendIdx); // 친구 추가
int IsFriend(int userIdx, int friendIdx); // 친구인지 확인

int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // 계획 생성
int GetPlanLen(int userIdx); // 유저가 생성한 계획의 수 리턴
int GetPlan(int userIdx); // 유저가 생성한 계획 리스트 출력
int GetFriendPlan(int userIdx, int friendIdx);
void GetDayPlan(int * arr, int userIdx, char date[]); // 특정 날의 계획 리스트 출력 및 인덱스 리턴
void DeletePlan(int userIdx, int planIdx); // 계획 삭제
void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx); // 계획 수정

int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 생성
int GetPlanDetailLen(int planIdx); // 유저가 생성한 계획 디테일의 수 리턴
int GetPlanDetail(int planIdx); // 유저가 생성한 계획 디테일 리스트 출력
void DeletePlandetail(int plandetailIdx, int planIdx); // 계획 디테일 삭제
void ModifyPlanDetail(char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 수정


int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // 서버와 통신 시작

    if (connection == NULL) // 연결 실패시
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }

    // char name[30];
    // printf("[ sign up ]\n");
    // printf("name : ");
    // fgets(name, 12, stdin);
    // CHOP(name);

    // SignUp(query, query_stat, name);
    // PrintUser(query, query_stat);


    // int userIdx = 1;
    // char planName[20] = "plantest";
    // char explain[20] = "explain";
    // int openLevel = 1;
    // char endAt[20] = "2022-11-10";
    // MakePlan(query, query_stat, userIdx, planName, explain, openLevel, endAt);
    
    // int userIdx = 1;
    // int planIdx = 2;
    // char date[10] = "2022-11-10";
    // int len = GetPlanLen(userIdx);
    // int *arr = (int*)malloc(sizeof(int) * len);
    // GetDayPlan(arr, userIdx, date);
    // for(int i = 0; i < len; ++i) {
    //     printf("%d : %d\n", i, *(arr+i));
    // }

    // MakePlanDetail(1, "DetailName1", "2022-11-06", "2022-11-20", "PlaceTest1");
    // MakePlanDetail(1, "DetailName2", "2022-11-20", "2022-11-22", "PlaceTest2");
    // GetPlanDetail(1);
    
    // GetFriendPlan(2, 1);
    // printf("\n");
    // GetFriendPlan(3, 1);
    // printf("\n");
    GetFriendPlan(4, 2);
    printf("\n");

    mysql_close(connection);

    return 0;
}


int SignUp(char name[]) {
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query);
    if (!query_stat) {
        printf("SignUp successed");
        return 1;
    } else {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
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

void DeletePlandetail(int plandetailIdx, int planIdx) {
    sprintf(query, "DELETE FROM Plandetail WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully deleted.");
}

void ModifyPlanDetail(char detailName[], char startedAt[], char endAt[], char where[]) {
    sprintf(query, "UPDATE Plandetail SET detailName = '%s', startedAt = '%s', endAt = '%s', `where` = '%s'", detailName, startedAt, endAt, where);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}
