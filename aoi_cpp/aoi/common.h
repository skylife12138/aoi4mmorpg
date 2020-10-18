#ifndef _COMMON_H
#define _COMMON_H

#include <algorithm>
#include "define.h"


class LinkList;
class Auctor_Base;
class Map_Mgr_Base;


int  RandInt(int a, int b);
void RandMove(int& r_x, int& r_y);
int  Calc_Map_Uuid_By_Center(int center_x, int center_y);
int  Calc_Map_Uuid_By_Coordinate(int x, int y);

extern int New_Count;


class Map_Base
{
public:
	Map_Base(int x, int y) :m_center_x(x), m_center_y(y) {
		m_uuid = Calc_Map_Uuid_By_Center(m_center_x, m_center_y);
		memset(m_neighbor, 0, sizeof(m_neighbor));
		Calc_Neighbor_Map_Uuid();
	}
	virtual ~Map_Base();
	void Add_Act(Auctor_Base* Act);
	void Del_Act(Auctor_Base* Act);
private:
	void Calc_Neighbor_Map_Uuid();
public:
	int m_center_x;
	int m_center_y;
	int m_uuid;
	int m_neighbor[Max_Neighbor_Size];

	// 记录map区域内的真实对象
	map<int, Auctor_Base*> Act_in_Area;
};

class Map_Mgr_Base
{
public:
	Map_Mgr_Base() :Send_Move_Msg_Count(0), Move_Count(0), Enter_Count(0), Leave_Count(0) {}
	virtual ~Map_Mgr_Base();
	void Add_Send_Move_Msg_Count() { Send_Move_Msg_Count++; }
	int Get_Send_Move_Msg_Count() { return Send_Move_Msg_Count; }
	void Add_Move_Count() { Move_Count++; }
	int Get_Move_Count() { return Move_Count; }
	void Add_Enter_Count() { Enter_Count++; }
	int Get_Enter_Count() { return Enter_Count; }
	void Add_Leave_Count() { Leave_Count++; }
	int Get_Leave_Count() { return Leave_Count; }
	const int GetAuctorSize() { return Auctor_Set.size(); }
public:
	virtual void Move(int TickTime) = 0;
	virtual void AddActor(Auctor_Base* act) = 0;
	virtual void Show() = 0;
	int Send_Move_Msg_Count;
	int Move_Count;
	int Enter_Count;
	int Leave_Count;
	vector<Auctor_Base*> Auctor_Set;
};


class Auctor_Base
{
public:
	Auctor_Base(int uid, int x, int y) :m_uid(uid), m_x(x), m_y(y), m_tick_time(0) {
		memset(m_EdgeArr, 0, sizeof(m_EdgeArr));
		SetEdgeArr(0);
	}
	virtual ~Auctor_Base();
	int m_uid;
	int m_x;
	int m_y;
	int m_tick_time;

	int m_EdgeArr[4];

public:
	virtual bool Move(int TickTime);
	virtual void Leave() = 0;
	void GetMoveOffset(int& Offset_x, int& Offset_y, int TickTime);
	void SetEdgeArr(int TickTime);

	int GetValueByName(const char* Name);
};

struct Node
{
	Node(Auctor_Base* value) :m_Act(value), prev(NULL), next(NULL) {}
	~Node() { m_Act = NULL; prev = NULL; next = NULL; }
	Auctor_Base* m_Act;
	Node* prev;
	Node* next;
};

class LinkList 
{
public:
	LinkList() :m_Head(NULL), m_Tail(NULL), m_Size(0){}
	~LinkList();

	bool IsEmpty() {return m_Head == NULL;}
	int GetLen() { return m_Size; }

	Node* Add_in_sort(Auctor_Base* Act, const char* Name);
	void Remove(Auctor_Base* Act);
	void MoveNode(Node* node, const char* Name);
	bool CheckAnnular();
private:
	Node* m_Head;
	Node* m_Tail;

	int m_Size;
};

#endif