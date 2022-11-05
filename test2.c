#include <stdio.h>
#include <string.h>
#include <time.h>
#include "/usr/include/mysql/mysql.h"


#define HOST "localhost"
#define USER "kapr"
#define PASSWORD "kyowon1108"
#define DB "PFP"
#define CHOP(x) x[strlen(x) - 1] = ' '

MYSQL *connection = NULL, conn;
MYSQL_RES *sql_result;
MYSQL_ROW sql_row;

int SignUp(char *query, int query_stat, char name[]);
int MakePlan(char *query, int query_stat, int userIdx, char planName[], char explain[], int openLevel, char endAt[]);


int main(void) {
    int query_stat;
    char query[255]; // 입력할 mysql 쿼리문이 들어갈 변수
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // 서버와 통신 시작

    if (connection == NULL) // 연결 실패시
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }

    char name[30];
    printf("[ sign up ]\n");
    printf("name : ");
    fgets(name, 12, stdin);
    CHOP(name);

    SignUp(query, query_stat, name);


    // int userIdx = 1;
    // char planName[20] = "planTest";
    // char explain[20] = "planExplain";
    // int openLevel = 1;
    // char endAt[20] = "2022-11-10";
    // MakePlan(query, query_stat, userIdx, planName, explain, openLevel, endAt);
}


int SignUp(char *query, int query_stat, char name[]) {
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User을 추가하는 쿼리문

    query_stat = mysql_query(connection, query);
    if (!query_stat) {
        printf("SignUp successed");
    } else {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }

    // sprintf(query, "SELECT * FROM User"); // User 테이블 안에 있는 모든 값을 가져오는 쿼리문
    // query_stat = mysql_query(connection, query); 
    // if (query_stat != 0)
    // {
    //     fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
    //     return 1;
    // }
    
    // sql_result = mysql_store_result(connection);
    // printf("\n--------------------------------------\n");
    // while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
    //     printf("%s : %s\n", sql_row[0], sql_row[1]);
    // }
    // printf("--------------------------------------\n\n");

    mysql_close(connection);
}



int MakePlan(char *query, int query_stat, int userIdx, char planName[], char explain[], int openLevel, char endAt[]) {
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 1;
    }
    mysql_close(connection);
}