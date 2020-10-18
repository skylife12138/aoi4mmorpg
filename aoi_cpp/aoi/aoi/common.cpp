#include "common.h"
#include <assert.h>

int New_Count = 0;
Map_Mgr_Base* Map_Mgr = NULL;


int RandInt(int a, int b)
{
	return (rand() % (b - a + 1)) + a;
}

void RandMove(int& r_x, int& r_y)
{
	r_x = RandInt(0, 1) == 0 ? -1 : 1;
	r_y = RandInt(0, 1) == 0 ? -1 : 1;
}

int Calc_Map_Uuid_By_Center(int center_x, int center_y)
{
	return int((center_x << 16) | center_y);
}

int Calc_Map_Uuid_By_Coordinate(int x, int y)
{
	int c_x = (3 * (x / 3)) + 1;
	int c_y = (3 * (y / 3)) + 1;

	return Calc_Map_Uuid_By_Center(c_x, c_y);
}

Map_Base::~Map_Base()
{
	m_center_x = 0;
	m_center_y = 0;
	m_uuid = 0;
	memset(m_neighbor, 0, sizeof(m_neighbor));
	Act_in_Area.clear();
}

void Map_Base::Add_Act(Auctor_Base* Act)
{
	if (!Act)
		return;
	Act_in_Area.insert(pair<int, Auctor_Base*>(Act->m_uid, Act));
}

void Map_Base::Del_Act(Auctor_Base* Act)
{
	if (!Act)
		return;
	auto iter = Act_in_Area.find(Act->m_uid);
	if (iter != Act_in_Area.end())
		Act_in_Area.erase(iter);
	else {
		cout << "Error del act" << endl;
	}
}

void Map_Base::Calc_Neighbor_Map_Uuid()
{
	int count_x = 0;
	int t_x = m_center_x - 3;
	
	while (count_x < 3)
	{
		if (t_x < 0 || t_x >= Max_Grid_Size) {
			t_x += 3;
			count_x++;
			continue;
		}
			
		int count_y = 0;
		int t_y = m_center_y - 3;
		while (count_y < 3)
		{
			if (t_y < 0 || t_y >= Max_Grid_Size) {
				t_y += 3;
				count_y++;
				continue;
			}

			int count_nei = count_x * 3 + count_y;
			if (count_nei >= Max_Neighbor_Size) {
				cout << "calc neighbor error !!!" << count_nei << endl;
				return;
			}
			m_neighbor[count_nei] = Calc_Map_Uuid_By_Center(t_x, t_y);

			t_y += 3;
			count_y++;
		}

		t_x += 3;
		count_x++;
	}
}

Auctor_Base::~Auctor_Base()
{
	m_uid = 0;
	m_x = 0;
	m_y = 0;
	m_tick_time = 0;
	memset(m_EdgeArr, 0, sizeof(m_EdgeArr));
}

void Auctor_Base::GetMoveOffset(int& Offset_x, int& Offset_y, int TickTime)
{
	RandMove(Offset_x, Offset_y);
	if (m_EdgeArr[0] && m_EdgeArr[0] + RandInt(3, 5) >= TickTime)
		Offset_x = 1;
	else if (m_EdgeArr[1] && m_EdgeArr[1] + RandInt(3, 5) >= TickTime)
		Offset_x = -1;

	if (m_EdgeArr[2] && m_EdgeArr[2] + RandInt(3, 5) >= TickTime)
		Offset_y = 1;
	else if (m_EdgeArr[3] && m_EdgeArr[3] + RandInt(3, 5) >= TickTime)
		Offset_y = -1;

	
	if (Offset_x == 0 && Offset_y == 0) {
		if (m_x > (Max_Grid_Size * 2 / 3))
			Offset_x = -1;
		else
			Offset_x = 1;

		if (m_y > (Max_Grid_Size * 2 / 3))
			Offset_y = -1;
		else
			Offset_y = 1;
	}
}

void Auctor_Base::SetEdgeArr(int TickTime)
{
	if (m_x == 0)
		m_EdgeArr[0] = TickTime;
	else if (m_x == Max_Grid_Size - 1)
		m_EdgeArr[1] = TickTime;

	if (m_y == 0)
		m_EdgeArr[2] = TickTime;
	else if (m_y == Max_Grid_Size - 1)
		m_EdgeArr[3] = TickTime;
}

bool Auctor_Base::Move( int TickTime)
{
	if (m_tick_time && (m_tick_time == TickTime))
		return false;
	int Offset_x = 0, Offset_y = 0;
	GetMoveOffset(Offset_x, Offset_y, TickTime);
	//cout << "uid = " << m_uid << " Offset_x = " << Offset_x << " Offset_y = " << Offset_y << endl;
	//测试
	/*if (m_uid != 10000)
		return false;
	Offset_x = 1;*/
	//测试
		
	m_x = min<int>(Max_Grid_Size - 1, max<int>(0, m_x + Offset_x));
	m_y = min<int>(Max_Grid_Size - 1, max<int>(0, m_y + Offset_y));

	SetEdgeArr(TickTime);

	return true;
}

int Auctor_Base::GetValueByName(const char* Name)
{
	if (strcmp(Name, "x") == 0) {
		return m_x;
	}
	else if (strcmp(Name, "y") == 0) {
		return m_y;
	}
	else
	{
		return -1;
	}
}

