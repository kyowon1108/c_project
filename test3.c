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

void MakeLog(FILE* fp, int userIdx, char content[]);
char* GetISOTime(struct tm *t);

// [ �޷� ��� ���� �Լ�]
int IsLeafYear(int year); // �������� üũ
int getDay(int year, int month); // ���� ��¥ ���� ������ ����
int getStartDay(int year, int month); // �޷¿��� 1���� ���� ����
void printCalendar(int year, int month); // �޷� ���

// [ ���� ���� �Լ� ]
int SignUp(char name[]); // ȸ������
int CheckLastUserIdx(); //���� �ֱٿ� �߰��� user�� idx �ҷ�����
int CheckUser(int userIdx); // ���� ���� Ȯ��
int printUser(); // ���� ����Ʈ ���
int MakeFriend(int userIdx, int friendIdx); // ģ�� �߰�
int GetFriendLen(int userIdx); //ģ�� �� �ҷ�����
int GetFriend(int * idxArr, int userIdx); // ģ�� �ҷ�����
int IsFriend(int userIdx, int friendIdx); // ģ������ �ƴ��� Ȯ��

// [ ��ȹ ���� �Լ� ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // ��ȹ ����
int CheckLastPlanIdx(); //���� �ֱ� ���� ��ȹ�� �ε��� �ҷ�����
int GetPlanLen(int userIdx); // ������ ������ ��ȹ�� �� ����
int GetPlanIdx(int userIdx, int * idxArr); // ������ ������ ��ȹ�� �ε����� arr�� ����
int GetPlan(char ** arr, int userIdx); // ������ ������ ��ȹ ����Ʈ ���
int GetFriendPlan(int userIdx, int friendIdx, int * idxArr); // ģ���� ������ ��ȹ ����Ʈ ���
int GetDayPlanLen(int userIdx, char date[]); //������ ������ ��¥�� ��ȹ ���� �ҷ�����
int GetDayPlan(int userIdx, char date[], int * idxArr); // Ư�� ���� ��ȹ ����Ʈ ��� �� �ε��� ����
int DeletePlan(int userIdx, int planIdx); // ��ȹ ����
int ModifyPlan(int planIdx, char planName[], char explain[], char endAt[]); // ��ȹ ����

// [ ���� ��ȹ ���� �Լ� ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����
int CheckLastDetailIdx(); //���� �ֱ� ���� ���ΰ�ȹ �ε��� �ҷ�����
int GetPlanDetailLen(int planIdx); // ������ ������ ��ȹ �������� �� �ҷ�����
int GetPlanDetail(int planIdx); // ������ ������ ��ȹ ������ ����Ʈ ���
int DeletAllePlandetail(int planIdx); // ��� ���ΰ�ȹ ����
void DeletePlandetail(int detailIdx); // ��ȹ ������ ����
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����

// [ ��ȹ ���� ���� �Լ� ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score); // ��ȹ ���� ����
int GetPlanReviewLen(int planIdx); //���� ���� �ҷ�����
int GetPlanReview(int planIdx); // ��ȹ ���� ���
void DeletePlanReview(int planreviewIdx); // ���� ����
void ModifyPlanReview(int planreviewIdx, char content[], int score); // ���� ����

//[ ç���� ���� �Լ� ]
int MakeChallenge(char depositName[], int money, char endAt[]); //ç���� ����
int MakeChallengeUser(int depositIdx, int userIdx); //ç���� ���� �߰�
int CheckLastDepositIdx(); //�ֱ� ���� ç���� �ε��� �ҷ�����
int IsChallengeUser(int depositIdx, int userIdx); //ç���� �������� Ȯ��
int GetChallengeLen(int userIdx); //ç���� �� �ҷ�����
int GetChallengeUserLen(int depositIdx); //ç������ �����ϴ� ���� �� �ҷ�����
int GetChallengeIdx(int *idxArr, int len, int userIdx);  //�������� ç���� idx�ҷ�����
int GetChallenge(int depositIdx); // ç���� ���


int userIdx;

