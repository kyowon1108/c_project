//#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "/usr/include/mysql/mysql.h"

MYSQL *conn_ptr;

MYSQL_RES *res_ptr;

MYSQL_ROW sqlrow;



int db_init() {
        conn_ptr = mysql_init(NULL);
        if(!conn_ptr) {
                printf("error");
                exit(0);
        }
        else {
                printf("debug1\n");
        }

        conn_ptr = mysql_real_connect(conn_ptr,"localhost","kapr","kyowon1108","PFP",0,NULL,0);
        if(conn_ptr) {
                printf("sucess\n");
        }
        else {
                printf("connect error!\n");
        }

        mysql_close(conn_ptr);
        return 0;
}

int main(int argc, char **argv) {
        printf("main!~~\n");

        int temp = db_init();

        return 0;
}