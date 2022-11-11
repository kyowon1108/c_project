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
char query[255]; // �Է��� mysql �������� �� ����

int SignUp(char name[]);
int printUser();
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]);
int GetPlanLen(int userIdx);
int GetPlan(int userIdx);
int PrintPlan(int userIdx);
void GetDayPlan(int userIdx, char date[]);
void DeletePlan(int userIdx, int planIdx);

int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // ������ ��� ����

    if (connection == NULL) // ���� ���н�
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
    
    int userIdx = 1;
    int planIdx = 2;
    char date[10] = "2022-11-10";
    int a[30] = GetDayPlan(userIdx, date);
    for(int i = 0; i < GetPlanLen(userIdx); ++i) {
        printf("%d : %d\n", i, a[i]);
    }
    mysql_close(connection);

    return 0;
}


int SignUp(char name[]) {
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User�� �߰��ϴ� ������
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
    sprintf(query, "SELECT * FROM User"); // User ���̺� �ȿ� �ִ� ��� ���� �������� ������
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
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User�� �߰��ϴ� ������
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

char * GetDayPlan(int userIdx, char date[]) {
    sprintf(query, "SELECT planIdx, planName, createdAt, endAt FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return;
    }
    sql_result = mysql_store_result(connection);
    printf("--------------------------------------\n");
    int len = GetPlanLen(userIdx);
    int *arr = (int*)malloc(sizeof(int) * len);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("No.%d - | %s | %s | %s | %s |\n", i+1, sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
        *(arr + i) = atoi(sql_row[0]);
        ++i;
    }
    printf("--------------------------------------\n\n");
    return arr;
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