int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // ������ ��� ����

    if (connection == NULL) {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    } //���� ���� �ȵɽ� ���� ȭ��

    FILE* fp = fopen("log.txt","a"); // �α׸� ����� ���� ���� ����
    char logContent[1024];

    while (1) { //�α���, ȸ������
        printf("| 1. SignIn | 2. SignUp |\nNumber to Execute : ");
        int func, a;
        scanf("%d", &func);
        switch (func) {
            case 1 : //�α���
                printf("userIdx to signin : "); //���� �ε����� �α���
                scanf("%d", &a);
                if (CheckUser(a)) { //ȸ������ �Ǿ��ִ� �������� Ȯ��
                    userIdx = a;
                    printf("Success Signin userIdx : %d\n", userIdx);
                    MakeLog(fp, userIdx, "signin");
                    break;
                }
                else { //ȸ������ �ȵǾ� ���� ��
                    printf("This userIdx does not exist. Please check userIdx.\n");
                    continue;
                }

            case 2 : //ȸ������
                printf("Please enter a name : ");
                char name[30];
                scanf("%s", name);
                int sign = SignUp(name);
                if (sign) {
                    int idx = CheckLastUserIdx(); //�ֱ� �߰��� ������ �ε��� �ҷ�����
                    printf("successfully registered.\nuserIdx : %d (SignIn to userIdx)\n", idx);
                    MakeLog(fp, idx, "signup");
                    continue;
                } else {
                    printf("error");
                    continue;
                }

            default : // 1, 2�� �ƴ� �ٸ� �Է� ������ ��
                printf("Please check the number.\n");
                continue;
        }
        break;
    }

    printf("\n\n");

    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    printCalendar(t->tm_year + 1900, t->tm_mon + 1); //�޷� ���

    printf("\n\n");
    int isRoof = 1;
    int function;

    int planLen;
    int * idxArr;
    char ** nameArr;
    char ** endArr;
    char ** explainArr;

    while(isRoof) {
        printf("| 1 : Add plan | 2 : Delete plan | 3 : Modify plan | 4 : Check plan |\n");
        printf("| 5 : Check review | 6 : Check friend plan | 7 : Add friend |\n");
        printf("| 8 : Add Challenge | 9 : Join Challenge | 10 : Check Challenge | 0 : End |\nNumber to execute : ");
        scanf("%d", &function);
        if (function == 0) {  //����
            printf("Bye.");
            MakeLog(fp, userIdx, "signout");
            break;
        }
        switch (function) {
            case 1 : { // ��ȹ �߰�
                printf("Selected Add Plan.\n\n");
                char planName[20], explain[1024], endAt[20];
                int openLevel;
                printf("Please enter a plan name(20 char maximum) : "); 
                scanf("%s", planName); //��ȹ �̸� ����
                printf("Please enter a description of the plan (1024 char maximum) : ");
                scanf("%s", explain); //��ȹ ����
                while (1) {
                    printf("Please enter the openLevel of the plan (1 : Full disclosure, 2 : Friend disclosure, 3 : Only me) : ");
                    scanf("%d", &openLevel); //�������� �Ϻΰ��� ��ü���� ����
                    if (openLevel > 0 && openLevel < 4) break;
                    else { //1, 2, 3�� �ƴ� �ٸ� �Է� ������ �� ���Է�
                        printf("Please enter only numbers from 1 to 3.\n");
                        continue;
                    }
                }
                printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                scanf("%s", endAt); //��ȹ ���� ��¥
                MakePlan(userIdx, planName, explain, openLevel, endAt); // ��ȹ ����
                int planIdx = CheckLastPlanIdx(); //���ΰ�ȹ �߰� ��ġ�� ���ϱ� ����
                printf("Plan has been saved.\n");
                while (1) { //���ΰ�ȹ �߰�
                    printf("Please enter a detail plan name(if want end enter 'end') : ");
                    char input[20];
                    scanf("%s", input);
                    if (!strcmp(input, "end")) { //���ΰ�ȹ �߰� ��
                        printf("Ends detail plan registration.\n");
                        break;
                    }
                    char detailName[20], startedAt[20], endAt[20], where[20];
                    strcpy(detailName, input); //���ΰ�ȹ �̸� ����
                    printf("Please enter a date to start (format: yyyy-mm-dd) : ");
                    scanf("%s", startedAt); //���ΰ�ȹ ���� ��¥ ����
                    printf("Please enter the deadline (format: yyyy-mm-dd) : ");
                    scanf("%s", endAt); //���ΰ�ȹ ���� ��¥ ����
                    printf("Please enter a place to run : ");
                    scanf("%s", where); // ���ΰ�ȹ ���� ��� ���� ex) cafe, studyroom 
                    MakePlanDetail(planIdx, detailName, startedAt, endAt, where); //���ΰ�ȹ ����
                    printf("successfully added a detail plan.\n\n");
                    continue;
                }
                printf("%s has been added to %s.\n\n", planName, endAt);
                MakeLog(fp,  userIdx, "add plan success");
                break; }

            case 2 : { //��ȹ ����
                printf("Selected Delete plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //��ȹ�� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "delete plan fail ( no exist plan )");
                    break;
                } //����ȭ������ ���ư�
                idxArr = (int*)malloc(sizeof(int) * planLen); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                GetPlanIdx(userIdx, idxArr); //��ȹ �ҷ�����
                printf("--------------------------------------\nnumber(cancel : 0) : "); 
                while (1) { //��ȹ ����
                    int num;
                    scanf("%d", &num); //������ ��ȹ ����
                    if (num == 0) { //�Լ� ������
                        printf("delete canceled.\n\n");
                        MakeLog(fp, userIdx, "delete plan cancel");
                        break;
                    }
                    if (num < 0 || num > planLen) { //��ȹ �ȿ� �ִ� �� �ܿ� �ٸ� ���� �������� ��
                        printf("Please enter a valid number.\nNumber of plan to delete : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num); //planIdx�� ����ڰ� ������ �� ����
                    DeletAllePlandetail(planIdx); // ū ��ȹ ����
                    DeletePlan(userIdx, planIdx); // ���ΰ�ȹ ����
                    printf("Plan has been deleted.\n\n");
                    MakeLog(fp, userIdx, "delete plan success");
                    break;
                }
                break; }

            case 3 : { // ��ȹ ����
                printf("Selected Modify Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //��ȹ�� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "modify plan fail ( no exist plan )");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                GetPlanIdx(userIdx, idxArr); // ������ ������ ��ȹ�� �ε����� arr�� ����
                printf("--------------------------------------\nnumber(cancel : 0) : ");
                while (1) { //������ ��ȹ ����
                    int num;
                    scanf("%d", &num);
                    if (num == 0) { //�Լ� ������
                        printf("modify canceled.\n\n");
                        MakeLog(fp, userIdx, "modify plan cancel");
                        break;
                    }
                    if (num < 0 || num > planLen) { //��ȹ�� ���� �ε��� ���ý�
                        printf("Please enter a valid number.\nNumber of plan to modify : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num); //planIdx�� ����ڰ� ������ �� ����
                    char ** arr = (char**)malloc(sizeof(char*) * 7);
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    }
                    GetPlan(arr, planIdx); //��� ��ȹ arr�� �ҷ�����
                    char planName[20], explain[1024], endAt[20];
                    strcpy(planName, *(arr + 2)), strcpy(explain, *(arr + 3)), strcpy(endAt, *(arr +  6)); 
                    printf("selected %d. Choose you wan to modify.\n| 1 : planName | 2 : explain | 3 : deadline | : ", num);
                    int modifyFunc;
                    scanf("%d", &modifyFunc); //������ ���� ����
                    switch (modifyFunc) {
                        case 1 : // ��ȹ �̸� ����
                            printf("Please enter a plan name(20 char maximum) : ");
                            scanf("%s", planName);
                            break;
                        case 2 : //��ȹ ���� ����
                            printf("Please enter a description of the plan (1024 char maximum) : ");
                            scanf("%s", explain);
                            break;
                        case 3 : //��ȹ ���� ����
                            printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                            scanf("%s", endAt);
                            break;
                        default :
                            printf("Please check your number.\n");       
                    }
                    
                    ModifyPlan(planIdx, planName, explain, endAt); //��ȹ ����
                    printf("Plan has been modified.\n\n");
                    MakeLog(fp, userIdx, "modify plan success");
                    break;
                }
                break; }

            case 4 : { //��ȹ Ȯ�� 
                printf("Selected Check Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //��ȹ�� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check plan fail ( no exist plan )");
                    break;
                }
                printf("Please select the date of the plan to check (format : yyyy-mm-dd) : ");
                char date[20];
                scanf("%s", date); // Ȯ���� ��¥ ����
                int planLen = GetDayPlanLen(userIdx, date); //�������� �Ҵ� ���� �Լ�
                if (!planLen) { //��¥�� ��ȹ�� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); //�������� �Ҵ�
                GetDayPlan(userIdx, date, idxArr); //�Է¹��� ���� ��ȹ ����Ʈ ���, �ε��� ����
                for (int i = 0; i < planLen; ++i) { 
                    int planIdx = *(idxArr + i);
                    char ** arr = (char**)malloc(sizeof(char*) * 7); 
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    } 
                    GetPlan(arr, planIdx);
                    printf("\n--------------------------------------\n");
                    printf("[ Plan %d ] : %s\nexplain : %s\n\n", i + 1, *(arr + 2), *(arr + 3));
                    if (GetPlanDetailLen(planIdx)) {
                        printf("[ Plan Detail ]\n");
                        GetPlanDetail(planIdx);
                    }
                    else printf("(There is no plan detail in this plan.)\n");
                    printf("--------------------------------------\n");
                }
                MakeLog(fp, userIdx, "check plan success");
                break; }

            case 5 : { // ���� Ȯ��
                printf("Seleted Check Review.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //��ȹ�� �ϳ��� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check reiew fail ( no exist plan )");
                    break;
                }
                printf("Please select the date of the review to check (format : yyyy-mm-dd) : ");
                char date[20];
                scanf("%s", date); //���� Ȯ���� ��¥ �Է�
                int planLen = GetDayPlanLen(userIdx, date); // �������� �Ҵ� ���� �Լ�

                if (!planLen) { //������ ��¥�� ��ȹ�� �������� ���� ��
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); //�������� �Ҵ�
                GetDayPlan(userIdx, date, idxArr); //�Է¹��� ���� ��ȹ ����Ʈ ���, �ε��� ����
                printf("planLen : %d\n", planLen); //��ȹ ���� ���
                for (int i = 0; i < planLen; ++i) {
                    int planIdx = *(idxArr + i);
                    char ** arr = (char**)malloc(sizeof(char*) * 7);
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    }
                    GetPlan(arr, planIdx); //��� ��ȹ ���
                    printf("\n--------------------------------------\n");
                    printf("[ Plan %d ] : %s\nexplain : %s\n\n", i + 1, *(arr + 2), *(arr + 3));
                    if (GetPlanReviewLen(planIdx)) {
                        printf("[ Plan Review ]\n"); 
                        GetPlanReview(planIdx); //���� ��������
                    }
                    else printf("(There is no review in this plan.)\n");
                    printf("--------------------------------------\n\n");
                }
                MakeLog(fp, userIdx, "check review success");
                break; }

            case 6 : { // ģ�� ��ȹ Ȯ��
                printf("Seleted Check Friend Plan.\n\n");
                int friendLen = GetFriendLen(userIdx); //ģ�� �� üũ�� �������� �Ҵ� ���� �ʿ��� �Լ�
                if (!friendLen) { //ģ���� �ϳ��� �������� ���� ��
                    printf("Friend does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friend )");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * friendLen); //�������� �Ҵ�
                printf("--------------------------------------\n");
                GetFriend(idxArr, userIdx); //ģ�� �ҷ�����
                printf("--------------------------------------\n"); 
                printf("Select the number : ");
                int number;
                scanf("%d", &number); //��ȹ Ȯ���� ģ���� idx ����
                if (number > friendLen || number < 0) { //idx�� ��� �� ���ý�
                    printf("Number does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friendIdx )");
                    break;
                }
                --number;
                int friendIdx = *(idxArr + number); 
                int planLen = GetPlanLen(friendIdx); //������ ģ���� ��ȹ ���� �ҷ�����
                if (!planLen) { //ģ���� ��ȹ�� �������� ���� ��
                    printf("Friend's plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friend planIdx )");
                    break;
                }
                free(idxArr);
                int * idxArr2 = (int*)malloc(sizeof(int) * planLen); // ������ ģ���� ��ȹ �ҷ��� ������ �����迭 �Ҵ�
                printf("\n--------------------------------------\n");
                GetFriendPlan(userIdx, friendIdx, idxArr2); //������ ģ���� ��ȹ �ҷ�����
                printf("--------------------------------------\n");
                printf("Select the number : ");
                scanf("%d", &number); //�� ���������� Ȯ���� ��ȹ�� �ε��� ����
                if (number > planLen || number < 0) { //������ ��ȣ�� ��ȹ�� �������� ���� ��
                    printf("Number does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( wrong number )");
                    break;
                }
                --number;
                int planIdx = *(idxArr2 + number); 
                char ** arr = (char**)malloc(sizeof(char*) * 7); //������ ģ���� ��ȹ�� ��� value �ҷ����� ���� ������ �����迭 �Ҵ�
                for(int i = 0; i < 7; ++i) {
                    *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                }
                GetPlan(arr, planIdx); //������ ģ�� ��ȹ�� ��� value �ҷ�����
                printf("\n-------------------------------------\n");
                printf("[ Plan ]\n");
                printf("name : %s (~%s)\nexplain : %s\n", *(arr+2), *(arr+6), *(arr+3)); //ģ�� ��ȹ Ȯ��
                if (GetPlanDetailLen(planIdx)) { //������ ģ�� ��ȹ�� ���ΰ�ȹ�� ������ ����
                    printf("[ Plan Detail ]\n");
                    GetPlanDetail(planIdx); //ģ���� ���ΰ�ȹ �ҷ�����
                } else printf("\n(There is no plan detail in this plan.)\n"); 
                printf("--------------------------------------\n\n");
                printf("Make review for this plan (max 100 char, exit : 'end') : ");
                char input[100];
                scanf("%s", input); //���� �����
                if (!strcmp(input, "end")) { //end �Է½� ����
                    printf("Ends review about friend's plan.\n");
                    break;
                }
                char content[100];
                strcpy(content, input); 
                int score;
                while (1) {
                    printf("Please enter score (1~5) : ");
                    scanf("%d", &score); //���� �߰�
                    if (score > 5 || score < 0) { //���� ��� �� �Է½�
                        printf("Invaild score.\n\n");
                        continue;
                    }
                    break;
                }
                MakePlanReview(planIdx, userIdx, content, score); //���� ����
                printf("successfully added a review.\n\n");
                
                free(idxArr2);
                MakeLog(fp, userIdx, "check friend plan success ");
                break; }

            case 7 : { //ģ�� �߰�
                printf("Selected Add Friend.\n\n");
                int friendIdx;
                printf("Enter friend's userIdx : ");
                scanf("%d", &friendIdx); //�߰��� ģ�� �ε��� �Է�
                int check = CheckUser(friendIdx); //ģ�� ���� Ȯ��
                if (!check) { //ģ���� �������� ���� ��
                    printf("userIdx not exists.\n\n");
                    break;
                }
                MakeFriend(userIdx, friendIdx); //ģ�� �߰�
                printf("Successfully friend added.\n\n");
                break; }

            case 8 : { // ç���� �����
                printf("Selected Add Challenge.\n\n");
                char depositName[20], endAt[20];
                int money;
                printf("Please enter a challenge name(20 char maximum) : ");
                scanf("%s", depositName); //ç���� �̸� ����
                printf("Please enter money to deposit : ");
                scanf("%d", &money); //�δ� ��ݾ� ����
                printf("Please enter a challenge deadline (format: yyyy-mm-dd) : ");
                scanf("%s", endAt); //ç���� �Ⱓ ����
                MakeChallenge(depositName, money, endAt); //ç���� ����(�̸�, ��ݾ�, �Ⱓ �߰�)
                int depositIdx = CheckLastDepositIdx(); //�ֱ� ������ ç���� idx�� depositIdx�� ����
                MakeChallengeUser(depositIdx, userIdx); //ç������ ���� �̸�, ���� �ε��� �߰�

                printf("Successfully challenge added.\nif you want to join another people, take this idx : %d\n\n", depositIdx);
                MakeLog(fp, userIdx, "make challenge success");
                break; }

            case 9 : { // ç���� ����
                printf("Selected Join Challenge.\n\n");
                int depositIdx;
                printf("Please enter idx to join : ");
                scanf("%d", &depositIdx); // ������ ç���� �ε��� ����
                int userLen = GetChallengeUserLen(depositIdx); //ç���� �������� ���� �� �ҷ�����
                if (!userLen) { //���� �������� ������
                    printf("Idx does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "join challenge fail ( no exist depositIdx )");
                    break;
                }
                int check = IsChallengeUser(depositIdx, userIdx); //������ ç���� ���������� �ƴ��� üũ
                if (check) { //check = 1 �� ��, �̹� �������� ç���� �Դϴ�. print�� ������
                    printf("You already joined this challenge. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "join challenge fail ( already joined )");
                    break;
                }
                printf("--------------------------------------\n");
                printf("[ %d people joined ]\n", userLen);
                GetChallenge(depositIdx); // ç���� �ҷ�����
                printf("--------------------------------------\n");
                MakeChallengeUser(depositIdx, userIdx); // ç���� ����
                printf("Successfully Joined.\n\n");
                MakeLog(fp, userIdx, "join challenge success");
                break; }

            case 10 : { // ç���� Ȯ��
                printf("Selected Check Challenge.\n\n");
                int len = GetChallengeLen(userIdx); // �������� ç���� ���� ���
                int * idxArr = (int*)malloc(sizeof(int) * len); // ç���� �޾ƿ� �����迭�Ҵ�
                GetChallengeIdx(idxArr, len, userIdx); //ç���� �ε��� ��� �ҷ�����
                for (int i = 0; i < len; ++i) { //��� ç������ ç���� �������� ���� �� print
                    int depositIdx = *(idxArr + i);
                    int userLen = GetChallengeUserLen(depositIdx); 
                    printf("\n--------------------------------------\n");
                    printf("[ %d people joined ]\n", userLen); 
                    GetChallenge(depositIdx); 
                    printf("--------------------------------------\n");
                    MakeLog(fp, userIdx, "check challenge success");
                }
                break; }
            default : // 1~10 ���� �ٸ� ���� �Է½�
                printf("Please check your number.\n\n");
                continue;
        }
        continue;
    }

    mysql_close(connection);
    fclose(fp);

    return 0;
}

void MakeLog(FILE * fp, int userIdx, char content[]) {
    char log[1024];
    struct tm *t;
    char time[40] = GetISOTime(t);
    sprintf(log, "[%s] %d : %s\n", time, userIdx, content,)
    fputs(log, fp);
}

char* GetISOTime(struct tm *t) {
    static char s[20];
    sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec
            );
    return s;
}

int IsLeafYear(int year) { 
	if(year % 400 == 0)
		return 1;
	if((year % 100 != 0) && (year % 4 == 0))
		return 1;
	return 0;
}
/*
��year : �⵵ �޾ƿ�
4, 100���� �������� ����, �ƴϸ� ���
�����̸� 1����, ����̸� 0����
*/

int getDay(int year, int month) { 
    if (month < 1 || month > 12) return 0;
    int day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4) == 0 && (year % 100) != 0 || (year % 400) == 0) day[1] = 29; //�����̸� 2���� 29�ϱ���
    return day[month - 1]; 
} 
/*
��year : �⵵ �޾ƿ�
��month : �� �޾ƿ�
�� �� ��¥�� ��� �� �ش� �޿� �´� ��¥�� ����
*/

