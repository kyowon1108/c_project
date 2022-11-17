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

// [ �޷� ��� ���� �Լ�]
int IsLeafYear(int year); // �������� üũ
int getDay(int year, int month); // ���� ��¥ ���� ������ ����
int getStartDay(int year, int month); // �޷¿��� 1���� ���� ����
void printCalendar(int year, int month); // �޷� ���

// [ ���� ���� �Լ� ]
int SignUp(char name[]); // ȸ������
int CheckLastUserIdx();
int CheckUser(int userIdx);
int printUser(); // ���� ����Ʈ ���
int MakeFriend(int userIdx, int friendIdx); // ģ�� �߰�
int IsFriend(int userIdx, int friendIdx); // ģ������ Ȯ��

// [ ��ȹ ���� �Լ� ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // ��ȹ ����
int CheckLastPlanIdx();
int GetPlanLen(int userIdx); // ������ ������ ��ȹ�� �� ����
int GetPlanIdx(int userIdx, char ** arr); // ������ ������ ��ȹ�� �ε����� arr�� ����
int GetPlan(int userIdx); // ������ ������ ��ȹ ����Ʈ ���
int GetFriendPlan(int userIdx, int friendIdx); // ģ���� ������ ��ȹ ����Ʈ ���
void GetDayPlan(int * arr, int userIdx, char date[]); // Ư�� ���� ��ȹ ����Ʈ ��� �� �ε��� ����
void DeletePlan(int userIdx, int planIdx); // ��ȹ ����
void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx); // ��ȹ ����

// [ ���� ��ȹ ���� �Լ� ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����
int CheckLastDetailIdx();
int GetPlanDetailLen(int planIdx); // ������ ������ ��ȹ �������� �� ����
int GetPlanDetail(int planIdx); // ������ ������ ��ȹ ������ ����Ʈ ���
void DeletePlandetail(int detailIdx); // ��ȹ ������ ����
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����

// [ ��ȹ ���� ���� �Լ� ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score);
int GetPlanReview(int planreviewIdx);
void DeletePlanReview(int planreviewIdx);
void ModifyPlanReview(int planreviewIdx, char content[], int score);

// [ ���� ��ȹ ���� ���� �Լ�]
int MakeDetailReview(int detailIdx, int userIdx, char content[], int score);
int GetDetailReview(int detailreviewIdx);
void DeleteDetailReview(int detailreviewIdx);
void ModifyDetailReview(int detailreviewIdx, char content[], int score);


int userIdx;

