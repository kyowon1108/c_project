#include <stdio.h>
#include <time.h>

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
    printf("           [ %d년 %d월 ]            \n\n", year, month);
    printf("   일   월   화   수   목   금   토\n\n");
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

// [ 달력 출력 관련 함수]
int IsLeafYear(int year); // 윤년인지 체크
int getDay(int year, int month); // 월별 날짜 수가 몇인지 리턴
int getStartDay(int year, int month); // 달력에서 1일인 요일 리턴
void printCalendar(int year, int month); // 달력 출력

// [ 유저 관련 함수 ]
int SignUp(char name[]); // 회원가입
int printUser(); // 유저 리스트 출력
int MakeFriend(int userIdx, int friendIdx); // 친구 추가
int IsFriend(int userIdx, int friendIdx); // 친구인지 확인

// [ 계획 관련 함수 ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // 계획 생성
int GetPlanLen(int userIdx); // 유저가 생성한 계획의 수 리턴
int GetPlan(int userIdx); // 유저가 생성한 계획 리스트 출력
int GetFriendPlan(int userIdx, int friendIdx); // 친구가 생성한 계획 리스트 출력
void GetDayPlan(int * arr, int userIdx, char date[]); // 특정 날의 계획 리스트 출력 및 인덱스 리턴
void DeletePlan(int userIdx, int planIdx); // 계획 삭제
void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx); // 계획 수정

// [ 세부 계획 관련 함수 ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 생성
int GetPlanDetailLen(int planIdx); // 유저가 생성한 계획 디테일의 수 리턴
int GetPlanDetail(int planIdx); // 유저가 생성한 계획 디테일 리스트 출력
void DeletePlandetail(int detailIdx); // 계획 디테일 삭제
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // 계획 디테일 수정

// [ 계획 리뷰 관련 함수 ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score);
int GetPlanReview(int planreviewIdx);
void DeletePlanReview(int planreviewIdx);
void ModifyPlanReview(int planreviewIdx, char content[], int score)

// [ 세부 계획 리뷰 관련 함수]
int MakeDetailReview(int detailIdx, int userIdx, char content[], int score);
int GetDetailReview(int detailreviewIdx);
void DeleteDetailReview(int detailreviewIdx);
void ModifyDetailReview(int detailreviewIdx, char content[], int score);


int main() {
    struct tm* t;
    time_t base = time(NULL);
    t = localtime(&base);

    printCalendar(t->tm_year + 1900, t->tm_mon + 1);

    printf("\n\n");
    int isRoof = 1;
    int function;
    while(isRoof) {
        printf("| 1 : 계획 추가 | 2 : 계획 삭제 | 3 : 계획 수정 | 4 : 계획 확인 |\n| 5 : 평가 확인 | 6 : 친구 계획 확인 | 7 : 친구 추가 |\n 실행할 번호 : ");
        scanf("%d", &function);
        if (function > 7 || function < 0) {
            printf("올바른 번호를 입력해주세요.\n\n");
            continue;
        }
        printf("%d번을 선택했습니다.", function);
        /*
        추가로 코드를 작성해야할 곳
        */
        break;
    }
    return 0;

}