int getStartDay(int year, int month) {
	int past = 0;
	for(int y = 1; y < year; ++y)
		past = past + 365 + IsLeafYear(y);
	for(int m = 1; m < month; ++m)
		past = past + getDay(year, m);
	return (1 + past) % 7;
} 
/*
��year : �⵵ �޾ƿ�
��month : �� �޾ƿ�
���� �����ϴ� ���� ����
*/

void printCalendar(int year, int month) {
    int day = getDay(year, month);
    int start_day = getStartDay(year, month);
    printf("           [ %d - %d ]            \n\n", year, month);
    printf("    S    M    T    W    T    F    S\n\n");
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
/*
��year : �⵵ �޾ƿ�
��month : �� �޾ƿ�

*/

int SignUp(char name[]) { //���� �̸� ����
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User�� �߰��ϴ� ������
    query_stat = mysql_query(connection, query);
    if (!query_stat) { //���� ���� ���ϸ� 1 return
        return 1;
    } else { //������ �̹� �����ϸ� ����ȭ�� �߸鼭 0 return
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}
/*
��name : ȸ������ �� ���� �̸�
�̹� ���� ����� �����ϸ� ���� print, ȸ������ �����ϸ� 1 ����

*/

int CheckLastUserIdx() {
    sprintf(query, "SELECT MAX(userIdx) FROM User"); // ������ UserIdx �� ���� ū �� �������� �޾ƿ���
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0]; //userIdx �����ϱ� ���� ������ ����
    return atoi(res);  //useridx ����
}
/*
������ UserIdx �� ���� ū �� �������� �޾ƿͼ� ���
�ҷ����� �����ϸ� UserIdx �� ���� ū �� ���, �ƴ϶�� �����ϸ� ���� print
*/

int CheckUser(int userIdx) {
    sprintf(query, "SELECT EXISTS (SELECT 1 FROM User WHERE userIdx = %d) AS cnt", userIdx);  //useridx ���̺��� ����ڰ� ������ useridx�ҷ����� ������
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection); //��� ����
    char * a;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //���� ���� ������ ��ȯ
        a = sql_row[0];
    }
    return atoi(a); //���� ����� useridx ����
}
/*
��userIdx : ���翩�� üũ�� ������ idx
���� ���� ���ҽ� 0 ����, ����� useridx ����
*/

