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

void MakeLog(FILE* fp, int userIdx, char content[]);
char* GetISOTime(struct tm *t);

// [ 달력 출력 관련 함수]
int IsLeafYear(int year); // 윤년인지 체크
int getDay(int year, int month); // 월별 날짜 수가 몇인지 리턴
int getStartDay(int year, int month); // 달력에서 1일인 요일 리턴
void printCalendar(int year, int month); // 달력 출력

// [ 유저 관련 함수 ]
int SignUp(char name[]); // 회원가입
int CheckLastUserIdx(); //가장 최근에 추가된 user의 idx 불러오기
int CheckUser(int userIdx); // 유저 존재 확인
int printUser(); // 유저 리스트 출력
int MakeFriend(int userIdx, int friendIdx); // 친구 추가
int GetFriendLen(int userIdx); //친구 수 불러오기
int GetFriend(int * idxArr, int userIdx); // 친구 불러오기
int IsFriend(int userIdx, int friendIdx); // 친구인지 아닌지 확인

// [ 계획 관련 함수 ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // 계획 생성
int CheckLastPlanIdx(); //가장 최근 만든 계획의 인덱스 불러오기
int GetPlanLen(int userIdx); // 유저가 생성한 계획의 수 리턴
int GetPlanIdx(int userIdx, int * idxArr); // 유저가 생성한 계획의 인덱스를 arr에 저장
int GetPlan(char ** arr, int userIdx); // 유저가 생성한 계획 리스트 출력
int GetFriendPlan(int userIdx, int friendIdx, int * idxArr); // 친구가 생성한 계획 리스트 출력
int GetDayPlanLen(int userIdx, char date[]); //유저가 선택한 날짜의 계획 개수 불러오기
int GetDayPlan(int userIdx, char date[], int * idxArr); // 특정 날의 계획 리스트 출력 및 인덱스 리턴
int DeletePlan(int userIdx, int planIdx); // 계획 삭제
int ModifyPlan(int planIdx, char planName[], char explain[], char endAt[]); // 계획 수정

// [ 세부 계획 관련 함수 ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 생성
int CheckLastDetailIdx(); //가장 최근 만든 세부계획 인덱스 불러오기
int GetPlanDetailLen(int planIdx); // 유저가 생성한 계획 디테일의 수 불러오기
int GetPlanDetail(int planIdx); // 유저가 생성한 계획 디테일 리스트 출력
int DeletAllePlandetail(int planIdx); // 모든 세부계획 삭제
void DeletePlandetail(int detailIdx); // 계획 디테일 삭제
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 수정

// [ 계획 리뷰 관련 함수 ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score); // 계획 리뷰 생성
int GetPlanReviewLen(int planIdx); //리뷰 개수 불러오기
int GetPlanReview(int planIdx); // 계획 리뷰 출력
void DeletePlanReview(int planreviewIdx); // 리뷰 삭제
void ModifyPlanReview(int planreviewIdx, char content[], int score); // 리뷰 수정

//[ 챌린지 관련 함수 ]
int MakeChallenge(char depositName[], int money, char endAt[]); //챌린지 설정
int MakeChallengeUser(int depositIdx, int userIdx); //챌린지 유저 추가
int CheckLastDepositIdx(); //최근 만든 챌린지 인덱스 불러오기
int IsChallengeUser(int depositIdx, int userIdx); //챌린지 유저인지 확인
int GetChallengeLen(int userIdx); //챌린지 수 불러오기
int GetChallengeUserLen(int depositIdx); //챌린지에 참여하는 유저 수 불러오기
int GetChallengeIdx(int *idxArr, int len, int userIdx);  //참여중인 챌린지 idx불러오기
int GetChallenge(int depositIdx); // 챌린지 출력


int userIdx;

