/*
ÿ���� �̸�
ÿ���� ���� ���̵�
��ݾ�
������ ���
�Ⱓ(���۽ð�, �����ð�)
*/
#ifndef __CHALLENGE__
#define __CHALLENGE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "struct.h"
#include "setting.h"
#include "function.h"



void createChallenge(char name[], unsigned int money, person * joined, time_t startAt, time_t endAt) {
    if (strlen(name) > 30) {
        printf("name length is too long!");
        return;
    } // ���� ÿ���� �̸��� 30���ڰ� �Ѵ´ٸ� ������
    challenge asdf;
    asdf.idx = 1; // �ӽ÷� ��������� ���Ŀ� �Լ��� �߰��ؼ� idx�� �����ϰų� ���ο� idx�� �����Ұ���
    asdf.name = name;
    asdf.money = money;
    asdf.startAt = startAt;
    asdf.endAt = endAt;
    
}

#endif