int PrintUser() {
    sprintf(query, "SELECT * FROM User"); // User ���̺� �ȿ� �ִ� ��� ���� �������� ������
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection); //��� ����
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //���� ���� ������ ��ȯ
        printf("%s : %s\n", sql_row[0], sql_row[1]); //���� �ε����� ���� print
    }
    printf("--------------------------------------\n\n");
    return 1;
}
/*
�����ε����� ���� print
*/

int MakeFriend(int userIdx, int friendIdx) {
    sprintf(query, "INSERT INTO Friend VALUES (%d, %d), (%d, %d)", userIdx, friendIdx, friendIdx, userIdx); //ģ�� �߰� ����
    query_stat = mysql_query(connection, query);
    if (!query_stat) { //�ҷ����� ������ 1����
        return 1;
    } else { //�ҷ����� ���н� ���� ����ϸ� 0 ����
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}
/*
��userIdx :
��friendIdx : 


*/

int GetFriendLen(int userIdx) {
    sprintf(query, "SELECT COUNT(friendIdx) FROM Friend WHERE userIdx = %d", userIdx); //������ ģ�� �� �ҷ�����
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)  //�ҷ����� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    char * res; 
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0]; //ģ�� �� �����ϱ� ���� ������ ���� 
    return atoi(res); //ģ�� �� ����
}
/*
��userIdx : user�� ģ�� �� �ҷ����� ���� userIdx
������ ģ�� �� �ҷ����� ������ ģ�� �� ����, ���н� ���� ����ϸ� 0 ����
*/