int main(void) {
    
    mysql_init(&conn);

    connection = mysql_real_connect(&conn, HOST, USER, PASSWORD, DB, 3306, NULL, 0); // 서버와 통신 시작

    if (connection == NULL) {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
        return 1;
    } //서버 연결 안될시 에러 화면

    FILE* fp = fopen("log.txt","a"); // 로그를 남기기 위한 파일 생성
    char logContent[1024];

    while (1) { //로그인, 회원가입
        printf("| 1. SignIn | 2. SignUp |\nNumber to Execute : ");
        int func, a;
        scanf("%d", &func);
        switch (func) {
            case 1 : //로그인
                printf("userIdx to signin : "); //유저 인덱스로 로그인
                scanf("%d", &a);
                if (CheckUser(a)) { //회원가입 되어있는 유저인지 확인
                    userIdx = a;
                    printf("Success Signin userIdx : %d\n", userIdx);
                    MakeLog(fp, userIdx, "signin");
                    break;
                }
                else { //회원가입 안되어 있을 때
                    printf("This userIdx does not exist. Please check userIdx.\n");
                    continue;
                }

            case 2 : //회원가입
                printf("Please enter a name : ");
                char name[30];
                scanf("%s", name);
                int sign = SignUp(name);
                if (sign) {
                    int idx = CheckLastUserIdx(); //최근 추가한 유저의 인덱스 불러오기
                    printf("successfully registered.\nuserIdx : %d (SignIn to userIdx)\n", idx);
                    MakeLog(fp, idx, "signup");
                    continue;
                } else {
                    printf("error");
                    continue;
                }

            default : // 1, 2가 아닌 다른 입력 들어왔을 때
                printf("Please check the number.\n");
                continue;
        }
        break;
    }

    printf("\n\n");

    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    printCalendar(t->tm_year + 1900, t->tm_mon + 1); //달력 출력

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
        if (function == 0) {  //종료
            printf("Bye.");
            MakeLog(fp, userIdx, "signout");
            break;
        }
        switch (function) {
            case 1 : { // 계획 추가
                printf("Selected Add Plan.\n\n");
                char planName[20], explain[1024], endAt[20];
                int openLevel;
                printf("Please enter a plan name(20 char maximum) : "); 
                scanf("%s", planName); //계획 이름 설정
                printf("Please enter a description of the plan (1024 char maximum) : ");
                scanf("%s", explain); //계획 설명
                while (1) {
                    printf("Please enter the openLevel of the plan (1 : Full disclosure, 2 : Friend disclosure, 3 : Only me) : ");
                    scanf("%d", &openLevel); //나만보기 일부공개 전체공개 설정
                    if (openLevel > 0 && openLevel < 4) break;
                    else { //1, 2, 3이 아닌 다른 입력 들어왔을 때 재입력
                        printf("Please enter only numbers from 1 to 3.\n");
                        continue;
                    }
                }
                printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                scanf("%s", endAt); //계획 마감 날짜
                MakePlan(userIdx, planName, explain, openLevel, endAt); // 계획 생성
                int planIdx = CheckLastPlanIdx(); //세부계획 추가 위치를 정하기 위함
                printf("Plan has been saved.\n");
                while (1) { //세부계획 추가
                    printf("Please enter a detail plan name(if want end enter 'end') : ");
                    char input[20];
                    scanf("%s", input);
                    if (!strcmp(input, "end")) { //세부계획 추가 끝
                        printf("Ends detail plan registration.\n");
                        break;
                    }
                    char detailName[20], startedAt[20], endAt[20], where[20];
                    strcpy(detailName, input); //세부계획 이름 설정
                    printf("Please enter a date to start (format: yyyy-mm-dd) : ");
                    scanf("%s", startedAt); //세부계획 시작 날짜 설정
                    printf("Please enter the deadline (format: yyyy-mm-dd) : ");
                    scanf("%s", endAt); //세부계획 마감 날짜 설정
                    printf("Please enter a place to run : ");
                    scanf("%s", where); // 세부계획 실행 장소 설정 ex) cafe, studyroom 
                    MakePlanDetail(planIdx, detailName, startedAt, endAt, where); //세부계획 생성
                    printf("successfully added a detail plan.\n\n");
                    continue;
                }
                printf("%s has been added to %s.\n\n", planName, endAt);
                MakeLog(fp,  userIdx, "add plan success");
                break; }

            case 2 : { //계획 삭제
                printf("Selected Delete plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //계획이 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "delete plan fail ( no exist plan )");
                    break;
                } //이전화면으로 돌아감
                idxArr = (int*)malloc(sizeof(int) * planLen); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                GetPlanIdx(userIdx, idxArr); //계획 불러오기
                printf("--------------------------------------\nnumber(cancel : 0) : "); 
                while (1) { //계획 삭제
                    int num;
                    scanf("%d", &num); //삭제할 게획 선택
                    if (num == 0) { //함수 끝내기
                        printf("delete canceled.\n\n");
                        MakeLog(fp, userIdx, "delete plan cancel");
                        break;
                    }
                    if (num < 0 || num > planLen) { //계획 안에 있는 수 외에 다른 수를 선택했을 때
                        printf("Please enter a valid number.\nNumber of plan to delete : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num); //planIdx에 사용자가 삭제할 값 지정
                    DeletAllePlandetail(planIdx); // 큰 계획 삭제
                    DeletePlan(userIdx, planIdx); // 세부계획 삭제
                    printf("Plan has been deleted.\n\n");
                    MakeLog(fp, userIdx, "delete plan success");
                    break;
                }
                break; }

            case 3 : { // 계획 수정
                printf("Selected Modify Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //계획이 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "modify plan fail ( no exist plan )");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); 
                printf("Please select the number of the plan to delete.\n--------------------------------------\n");
                GetPlanIdx(userIdx, idxArr); // 유저가 생성한 계획의 인덱스를 arr에 저장
                printf("--------------------------------------\nnumber(cancel : 0) : ");
                while (1) { //수정할 게획 선택
                    int num;
                    scanf("%d", &num);
                    if (num == 0) { //함수 끝내기
                        printf("modify canceled.\n\n");
                        MakeLog(fp, userIdx, "modify plan cancel");
                        break;
                    }
                    if (num < 0 || num > planLen) { //계획이 없는 인덱스 선택시
                        printf("Please enter a valid number.\nNumber of plan to modify : ");
                        continue;
                    }
                    --num;
                    int planIdx = *(idxArr + num); //planIdx에 사용자가 삭제할 값 지정
                    char ** arr = (char**)malloc(sizeof(char*) * 7);
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    }
                    GetPlan(arr, planIdx); //모든 계획 arr에 불러오기
                    char planName[20], explain[1024], endAt[20];
                    strcpy(planName, *(arr + 2)), strcpy(explain, *(arr + 3)), strcpy(endAt, *(arr +  6)); 
                    printf("selected %d. Choose you wan to modify.\n| 1 : planName | 2 : explain | 3 : deadline | : ", num);
                    int modifyFunc;
                    scanf("%d", &modifyFunc); //수정할 내용 선택
                    switch (modifyFunc) {
                        case 1 : // 계획 이름 수정
                            printf("Please enter a plan name(20 char maximum) : ");
                            scanf("%s", planName);
                            break;
                        case 2 : //계획 설명 수정
                            printf("Please enter a description of the plan (1024 char maximum) : ");
                            scanf("%s", explain);
                            break;
                        case 3 : //계획 기한 수정
                            printf("Please enter a plan deadline (format: yyyy-mm-dd) : ");
                            scanf("%s", endAt);
                            break;
                        default :
                            printf("Please check your number.\n");       
                    }
                    
                    ModifyPlan(planIdx, planName, explain, endAt); //계획 수정
                    printf("Plan has been modified.\n\n");
                    MakeLog(fp, userIdx, "modify plan success");
                    break;
                }
                break; }

            case 4 : { //계획 확인 
                printf("Selected Check Plan.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //계획이 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check plan fail ( no exist plan )");
                    break;
                }
                printf("Please select the date of the plan to check (format : yyyy-mm-dd) : ");
                char date[20];
                scanf("%s", date); // 확인할 날짜 선택
                int planLen = GetDayPlanLen(userIdx, date); //동적변수 할당 위한 함수
                if (!planLen) { //날짜에 계획이 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); //동적변수 할당
                GetDayPlan(userIdx, date, idxArr); //입력받은 날의 계획 리스트 출력, 인덱스 리턴
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

            case 5 : { // 리뷰 확인
                printf("Seleted Check Review.\n\n");
                planLen = GetPlanLen(userIdx);
                if (!planLen) { //계획이 하나도 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check reiew fail ( no exist plan )");
                    break;
                }
                printf("Please select the date of the review to check (format : yyyy-mm-dd) : ");
                char date[20];
                scanf("%s", date); //리뷰 확인할 날짜 입력
                int planLen = GetDayPlanLen(userIdx, date); // 동적변수 할당 위한 함수

                if (!planLen) { //선택한 날짜에 계획이 존재하지 않을 때
                    printf("Plan does not exist. Return to the number selection window.\n\n");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * planLen); //동적변수 할당
                GetDayPlan(userIdx, date, idxArr); //입력받은 날의 계획 리스트 출력, 인덱스 리턴
                printf("planLen : %d\n", planLen); //계획 개수 출력
                for (int i = 0; i < planLen; ++i) {
                    int planIdx = *(idxArr + i);
                    char ** arr = (char**)malloc(sizeof(char*) * 7);
                    for(int i = 0; i < 7; ++i) {
                        *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                    }
                    GetPlan(arr, planIdx); //모든 계획 출력
                    printf("\n--------------------------------------\n");
                    printf("[ Plan %d ] : %s\nexplain : %s\n\n", i + 1, *(arr + 2), *(arr + 3));
                    if (GetPlanReviewLen(planIdx)) {
                        printf("[ Plan Review ]\n"); 
                        GetPlanReview(planIdx); //리뷰 가져오기
                    }
                    else printf("(There is no review in this plan.)\n");
                    printf("--------------------------------------\n\n");
                }
                MakeLog(fp, userIdx, "check review success");
                break; }

            case 6 : { // 친구 계획 확인
                printf("Seleted Check Friend Plan.\n\n");
                int friendLen = GetFriendLen(userIdx); //친구 수 체크와 동적변수 할당 위해 필요한 함수
                if (!friendLen) { //친구가 하나도 존재하지 않을 때
                    printf("Friend does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friend )");
                    break;
                }
                int * idxArr = (int*)malloc(sizeof(int) * friendLen); //동적변수 할당
                printf("--------------------------------------\n");
                GetFriend(idxArr, userIdx); //친구 불러오기
                printf("--------------------------------------\n"); 
                printf("Select the number : ");
                int number;
                scanf("%d", &number); //계획 확인할 친구의 idx 선택
                if (number > friendLen || number < 0) { //idx를 벗어난 수 선택시
                    printf("Number does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friendIdx )");
                    break;
                }
                --number;
                int friendIdx = *(idxArr + number); 
                int planLen = GetPlanLen(friendIdx); //선택한 친구의 계획 개수 불러오기
                if (!planLen) { //친구의 계획이 존재하지 않을 때
                    printf("Friend's plan does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( no exist friend planIdx )");
                    break;
                }
                free(idxArr);
                int * idxArr2 = (int*)malloc(sizeof(int) * planLen); // 선택한 친구의 계획 불러올 이차원 동적배열 할당
                printf("\n--------------------------------------\n");
                GetFriendPlan(userIdx, friendIdx, idxArr2); //선택한 친구의 계획 불러오기
                printf("--------------------------------------\n");
                printf("Select the number : ");
                scanf("%d", &number); //더 세부적으로 확인할 계획의 인덱스 선택
                if (number > planLen || number < 0) { //선택한 번호에 계획이 존재하지 않을 때
                    printf("Number does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "check friend plan fail ( wrong number )");
                    break;
                }
                --number;
                int planIdx = *(idxArr2 + number); 
                char ** arr = (char**)malloc(sizeof(char*) * 7); //선택한 친구의 계획의 모든 value 불러오기 위한 이차원 동적배열 할당
                for(int i = 0; i < 7; ++i) {
                    *(arr + i) = (char*)malloc(sizeof(char) * 1024);
                }
                GetPlan(arr, planIdx); //선택한 친구 계획의 모든 value 불러오기
                printf("\n-------------------------------------\n");
                printf("[ Plan ]\n");
                printf("name : %s (~%s)\nexplain : %s\n", *(arr+2), *(arr+6), *(arr+3)); //친구 계획 확인
                if (GetPlanDetailLen(planIdx)) { //선택한 친구 계획의 세부계획이 있으면 실행
                    printf("[ Plan Detail ]\n");
                    GetPlanDetail(planIdx); //친구의 세부계획 불러오기
                } else printf("\n(There is no plan detail in this plan.)\n"); 
                printf("--------------------------------------\n\n");
                printf("Make review for this plan (max 100 char, exit : 'end') : ");
                char input[100];
                scanf("%s", input); //리뷰 남기기
                if (!strcmp(input, "end")) { //end 입력시 종료
                    printf("Ends review about friend's plan.\n");
                    break;
                }
                char content[100];
                strcpy(content, input); 
                int score;
                while (1) {
                    printf("Please enter score (1~5) : ");
                    scanf("%d", &score); //별점 추가
                    if (score > 5 || score < 0) { //범위 벗어난 수 입력시
                        printf("Invaild score.\n\n");
                        continue;
                    }
                    break;
                }
                MakePlanReview(planIdx, userIdx, content, score); //리뷰 생성
                printf("successfully added a review.\n\n");
                
                free(idxArr2);
                MakeLog(fp, userIdx, "check friend plan success ");
                break; }

            case 7 : { //친구 추가
                printf("Selected Add Friend.\n\n");
                int friendIdx;
                printf("Enter friend's userIdx : ");
                scanf("%d", &friendIdx); //추가할 친구 인덱스 입력
                int check = CheckUser(friendIdx); //친구 존재 확인
                if (!check) { //친구가 존재하지 않을 때
                    printf("userIdx not exists.\n\n");
                    break;
                }
                MakeFriend(userIdx, friendIdx); //친구 추가
                printf("Successfully friend added.\n\n");
                break; }

            case 8 : { // 챌린지 만들기
                printf("Selected Add Challenge.\n\n");
                char depositName[20], endAt[20];
                int money;
                printf("Please enter a challenge name(20 char maximum) : ");
                scanf("%s", depositName); //챌린지 이름 설정
                printf("Please enter money to deposit : ");
                scanf("%d", &money); //인당 모금액 설정
                printf("Please enter a challenge deadline (format: yyyy-mm-dd) : ");
                scanf("%s", endAt); //챌린지 기간 설정
                MakeChallenge(depositName, money, endAt); //챌린지 생성(이름, 모금액, 기간 추가)
                int depositIdx = CheckLastDepositIdx(); //최근 생성된 챌린지 idx를 depositIdx에 저장
                MakeChallengeUser(depositIdx, userIdx); //챌린지에 유저 이름, 유저 인덱스 추가

                printf("Successfully challenge added.\nif you want to join another people, take this idx : %d\n\n", depositIdx);
                MakeLog(fp, userIdx, "make challenge success");
                break; }

            case 9 : { // 챌린지 참여
                printf("Selected Join Challenge.\n\n");
                int depositIdx;
                printf("Please enter idx to join : ");
                scanf("%d", &depositIdx); // 참여할 챌린지 인덱스 선택
                int userLen = GetChallengeUserLen(depositIdx); //챌린지 참여중인 유저 수 불러오기
                if (!userLen) { //유저 존재하지 않을시
                    printf("Idx does not exist. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "join challenge fail ( no exist depositIdx )");
                    break;
                }
                int check = IsChallengeUser(depositIdx, userIdx); //유저가 챌린지 참여중인지 아닌지 체크
                if (check) { //check = 1 일 때, 이미 참여중인 챌린지 입니다. print후 끝내기
                    printf("You already joined this challenge. Return to the number selection window.\n\n");
                    MakeLog(fp, userIdx, "join challenge fail ( already joined )");
                    break;
                }
                printf("--------------------------------------\n");
                printf("[ %d people joined ]\n", userLen);
                GetChallenge(depositIdx); // 챌린지 불러오기
                printf("--------------------------------------\n");
                MakeChallengeUser(depositIdx, userIdx); // 챌린지 참여
                printf("Successfully Joined.\n\n");
                MakeLog(fp, userIdx, "join challenge success");
                break; }

            case 10 : { // 챌린지 확인
                printf("Selected Check Challenge.\n\n");
                int len = GetChallengeLen(userIdx); // 참여중인 챌린지 개수 출력
                int * idxArr = (int*)malloc(sizeof(int) * len); // 챌린지 받아올 동적배열할당
                GetChallengeIdx(idxArr, len, userIdx); //챌린지 인덱스 모두 불러오기
                for (int i = 0; i < len; ++i) { //모든 챌린지와 챌린지 참여중인 유저 수 print
                    int depositIdx = *(idxArr + i);
                    int userLen = GetChallengeUserLen(depositIdx); 
                    printf("\n--------------------------------------\n");
                    printf("[ %d people joined ]\n", userLen); 
                    GetChallenge(depositIdx); 
                    printf("--------------------------------------\n");
                    MakeLog(fp, userIdx, "check challenge success");
                }
                break; }
            default : // 1~10 외의 다른 숫자 입력시
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
ㄴyear : 년도 받아옴
4, 100으로 나눠지면 윤년, 아니면 평년
윤년이면 1리턴, 평년이면 0리턴
*/

int getDay(int year, int month) { 
    if (month < 1 || month > 12) return 0;
    int day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4) == 0 && (year % 100) != 0 || (year % 400) == 0) day[1] = 29; //윤년이면 2월은 29일까지
    return day[month - 1]; 
} 
/*
ㄴyear : 년도 받아옴
ㄴmonth : 월 받아옴
월 별 날짜수 계산 후 해당 달에 맞는 날짜수 리턴
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
ㄴyear : 년도 받아옴
ㄴmonth : 월 받아옴
월을 시작하는 요일 리턴
*/

void printCalendar(int year, int month) {
    int day = getDay(year, month);
    int start_day = getStartDay(year, month);
    printf("           [ %d - %d ]            \n\n", year, month);
    printf("    S    M    T    W    T    F    S\n\n");
    for(int i = 0; i < start_day; ++i) printf("     ");
    int count = start_day; // 7일이 찰때마다 주를 바꿔줘야 하기 때문에 count를 사용
    for (int j = 1; j <= day; ++j) {
        int check = 0;
        if (check == 0) {
            printf("%5d", j);
            count++;
        }
        if (count == 7) { // 카운트가 7이 되는 순간 다음주로 넘어감
            printf("\n\n");
            count = 0;
        }
    }
}
/*
ㄴyear : 년도 받아옴
ㄴmonth : 월 받아옴

*/

int SignUp(char name[]) { //유저 이름 받음
    sprintf(query, "INSERT INTO User VALUES (0, '%s')", name); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query);
    if (!query_stat) { //유저 존재 안하면 1 return
        return 1;
    } else { //유저가 이미 존재하면 에러화면 뜨면서 0 return
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}
/*
ㄴname : 회원가입 할 유저 이름
이미 유저 존재시 리턴하며 오류 print, 회원가입 성공하면 1 리턴

*/

int CheckLastUserIdx() {
    sprintf(query, "SELECT MAX(userIdx) FROM User"); // 유저의 UserIdx 중 가장 큰 수 쿼리문에 받아오기
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0]; //userIdx 리턴하기 위해 변수에 저장
    return atoi(res);  //useridx 리턴
}
/*
유저의 UserIdx 중 가장 큰 수 쿼리문에 받아와서 출력
불러오기 성공하면 UserIdx 중 가장 큰 값 출력, 아니라면 리턴하며 오류 print
*/

int CheckUser(int userIdx) {
    sprintf(query, "SELECT EXISTS (SELECT 1 FROM User WHERE userIdx = %d) AS cnt", userIdx);  //useridx 테이블에서 사용자가 선택한 useridx불러오는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection); //결과 저장
    char * a;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값이 없을 때까지 반환
        a = sql_row[0];
    }
    return atoi(a); //유저 존재시 useridx 리턴
}
/*
ㄴuserIdx : 존재여부 체크할 유저의 idx
유저 존재 안할시 0 리턴, 존재시 useridx 리턴
*/

int PrintUser() {
    sprintf(query, "SELECT * FROM User"); // User 테이블 안에 있는 모든 값을 가져오는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    
    sql_result = mysql_store_result(connection); //결과 저장
    printf("\n--------------------------------------\n");
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값이 없을 때까지 반환
        printf("%s : %s\n", sql_row[0], sql_row[1]); //유저 인덱스와 유저 print
    }
    printf("--------------------------------------\n\n");
    return 1;
}
/*
유저인덱스와 유저 print
*/

int MakeFriend(int userIdx, int friendIdx) {
    sprintf(query, "INSERT INTO Friend VALUES (%d, %d), (%d, %d)", userIdx, friendIdx, friendIdx, userIdx); //친구 추가 쿼리
    query_stat = mysql_query(connection, query);
    if (!query_stat) { //불러오기 성공시 1리턴
        return 1;
    } else { //불러오기 실패시 오류 출력하며 0 리턴
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
}
/*
ㄴuserIdx :
ㄴfriendIdx : 


*/

int GetFriendLen(int userIdx) {
    sprintf(query, "SELECT COUNT(friendIdx) FROM Friend WHERE userIdx = %d", userIdx); //유저의 친구 수 불러오기
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)  //불러오기 실패시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    char * res; 
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0]; //친구 수 리턴하기 위해 변수에 저장 
    return atoi(res); //친구 수 리턴
}
/*
ㄴuserIdx : user의 친구 수 불러오기 위한 userIdx
유저의 친구 수 불러오기 성공시 친구 수 리턴, 실패시 오류 출력하며 0 리턴
*/

int GetFriend(int * idxArr, int userIdx) {
    sprintf(query, "SELECT f.friendIdx, u.userName FROM Friend f, User u WHERE f.userIdx = %d AND f.friendIdx = u.userIdx", userIdx); //친구 인덱스에 해당하는 이름을 불러옴
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)  //불러오기 실패시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    int i = 0; 
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값이 없을 때까지 반환
        printf("[ %d ] %s ( idx : %s )\n", i+1, sql_row[1], sql_row[0]); //[친구의 계획 인덱스] 친구 (idx : 친구의 유저 인덱스)
        idxArr[i] = atoi(sql_row[0]);
        ++i;
    }
}
/*
ㄴidxArr : 유저의 friendidx 불러올 배열
ㄴuserIdx : user의 친구 불러오기 위한 userIdx
모든 친구 불러오기 성공하면 친구와 친구의 idx print, 실패시 오류 출력하며 0리턴
*/