int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // ������ ��� ����

    if (connection == NULL) {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    }


    while (1) {
        printf("| 1. �α��� | 2. ȸ������ |\n������ ��ȣ : ");
        int func, a;
        scanf("%d", &func);
        switch (func) {
            case 1 :
                printf("�α��� �� userIdx : ");
                scanf("%d", &a);
                if (CheckUser(a)) {
                    userIdx = a;
                    printf("%d�� �α����߽��ϴ�.\n", userIdx);
                    break;
                }
                else {
                    printf("�ش� ���̵�� �������� �ʽ��ϴ�. userIdx�� Ȯ�����ּ���.\n");
                    continue;
                }

            case 2 :
                printf("�̸��� �Է����ּ��� : ");
                char name[30];
                scanf("%s", name);
                int sign = SignUp(name);
                if (sign) {
                    int idx = CheckLastUserIdx();
                    printf("ȸ�������� �����߽��ϴ�.\nuserIdx : %d (userIdx�� �α������ּ���)\n", idx);
                    continue;
                } else {
                    printf("������ �߻��߽��ϴ�.");
                    continue;
                }

            default :
                printf("��ȣ�� Ȯ�����ּ���.\n");
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
        printf("| 1 : ��ȹ �߰� | 2 : ��ȹ ���� | 3 : ��ȹ ���� | 4 : ��ȹ Ȯ�� |\n| 5 : �� Ȯ�� | 6 : ģ�� ��ȹ Ȯ�� | 7 : ģ�� �߰� | 8 : ���� |\n ������ ��ȣ : ");
        scanf("%d", &function);
        if (function == 8) {
            printf("���α׷��� �����մϴ�.");
            break;
        }
        switch (function) {
            case 1 :
                printf("��ȹ �߰��� �����߽��ϴ�.\n\n");
                char planName[20], explain[1024], endAt[20];
                int openLevel;
                printf("��ȹ���� �Է����ּ���(�ִ� 20��) : ");
                scanf("%s", planName);
                printf("��ȹ ������ �Է����ּ���(�ִ� 1024��) : ");
                scanf("%s", explain);
                while (1) {
                    printf("��ȹ�� ������ ������ �Է����ּ���(1 : ��ü����, 2 : ģ������, 3 : ��������) : ");
                    scanf("%d", &openLevel);
                    if (openLevel > 0 && openLevel < 4) break;
                    else {
                        printf("1���� 3������ ���ڸ� �Է����ּ���.\n");
                        continue;
                    }
                }
                printf("��ȹ ���� ��¥�� �Է����ּ���(���� : yyyy-mm-dd) : ");
                scanf("%s", endAt);
                MakePlan(userIdx, planName, explain, openLevel, endAt);
                int planIdx = CheckLastPlanIdx();
                printf("��ȹ�� �����߽��ϴ�.\n");
                while (1) {
                    printf("���ΰ�ȹ���� �Է����ּ���(���� : end) : ");
                    char input[20];
                    scanf("%s", input);
                    if (!strcmp(input, "end")) {
                        printf("���ΰ�ȹ ����� �����մϴ�.\n");
                        break;
                    }
                    char detailName[20], startedAt[20], endAt[20], where[20];
                    strcpy(detailName, input);
                    printf("������ ��¥�� �Է����ּ���(���� : yyyy-mm-dd) : ");
                    scanf("%s", startedAt);
                    printf("���� ��¥�� �Է����ּ���(���� : yyyy-mm-dd) : ");
                    scanf("%s", endAt);
                    printf("������ ��Ҹ� �Է����ּ��� : ");
                    scanf("%s", where);
                    MakePlanDetail(planIdx, detailName, startedAt, endAt, where);
                    printf("���ΰ�ȹ�� �߰��߽��ϴ�.\n\n");
                    continue;
                }
                printf("%s�� %s�� �߰��Ǿ����ϴ�.", endAt, planName);
                break;
            case 2 :
                printf("��ȹ ������ �����߽��ϴ�.\n\n");
                int planLen = GetPlanLen(userIdx);
                char * idxArr = (char*)malloc(sizeof(char) * planLen);
                char * nameArr = (char*)malloc(sizeof(char) * planLen);
                GetPlanIdx(userIdx, idxArr, nameArr); 
                printf("������ ��ȹ�� ��ȣ�� �������ּ���.\n--------------------------------------\n");
                for(int i = 0; i < planLen; ++i) {
                    printf("%s, %s\n", *(idxArr+i), *(nameArr+i));
                }
                printf("\n--------------------------------------\n");
                break;
            case 3 :
                printf("��ȹ ������ �����߽��ϴ�.\n\n");

            case 4 :
                printf("��ȹ Ȯ���� �����߽��ϴ�.\n\n");

            case 5 :
                printf("�� Ȯ���� �����߽��ϴ�.\n\n");

            case 6 : 
                printf("ģ�� ��ȹ Ȯ���� �����߽��ϴ�.\n\n");

            case 7 :
                printf("ģ�� �߰��� �����߽��ϴ�.\n\n");

            default :
                printf("��ȣ�� Ȯ�����ּ���.");
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
    printf("           [ %d�� %d�� ]            \n\n", year, month);
    printf("   ��   ��   ȭ   ��   ��   ��   ��\n\n");
    for(int i = 0; i < start_day; ++i) printf("     ");
    int count = start_day; // 7���� �������� �ָ� �ٲ���� �ϱ� ������ count�� ���
    for (int j = 1; j <= day; ++j) {
        int check = 0;
        if (check == 0) {
            printf("%5d", j);
            count++;
        }
        if (count == 7) { // ī��Ʈ�� 7�� �Ǵ� ���� �����ַ� �Ѿ
            printf("\n\n");
            count = 0;
        }
    }
}


int SignUp(char name[]) {
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User�� �߰��ϴ� ������
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
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User�� �߰��ϴ� ������
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
    1 : ��ü ����
    2 : ģ�� ����
    3 : ���� ����
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
        *(arr + i) = atoi(sql_row[0]); // int* �迭�� result�� ���� planIdx�� ���� > �ش� planIdx�� ������ �� �ٸ� ��ɿ� ����ϱ� ����
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
        printf("%d�� - | %s | %s | %s | %s | %s |\n", i+1, sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);

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