int GetFriend(int * idxArr, int userIdx) {
    sprintf(query, "SELECT f.friendIdx, u.userName FROM Friend f, User u WHERE f.userIdx = %d AND f.friendIdx = u.userIdx", userIdx); //ģ�� �ε����� �ش��ϴ� �̸��� �ҷ���
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)  //�ҷ����� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    int i = 0; 
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //���� ���� ������ ��ȯ
        printf("[ %d ] %s ( idx : %s )\n", i+1, sql_row[1], sql_row[0]); //[ģ���� ��ȹ �ε���] ģ�� (idx : ģ���� ���� �ε���)
        idxArr[i] = atoi(sql_row[0]);
        ++i;
    }
}
/*
��idxArr : ������ friendidx �ҷ��� �迭
��userIdx : user�� ģ�� �ҷ����� ���� userIdx
��� ģ�� �ҷ����� �����ϸ� ģ���� ģ���� idx print, ���н� ���� ����ϸ� 0����
*/

int IsFriend(int userIdx, int friendIdx) {
    sprintf(query, "SELECT EXISTS (SELECT * FROM Friend WHERE userIdx = %d AND friendIdx = %d)", userIdx, friendIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //�߰� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { 
        return atoi(sql_row[0]);
    }
}
/*
userIdx�ȿ� friendIdx �ִ��� Ȯ��, Ȯ�� ���н� ���� ����ϸ� 0 ����
Ȯ�� ������ ������ 0���� , ������ friendIdx ����
*/