int IsFriend(int userIdx, int friendIdx) {
    sprintf(query, "SELECT EXISTS (SELECT * FROM Friend WHERE userIdx = %d AND friendIdx = %d)", userIdx, friendIdx);
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //추가 실패시 오류 출력하며 0 리턴
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
userIdx안에 friendIdx 있는지 확인, 확인 실패시 오류 출력하며 0 리턴
확인 성공시 없으면 0리턴 , 있으면 friendIdx 리턴
*/

int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]) { //
    sprintf(query, "INSERT INTO Plan VALUES (0, '%d', '%s', '%s', '%d', now(), '%s')", userIdx, planName, explain, openLevel, endAt); // User을 추가하는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //추가 실패시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    return 1; //추가 성공시 1리턴
}
/*
ㄴuserIdx : 저장할 위치
ㄴplanName : 계획 이름
ㄴexplain : 계획 설명
ㄴopenLevel : 공개 범위
ㄴendAt : 마감 기한
계획 추가 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
*/

int CheckLastPlanIdx() {
    sprintf(query, "SELECT MAX(planIdx) FROM Plan"); //planIdx 중 숫자가 가장 큰 idx(가장 마지막에 추가된 idx) 가져오는 쿼리문
    query_stat = mysql_query(connection, query);
    if (query_stat != 0)  //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
가장 최근에 저장된 planidx 불러옴 성공하면 가장 최근 저장된 planidx 리턴, 아니라면 오류 출력하며 0 리턴
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
userIdx : 유저의 idx
유저의 계획 개수 불러오기 성공하면 계획 개수 리턴, 아니라면 오류 출력하며 0 리턴
*/

int GetPlanIdx(int userIdx, int * idxArr) { 
    sprintf(query, "SELECT planIdx, planName, endAt FROM Plan WHERE userIdx = %d", userIdx); //선택한 useridx 속 planIdx, planName, endAt를 모두 불러오는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0)  //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값 없을 때까지 불러옴
        char *ptr = strtok(sql_row[2], " ");
        printf("[ %d ] : %s (~%s)\n", i + 1, sql_row[1], ptr);  // [인덱스] : 이름 (~마감기한) print
        idxArr[i] = atoi(sql_row[0]); //idxArr에 planIdx 저장
        ++i;
    }
    return 1;
}
/*
userIdx : plan 생성할 유저의 idx
idxArr : 유저가 생성한 planidx를 저장
유저가 설정한 계획인덱스, 계획명, 마감기한 모두 불러와서 print 후 idxArr에 planidx 저장하고 1리턴
불러오기 실패시 0을 리턴하며 오류 print
*/

