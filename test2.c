#include "/usr/include/mysql/mysql.h"
#include <string.h>
#include <stdio.h>


#define HOST "localhost"
#define USER "kapr"
#define PASSWORD "kyowon1108"
#define DB "PFP"
#define CHOP(x) x[strlen(x) - 1] = ' '


int main(void) {
    MYSQL *connection = NULL, conn;
    MYSQL_RES *sql_result;
    MYSQL_ROW sql_row;
    int query_stat; 

    char query[255]; // �Է��� mysql �������� �� ����
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // ������ ��� ����

    if (connection == NULL) // ���� ���н�
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }

    printf("[sign up]\nname : ");
    int name[30];
    scanf("%s", name);
    SignUp(name, connection);


    mysql_free_result(sql_result);
    mysql_close(connection);
    return 0;
}

int SignUp(char name[], MYSQL connection, MYSQL_RES *sql_result, MYSQL_ROW sql_row;) {
    char query[255];

    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User�� �߰��ϴ� ������

    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 1;
    }

    query_stat = mysql_query(connection, "select * from User"); // ���� ��� User�� �ҷ����� ������
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 1;
    }
    
    sql_result = mysql_store_result(connection);
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("%s : %s\n", sql_row[0], sql_row[1]);
    }
    printf("--------------------------------------\n\n");

}