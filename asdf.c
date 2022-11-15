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

// [ �޷� ��� ���� �Լ�]
int IsLeafYear(int year); // �������� üũ
int getDay(int year, int month); // ���� ��¥ ���� ������ ����
int getStartDay(int year, int month); // �޷¿��� 1���� ���� ����
void printCalendar(int year, int month); // �޷� ���

// [ ���� ���� �Լ� ]
int SignUp(char name[]); // ȸ������
int printUser(); // ���� ����Ʈ ���
int MakeFriend(int userIdx, int friendIdx); // ģ�� �߰�
int IsFriend(int userIdx, int friendIdx); // ģ������ Ȯ��

// [ ��ȹ ���� �Լ� ]
int MakePlan(int userIdx, char planName[], char explain[], int openLevel, char endAt[]); // ��ȹ ����
int GetPlanLen(int userIdx); // ������ ������ ��ȹ�� �� ����
int GetPlan(int userIdx); // ������ ������ ��ȹ ����Ʈ ���
int GetFriendPlan(int userIdx, int friendIdx); // ģ���� ������ ��ȹ ����Ʈ ���
void GetDayPlan(int * arr, int userIdx, char date[]); // Ư�� ���� ��ȹ ����Ʈ ��� �� �ε��� ����
void DeletePlan(int userIdx, int planIdx); // ��ȹ ����
void ModifyPlan(char planName[], char explain[], char endAt[], int planIdx); // ��ȹ ����

// [ ���� ��ȹ ���� �Լ� ]
int MakePlanDetail(int planIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����
int GetPlanDetailLen(int planIdx); // ������ ������ ��ȹ �������� �� ����
int GetPlanDetail(int planIdx); // ������ ������ ��ȹ ������ ����Ʈ ���
void DeletePlandetail(int detailIdx); // ��ȹ ������ ����
void ModifyPlanDetail(int detailIdx, char detailName[], char startedAt[], char endAt[], char where[]); // ��ȹ ������ ����

// [ ��ȹ ���� ���� �Լ� ]
int MakePlanReview(int planIdx, int userIdx, char content[], int score);
int GetPlanReview(int planreviewIdx);
void DeletePlanReview(int planreviewIdx);
void ModifyPlanReview(int planreviewIdx, char content[], int score)

// [ ���� ��ȹ ���� ���� �Լ�]
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
        printf("| 1 : ��ȹ �߰� | 2 : ��ȹ ���� | 3 : ��ȹ ���� | 4 : ��ȹ Ȯ�� |\n| 5 : �� Ȯ�� | 6 : ģ�� ��ȹ Ȯ�� | 7 : ģ�� �߰� |\n ������ ��ȣ : ");
        scanf("%d", &function);
        if (function > 7 || function < 0) {
            printf("�ùٸ� ��ȣ�� �Է����ּ���.\n\n");
            continue;
        }
        printf("%d���� �����߽��ϴ�.", function);
        /*
        �߰��� �ڵ带 �ۼ��ؾ��� ��
        */
        break;
    }
    return 0;

}