Map_Mgr_Base::~Map_Mgr_Base()
{
	Send_Move_Msg_Count = 0;
	Auctor_Set.clear();
}

LinkList::~LinkList()
{
	while (m_Head) {
		Node* t_Head = m_Head;
		m_Head = m_Head->next;
		delete t_Head;
		New_Count--;
	}
	m_Head = NULL;
	m_Tail = NULL;
}

Node* LinkList::Add_in_sort(Auctor_Base* Act, const char* Name)
{
	if (!Act)
		return NULL;
	Node* NewNode = new Node(Act);
	if (!NewNode)
		return NULL;
	New_Count += sizeof(Node);
	m_Size++;
	if (IsEmpty()) {
		m_Head = NewNode;
		m_Tail = NewNode;
		return NewNode;
	}
	Node* t_Head = m_Head;
	while (t_Head) {
		if (t_Head->m_Act->GetValueByName(Name) > Act->GetValueByName(Name)) {
			if (t_Head->prev == NULL) {
				NewNode->next = t_Head;
				t_Head->prev = NewNode;
				m_Head = NewNode;
			}
			else {
				t_Head->prev->next = NewNode;
				NewNode->prev = t_Head->prev;
				NewNode->next = t_Head;
				t_Head->prev = NewNode;
			}
			return NewNode;
		}
		t_Head = t_Head->next;
	}

	if (t_Head == NULL) {
		m_Tail->next = NewNode;
		NewNode->prev = m_Tail;
		m_Tail = NewNode;
	}
	return NewNode;
}

void LinkList::Remove(Auctor_Base* Act)
{
	if (!Act)
		return;
	if (IsEmpty())
		return;
	Node* t_Head = m_Head;
	while (t_Head) {
		if (t_Head->m_Act == Act) {
			if (t_Head == m_Head) {
				m_Head = m_Head->next;
				//处理只有一个节点的特殊情况
				if (m_Head) {
					m_Head->prev = NULL;
				}
			}
			else {
				t_Head->prev->next = t_Head->next;
				//处理删除节点是最后一个的情况
				if (t_Head->next) {
					t_Head->next->prev = t_Head->prev;
				}
			}
			m_Size--;
			delete t_Head;
			New_Count--;
			return;
		}
		else {
			t_Head = t_Head->next;
		}
	}
}

void LinkList::MoveNode(Node* node, const char* Name)
{
	if (!node || !node->m_Act || IsEmpty())
		return;
	Node* t_Node = node->prev;
	if (t_Node && t_Node->m_Act && (t_Node->m_Act->GetValueByName(Name) > node->m_Act->GetValueByName(Name))) {
		while (t_Node)
		{
			if (t_Node->m_Act->GetValueByName(Name) <= node->m_Act->GetValueByName(Name))
			{
				if (node->next) {
					node->next->prev = node->prev;
				}
				else
				{
					m_Tail = node->prev;
				}
				node->prev->next = node->next;

				node->prev = t_Node;
				node->next = t_Node->next;
				assert(t_Node->next);
				t_Node->next = node;
				node->next->prev = node;

				break;
			}
			t_Node = t_Node->prev;
		}

		if (!t_Node) {
			if (node->next) {
				node->next->prev = node->prev;
			}
			node->prev->next = node->next;

			node->prev = NULL;
			node->next = m_Head;
			m_Head->prev = node;
			m_Head = node;
		}
	}

	t_Node = node->next;
	if (t_Node && t_Node->m_Act && (t_Node->m_Act->GetValueByName(Name) < node->m_Act->GetValueByName(Name))) {
		while (t_Node)
		{
			if (t_Node->m_Act->GetValueByName(Name) >= node->m_Act->GetValueByName(Name)) {
				if (node->prev) {
					node->prev->next = node->next;
				}
				else
				{
					m_Head = node->next;
				}
				node->next->prev = node->prev;

				node->prev = t_Node->prev;
				node->next = t_Node;
				assert(t_Node->prev);
				t_Node->prev = node;
				node->prev->next = node;

				break;
			}
			t_Node = t_Node->next;
		}

		if (!t_Node) {
			if (node->prev) {
				node->prev->next = node->next;
			}
			node->next->prev = node->prev;

			node->next = NULL;
			node->prev = m_Tail;
			m_Tail->next = node;
			m_Tail = node;
		}
	}

	if (CheckAnnular())
	{
		cout << "move CheckAnnular" << " " << Name << endl;
	}
}

bool LinkList::CheckAnnular()
{
	if (!m_Head || !m_Tail)
		return false;
	Node* t1_Node = m_Head;
	Node* t2_Node = m_Head;
	while (t1_Node && t2_Node) {
		if (t1_Node->next == NULL)
			break;
		t1_Node = t1_Node->next->next;
		t2_Node = t2_Node->next;
		if (t1_Node == t2_Node)
			return true;
	}

	t1_Node = m_Tail;
	t2_Node = m_Tail;
	while (t1_Node && t2_Node) {
		if (t1_Node->prev == NULL)
			break;
		t1_Node = t1_Node->prev->prev;
		t2_Node = t2_Node->prev;
		if (t1_Node == t2_Node)
			return true;
	}
	return false;
}