int GetPlan(char ** arr, int planIdx) {
    sprintf(query, "SELECT * FROM Plan WHERE planIdx = %d", planIdx); //입력한 planidx의 value를 불러오는 쿼리문
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //존재 안할시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값 없을 때까지 불러옴
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
ㄴ**arr : plan에 있는 데이터를 모두 가져오기 위한 이중 포인터 배열
ㄴ**planIdx : 유저가 선택한 planidx
입력한 planidx의 모든 value를 arr로 불러옴 불러오기 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
*/

int GetFriendPlan(int userIdx, int friendIdx, int * idxArr) {
    int access = IsFriend(userIdx, friendIdx);
    if (!access) return 0; //친구인지 아닌지 확인 후 친구 아니면 0 리턴
    /*
    1 : 전체 공개, 2 : 친구 공개, 3 : 나만 공개
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
        printf("[ %d ] %s (~%s)\n", i+1, sql_row[1], sql_row[2]); //[인덱스] 친구의계획명 (~마감기한)
        ++i;
    }
    return 1;
}
/*
ㄴuserIdx : 유저의 idx
ㄴfriendIdx : 계획 확인할 친구의 idx
ㄴidxArr : 친구의 planidx 저장
친구인지, 친구의 계획이 전체공개나 친구공개인지 확인한 뒤, 확인 성공하면 idxArr에 친구의 planidx 저장하고, 친구의 계획과 설명 print 하며 1리턴
아니라면 오류 출력하며 0 리턴
*/

int GetDayPlanLen(int userIdx, char date[]) {
    sprintf(query, "SELECT COUNT(userIdx) FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date); //선택한 날짜에 있는 plan 개수 불러오기
    query_stat = mysql_query(connection, query); 
    if (query_stat != 0) //불러오기 실패시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    char * res;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) res = sql_row[0];
    return atoi(res);
}
/*
ㄴuserIdx : 유저의 idx
ㄴdate : 사용자가 확인할 날짜
사용자가 선택한 날짜의 계획 개수 불러오기 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
*/

int GetDayPlan(int userIdx, char date[], int * idxArr) {
    sprintf(query, "SELECT planIdx, planName, `explain` FROM Plan WHERE userIdx = %d AND DATE(endAt) = '%s'", userIdx, date); // 선택한 날짜에 있는 계획인덱스, 계획명, 계획설명,마감기한 불러오기
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //불러오기 실패시 오류 출력하며 0 리턴
    {
        fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
        return 0;
    }
    sql_result = mysql_store_result(connection); //결과 저장
    int i = 0;
    while ( (sql_row = mysql_fetch_row(sql_result)) != NULL ) { //값 없을 때까지 불러옴
        idxArr[i] = atoi(sql_row[0]); 
        ++i;
    }
    return 1;
}
/*
ㄴuserIdx : 유저의 idx 
ㄴdate : 유저가 선택한 날짜
ㄴidxArr : 선택한 날짜에 있는 계획의 idx를 저장하기 위한 배열
유저가 선택한 날짜에 있는 계획의 idx를 idxArr에 저장 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴuserIdx : 유저의 idx
ㄴplanIdx : 유저가 삭제할 계획의 idx
유저가 선택한 계획 삭제 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴplanIdx : 유저가 수정할 계획의 idx
ㄴplanName : 수정된 이름 
ㄴexplain : 수정된 설명
ㄴendAt : 수정된 기간
유저가 수정할 계획을 받아서 이름, 설명, 기간 수정 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴplanIdx : 세부계획 추가할 큰 계획의 idx
ㄴdatailName : 세부계획명
ㄴstartedAt : 세부계획 시작 날짜
ㄴendAt : 세부계획 마감 날짜
ㄴwhere : 세부계획 실행 위치
세부계획 생성 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
*/

int CheckLastDetailIdx() { 
    sprintf(query, "SELECT MAX(detailIdx) FROM Plandetail"); //DetailIdx 중 숫자가 가장 큰 idx(가장 마지막에 추가된 idx) 가져오는 쿼리문
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //불러오기 실패시 0 리턴하며 오류
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
가장 최근에 저장된 DetailIdx 불러오기 성공하면 가장 최근 저장된 DetailIdx 리턴, 아니라면 오류 출력하며 0 리턴
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
planIdx : 유저가 선택한 계획
유저가 선택한 계획 속 세부계획 개수 불러오기 성공하면 세부계획 수 리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴ**planIdx : 유저가 선택한 planidx
입력한 planidx의 모든 detailplan를 불러옴 불러오기 성공하면 세부계획이름, 시작시간, 마감시간, 장소 print 후 1리턴
아니라면 오류 출력하며 0 리턴
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
ㄴplanIdx : plandetail을 가져오기 위한 planIdx
유저가 선택한 plan속 모든 plandetail삭제 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴdetailIdx : 유저가 삭제하기로 선택한 detailIdx
선택한 세부계획 삭제 성공하면 삭제 성공 print , 아니라면 오류 출력하며 리턴
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
ㄴdetailIdx : 유저가 수정할 세부계획의 Idx
ㄴdetailName : 세부계획명
ㄴstartedAt : 세부계획 시작 날짜
ㄴendAt : 세부계획 마감 날짜
ㄴwhere : 세부계획 실행 위치
유저가 수정할 세부계획을 받아서 이름, 시작날짜, 마감날짜, 위치 수정 성공하면 수정 성공 print, 아니라면 오류 출력하며 리턴
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
ㄴplanIdx : 리뷰 추가할 plan의 idx
ㄴuserIdx : 리뷰 추가한 user의 idx
ㄴcontent : 내용
ㄴscore : 별점
친구 계획에 리뷰 생성 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴplanIdx : 유저가 선택한 리뷰 가져올 계획의 idx
리뷰 개수 가져오기 성공하면 리뷰 개수 리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴplanidx : 유저가 선택한 리뷰 가져올 계획의 idx
리뷰에 있는 userIdx, content, score, createdAt 모두 가져오기 성공하면 print 하며 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴplanreviewIdx : 유저가 삭제하기로 선택한 리뷰의 idx
선택한 리뷰 삭제 성공하면 삭제 성공 print, 아니라면 오류 출력하며 리턴
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
ㄴplanreviewIdx : 유저가 수정할 리뷰의 Idx
ㄴcontent : 리뷰 내용
ㄴscore : 별점
유저가 수정할 리뷰를 받아서 리뷰내용, 별점 수정 성공하면 수정 성공 print, 아니라면 오류 출력하며 리턴
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
ㄴdepositName : 챌린지 명
ㄴmoney : 모금액
ㄴendAt : 마감기한
챌린지 생성 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴ depositIdx : 챌린지를 불러올 idx
ㄴ userIdx : 챌린지에 참여할 유저의 idx
> 챌린지에 유저 참여 성공하면 1리턴, 아니라면 오류 출력하며 0 리턴
*/

int CheckLastDepositIdx() { 
    sprintf(query, "SELECT MAX(depositIdx) FROM Deposit"); //DepositIdx 중 숫자가 가장 큰 idx(가장 마지막에 추가된 idx) 가져오는 쿼리문
    query_stat = mysql_query(connection, query);
    if (query_stat != 0) //불러오기 실패시 0 리턴하며 오류
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
가장 최근에 저장된 챌린지의 idx불러옴 챌린지 idx리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴdepositIdx : 참여 여부 확인할 챌린지 idx
ㄴuserIdx : 참여 여부 확인할 유저 idx
챌린지 참여 여부 확인 성공하면 유저 idx 리턴, 아니라면 오류 출력하며 0 리턴
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
ㄴuserIdx : user의 친구 수 불러오기 위한 userIdx
챌린지 수 불러오기 성공시 친구 수 리턴, 실패시 오류 출력하며 0 리턴
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
ㄴ depositIdx : 챌린지에 참여한 user 수 불러오기 위한 idx
> 유저 수 불러오기 성공하면 유저 수 리턴, 아니라면 0을 리턴하며 오류 print 
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
idxArr : 유저가 참여중인 challengeIdx를 저장
len : 참여중인 챌린지 개수
userIdx : 유저의 idx
유저가 참여중인 challengeidx 모두 불러와서 idxArr에 저장하고 1리턴, 불러오기 실패시 0을 리턴하며 오류 print
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
ㄴ depositIdx : 챌린지를 불러올 idx
> 불러오기 성공하면 1 리턴, 아니라면 0을 리턴하며 오류 print 
*/