//#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "/usr/include/mysql/mysql.h"

MYSQL *conn_ptr;

MYSQL_RES *res_ptr;

MYSQL_ROW sqlrow;

#define HOST "localhost"
#define USER "kapr"
#define PASSWORD "kyowon1108"
#define DB "PFP"

int db_init() {
        conn_ptr = mysql_init(NULL);
        if(!conn_ptr) {
                printf("error");
                exit(0);
        }
        else printf("debug1\n");

        conn_ptr = mysql_real_connect(conn_ptr, HOST, USER, PASSWORD, DB, 0, NULL, 0);

        if(conn_ptr) printf("sucess\n");
        else printf("connect error!\n");

        mysql_close(conn_ptr);
        return 0;
}

int main(void) {
        int argc;
        char **argv;
        int temp = db_init();

        char query[255];
        sprintf(query, "insert into User values "
                   "('%s')", "name_test");
        mysql_querry(temp, query);
        return 0;
}