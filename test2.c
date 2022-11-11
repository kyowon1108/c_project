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

int SignUp(char name[]);
int printUser();

int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]);
int GetPlanLen(int userIdx);
int GetPlan(int userIdx);
int PrintPlan(int userIdx);
void GetDayPlan(int * arr, int userIdx, char date[]);
void DeletePlan(int userIdx, int planIdx);

int MakePlanDetail(int planIdx, char startedAt[], char endAt[], char where[]);
int GetPlanDetail(int planIdx);

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

    MakePlanDetail(1, "2022-11-06", "2022-11-20", "PlaceTest1");
    MakePlanDetail(1, "2022-11-20", "2022-11-22", "PlaceTest2");
    GetPlanDetail(1);
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

// void GetPlan(int userIdx) {
//     sprintf(query, "SELECT * FROM Plan WHERE userIdx = %d", userIdx);
//     query_stat = mysql_query(connection, query); 
//     if (query_stat != 0)
//     {
//         fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
//         return;
//     }
//     sql_result = mysql_store_result(connection);
//     while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {

//     }
// }

int PrintPlan(int userIdx) {
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

int MakePlanDetail(int planIdx, char startedAt[], char endAt[], char where[]) {
    sprintf(query, "INSERT INTO Plandetail VALUES (0, %d, '%s', '%s', '%s', now())", planIdx, startedAt, endAt, where);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}

int GetPlanDetail(int planIdx) {
    SELECT planIdx, planName, createdAt, endAt FROM Plan WHERE userIdx = %d
    sprintf(query, "SELECT detailIdx, startedAt, endAt where FROM Plandetail WHERE planIdx = %d", planIdx);
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
        printf("%d번 - | %s | %s | %s | %s \n", i+1, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
    }
    printf("--------------------------------------\n\n");
    return 1;
}