int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]) { //
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User�� �߰��ϴ� ������
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //�߰� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1; //�߰� ������ 1����
}
/*
��userIdx : ������ ��ġ
��planName : ��ȹ �̸�
��explain : ��ȹ ����
��openLevel : ���� ����
��endAt : ���� ����
��ȹ �߰� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int CheckLastPlanIdx() {
    sprintf(query, "SELECT MAX(planIdx) FROM Plan"); //planIdx �� ���ڰ� ���� ū idx(���� �������� �߰��� idx) �������� ������
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)  //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
���� �ֱٿ� ����� planidx �ҷ��� �����ϸ� ���� �ֱ� ����� planidx ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
userIdx : ������ idx
������ ��ȹ ���� �ҷ����� �����ϸ� ��ȹ ���� ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetPlanIdx(int userIdx, int * idxArr) { 
    sprintf(query, "SELECT planIdx, planName, endAt FROM Plan WHERE userIdx = %d", userIdx); //������ useridx �� planIdx, planName, endAt�� ��� �ҷ����� ������
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)  //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //�� ���� ������ �ҷ���
        char *ptr = strtok(sql_row[2], " ");
        printf("[ %d ] : %s (~%s)\n", i + 1, sql_row[1], ptr);  // [�ε���] : �̸� (~��������) print
        idxArr[i] = atoi(sql_row[0]); //idxArr�� planIdx ����
        ++i;
    }
    return 1;
}
/*
userIdx : plan ������ ������ idx
idxArr : ������ ������ planidx�� ����
������ ������ ��ȹ�ε���, ��ȹ��, �������� ��� �ҷ��ͼ� print �� idxArr�� planidx �����ϰ� 1����
�ҷ����� ���н� 0�� �����ϸ� ���� print
*/

