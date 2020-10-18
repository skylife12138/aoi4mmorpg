#ifndef _DEFINE_H
#define _DEFINE_H

#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <cstring>
#include <vector>
using namespace std;


// ��ͼ�߳� ���ܴ���2^16
const int Max_Grid_Size = 9;

// ��ͼ��Χ�ھ�(�����Լ�)
const int Max_Neighbor_Size = 9;

// �����ƶ�����
const int Max_Move_Count = 10;

// ��Ұ��������
const int Max_View_Num = 3000;

// ʮ��������Ұ��Χ�뾶
const int Max_Link_View_Size = 4;


#define CHECK_NULL(obj, error_log) \
	do{\
		if (obj == NULL){ \
			cout << error_log << endl;\
			return false;\
		}\
	}while(0)
	



#endif