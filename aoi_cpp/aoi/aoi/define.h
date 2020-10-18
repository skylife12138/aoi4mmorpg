#ifndef _DEFINE_H
#define _DEFINE_H

#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <cstring>
#include <vector>
using namespace std;


// 地图边长 不能大于2^16
const int Max_Grid_Size = 9;

// 地图周围邻居(包含自己)
const int Max_Neighbor_Size = 9;

// 测试移动次数
const int Max_Move_Count = 10;

// 视野上限数量
const int Max_View_Num = 3000;

// 十字链表视野范围半径
const int Max_Link_View_Size = 4;


#define CHECK_NULL(obj, error_log) \
	do{\
		if (obj == NULL){ \
			cout << error_log << endl;\
			return false;\
		}\
	}while(0)
	



#endif