int GetPlan(char ** arr, int planIdx) {
    sprintf(query, "SELECT * FROM Plan WHERE planIdx = %d", planIdx); //�Է��� planidx�� value�� �ҷ����� ������
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //���� ���ҽ� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //�� ���� ������ �ҷ���
        for (int i = 0; i < 7; ++i) {
            if (i == 6) {
                char *ptr = strtok(sql_row[i], " ");
                *(arr + i) = ptr;
            }
            else *(arr + i) = sql_row[i];
        }
    }
    return 1;
}
/*
��**arr : plan�� �ִ� �����͸� ��� �������� ���� ���� ������ �迭
��**planIdx : ������ ������ planidx
�Է��� planidx�� ��� value�� arr�� �ҷ��� �ҷ����� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetFriendPlan(int userIdx, int friendIdx, int * idxArr) {
    int access = IsFriend(userIdx, friendIdx);
    if (!access) return 0; //ģ������ �ƴ��� Ȯ�� �� ģ�� �ƴϸ� 0 ����
    /*
    1 : ��ü ����, 2 : ģ�� ����, 3 : ���� ����
    */
    sprintf(query, "SELECT planIdx, planName, endAt FROM Plan WHERE userIdx = %d AND openLevel < 3", friendIdx); 
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
        idxArr[i] = atoi(sql_row[0]);
        printf("[ %d ] %s (~%s)\n", i+1, sql_row[1], sql_row[2]); //[�ε���] ģ���ǰ�ȹ�� (~��������)
        ++i;
    }
    return 1;
}
/*
��userIdx : ������ idx
��friendIdx : ��ȹ Ȯ���� ģ���� idx
��idxArr : ģ���� planidx ����
ģ������, ģ���� ��ȹ�� ��ü������ ģ���������� Ȯ���� ��, Ȯ�� �����ϸ� idxArr�� ģ���� planidx �����ϰ�, ģ���� ��ȹ�� ���� print �ϸ� 1����
�ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetDayPlanLen(int userIdx, char date[]) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date); //������ ��¥�� �ִ� plan ���� �ҷ�����
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //�ҷ����� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
��userIdx : ������ idx
��date : ����ڰ� Ȯ���� ��¥
����ڰ� ������ ��¥�� ��ȹ ���� �ҷ����� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetDayPlan(int userIdx, char date[], int * idxArr) {
    sprintf(query, "SELECT planIdx, planName, `explain` FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date); // ������ ��¥�� �ִ� ��ȹ�ε���, ��ȹ��, ��ȹ����,�������� �ҷ�����
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //�ҷ����� ���н� ���� ����ϸ� 0 ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //��� ����
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //�� ���� ������ �ҷ���
        idxArr[i] = atoi(sql_row[0]); 
        ++i;
    }
    return 1;
}
/*
��userIdx : ������ idx 
��date : ������ ������ ��¥
��idxArr : ������ ��¥�� �ִ� ��ȹ�� idx�� �����ϱ� ���� �迭
������ ������ ��¥�� �ִ� ��ȹ�� idx�� idxArr�� ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/


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
/*
��userIdx : ������ idx
��planIdx : ������ ������ ��ȹ�� idx
������ ������ ��ȹ ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
��planIdx : ������ ������ ��ȹ�� idx
��planName : ������ �̸� 
��explain : ������ ����
��endAt : ������ �Ⱓ
������ ������ ��ȹ�� �޾Ƽ� �̸�, ����, �Ⱓ ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/


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
/*
��planIdx : ���ΰ�ȹ �߰��� ū ��ȹ�� idx
��datailName : ���ΰ�ȹ��
��startedAt : ���ΰ�ȹ ���� ��¥
��endAt : ���ΰ�ȹ ���� ��¥
��where : ���ΰ�ȹ ���� ��ġ
���ΰ�ȹ ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int CheckLastDetailIdx() { 
    sprintf(query, "SELECT MAX(detailIdx) FROM Plandetail"); //DetailIdx �� ���ڰ� ���� ū idx(���� �������� �߰��� idx) �������� ������
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //�ҷ����� ���н� 0 �����ϸ� ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
���� �ֱٿ� ����� DetailIdx �ҷ����� �����ϸ� ���� �ֱ� ����� DetailIdx ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
planIdx : ������ ������ ��ȹ
������ ������ ��ȹ �� ���ΰ�ȹ ���� �ҷ����� �����ϸ� ���ΰ�ȹ �� ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
        char * ptr1 = strtok(sql_row[1], " ");
        char * ptr2 = strtok(sql_row[2], " ");
        printf("%d. name : %s (%s~%s)\nplace : %s\n", i + 1, sql_row[0], ptr1, ptr2, sql_row[3]); 
        ++i;
    }
    return 1;
}
/*
��**planIdx : ������ ������ planidx
�Է��� planidx�� ��� detailplan�� �ҷ��� �ҷ����� �����ϸ� ���ΰ�ȹ�̸�, ���۽ð�, �����ð�, ��� print �� 1����
�ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
��planIdx : plandetail�� �������� ���� planIdx
������ ������ plan�� ��� plandetail���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
��detailIdx : ������ �����ϱ�� ������ detailIdx
������ ���ΰ�ȹ ���� �����ϸ� ���� ���� print , �ƴ϶�� ���� ����ϸ� ����
*/

