#include "/usr/include/mysql/mysql.h"
#include <string.h>
#include <stdio.h>


#define DB_HOST "localhost"
#define DB_USER "kapr"
#define DB_PASS "kyowon1108"
#define DB_NAME "PFP"
#define CHOP(x) x[strlen(x) - 1] = ' '
    
int main(void)
{
    MYSQL       *connection=NULL, conn;
    MYSQL_RES   *sql_result;
    MYSQL_ROW   sql_row;
    int       query_stat; 

    char name[12];
    char address[80];
    char tel[12];
    char query[255];
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, DB_HOST,
                                    DB_USER, DB_PASS,
                                    DB_NAME, 3306,
                                    (char *)NULL, 0);

    if (connection == NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }

    query_stat = mysql_query(connection, "select * from User");
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 1;
    }
    
    sql_result = mysql_store_result(connection);
    
    // printf("%+11s %-30s %-10s", "�̸�", "�ּ�", "��ȭ��ȣ");
    // while ( (sql_row = mysql_fetch_row(sql_result)) != NULL )
    // {
    //     printf("%+11s %-30s %-10s", sql_row[0], sql_row[1], sql_row[2]);
    // }

    mysql_free_result(sql_result);

    printf("�̸� :");
    fgets(name, 12, stdin);
    CHOP(name);

    sprintf(query, "insert into User values "
                   "('%s')",
                   name);

    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 1;
    }

    mysql_close(connection);
}