void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]) {
    sprintf(query, "UPDATE Plandetail SET detailName = '%s', startedAt = '%s', endAt = '%s', `where` = '%s' WHERE detailIdx = %d", detailName, startedAt, endAt, where, detailIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return ;
    }
    sql_result = mysql_store_result(connection);
    printf("successfully modified.");
}
/*
��detailIdx : ������ ������ ���ΰ�ȹ�� Idx
��detailName : ���ΰ�ȹ��
��startedAt : ���ΰ�ȹ ���� ��¥
��endAt : ���ΰ�ȹ ���� ��¥
��where : ���ΰ�ȹ ���� ��ġ
������ ������ ���ΰ�ȹ�� �޾Ƽ� �̸�, ���۳�¥, ������¥, ��ġ ���� �����ϸ� ���� ���� print, �ƴ϶�� ���� ����ϸ� ����
*/

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
/*
��planIdx : ���� �߰��� plan�� idx
��userIdx : ���� �߰��� user�� idx
��content : ����
��score : ����
ģ�� ��ȹ�� ���� ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetPlanReviewLen(int planIdx) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Planreview WHERE planIdx = %d", planIdx);
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
/*
��planIdx : ������ ������ ���� ������ ��ȹ�� idx
���� ���� �������� �����ϸ� ���� ���� ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetPlanReview(int planIdx) {
    sprintf(query, "SELECT userIdx, content, score, createdAt FROM Planreview WHERE planIdx = %d", planIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection);
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        int score = atoi(sql_row[2]);
        printf("%d. user : %s [ %d / 5 ] (%s)\ncontent : %s\n", i + 1, sql_row[0], score, sql_row[3], sql_row[1]); 
        ++i;
    }
    return 1;
}
/*
��planidx : ������ ������ ���� ������ ��ȹ�� idx
���信 �ִ� userIdx, content, score, createdAt ��� �������� �����ϸ� print �ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

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
/*
��planreviewIdx : ������ �����ϱ�� ������ ������ idx
������ ���� ���� �����ϸ� ���� ���� print, �ƴ϶�� ���� ����ϸ� ����
*/

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
/*
��planreviewIdx : ������ ������ ������ Idx
��content : ���� ����
��score : ����
������ ������ ���並 �޾Ƽ� ���䳻��, ���� ���� �����ϸ� ���� ���� print, �ƴ϶�� ���� ����ϸ� ����
*/

int MakeChallenge(char depositName[], int money, char endAt[]) {
    sprintf(query, "INSERT INTO Deposit (depositName, money, endAt) VALUES ('%s', %d, '%s')", depositName, money, endAt);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}
/*
��depositName : ç���� ��
��money : ��ݾ�
��endAt : ��������
ç���� ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int MakeChallengeUser(int depositIdx, int userIdx) {
    sprintf(query, "INSERT INTO Deposituser (depositIdx, userIdx) VALUES (%d, %d)", depositIdx, userIdx);
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1;
}
/*
�� depositIdx : ç������ �ҷ��� idx
�� userIdx : ç������ ������ ������ idx
> ç������ ���� ���� �����ϸ� 1����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int CheckLastDepositIdx() { 
    sprintf(query, "SELECT MAX(depositIdx) FROM Deposit"); //DepositIdx �� ���ڰ� ���� ū idx(���� �������� �߰��� idx) �������� ������
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //�ҷ����� ���н� 0 �����ϸ� ����
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
���� �ֱٿ� ����� ç������ idx�ҷ��� ç���� idx����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int IsChallengeUser(int depositIdx, int userIdx) {
    sprintf(query, "SELECT EXISTS (SELECT * FROM Deposituser WHERE userIdx = %d AND depositIdx = %d)", userIdx, depositIdx);
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
/*
��depositIdx : ���� ���� Ȯ���� ç���� idx
��userIdx : ���� ���� Ȯ���� ���� idx
ç���� ���� ���� Ȯ�� �����ϸ� ���� idx ����, �ƴ϶�� ���� ����ϸ� 0 ����
*/

int GetChallengeLen(int userIdx) {
    sprintf(query, "SELECT COUNT(depositIdx) FROM Deposituser WHERE userIdx = %d", userIdx);
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
/*
��userIdx : user�� ģ�� �� �ҷ����� ���� userIdx
ç���� �� �ҷ����� ������ ģ�� �� ����, ���н� ���� ����ϸ� 0 ����
*/

int GetChallengeUserLen(int depositIdx) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Deposituser WHERE depositIdx = %d", depositIdx);
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
/*
�� depositIdx : ç������ ������ user �� �ҷ����� ���� idx
> ���� �� �ҷ����� �����ϸ� ���� �� ����, �ƴ϶�� 0�� �����ϸ� ���� print 
*/


int GetChallengeIdx(int *idxArr, int len, int userIdx) {
    sprintf(query, "SELECT depositIdx FROM Deposituser WHERE userIdx = %d", userIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        for (int i = 0; i < len; ++i) {
            idxArr[i] = atoi(sql_row[0]);
        }
    }
    return 1;
}
/*
idxArr : ������ �������� challengeIdx�� ����
len : �������� ç���� ����
userIdx : ������ idx
������ �������� challengeidx ��� �ҷ��ͼ� idxArr�� �����ϰ� 1����, �ҷ����� ���н� 0�� �����ϸ� ���� print
*/

int GetChallenge(int depositIdx) {
    sprintf(query, "SELECT depositName, money, endAt FROM Deposit WHERE depositIdx = %d", depositIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection);
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) {
        printf("Challenge Name : %s \nChallenge Money : %s\nDead Line : %s\n", sql_row[0], sql_row[1], sql_row[2]); 
    }
    return 1;
}
/*
�� depositIdx : ç������ �ҷ��� idx
> �ҷ����� �����ϸ� 1 ����, �ƴ϶�� 0�� �����ϸ� ���� print 
*/