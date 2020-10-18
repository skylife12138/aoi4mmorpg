#include <iterator>
#include "link.h"

extern Map_Mgr_Base* Map_Mgr;

Auctor_Link::Auctor_Link(int uid, int x, int y):Auctor_Base(uid, x, y)
{
	x_Node = NULL;
	y_Node = NULL;
}

Auctor_Link::~Auctor_Link()
{
	x_Node = NULL;
	y_Node = NULL;

	View_Set.clear();
}

bool Auctor_Link::Move(int TickTime)
{
	if (!Map_Mgr || !Auctor_Base::Move(TickTime))
		return false;
	dynamic_cast<Map_Mgr_Link*>(Map_Mgr)->MoveActor(this);
	set<Auctor_Base*> t_set, t_view_set;
	set<Auctor_Base*> r_set = Get_Link_View_Data();

	set_difference(View_Set.begin(), View_Set.end(), r_set.begin(), r_set.end(), inserter(t_set, t_set.begin()));
	LeaveView(t_set);
	t_set.clear();

	set_intersection(View_Set.begin(), View_Set.end(), r_set.begin(), r_set.end(), inserter(t_view_set, t_view_set.begin()));
	MoveView(t_view_set);

	set_difference(r_set.begin(), r_set.end(), View_Set.begin(), View_Set.end(), inserter(t_set, t_set.begin()));
	SetViewSet(t_view_set);
	EnterView(t_set);
	
	return true;
}

bool Auctor_Link::FindActInView(Auctor_Base* act)
{
	return View_Set.find(act) != View_Set.end();
}

void Auctor_Link::AddLinkView(Auctor_Base* act)
{
	View_Set.insert(act); 
}

void Auctor_Link::DelLinkView(Auctor_Base* act)
{
	auto iter = View_Set.find(act);
	if (iter != View_Set.end())
		View_Set.erase(iter);
}

set<Auctor_Base*> Auctor_Link::Get_Link_View_Data()
{
	set<Auctor_Base*> x_set, y_set, r_set;
	if (!x_Node || !y_Node || !Map_Mgr)
		return r_set;
	Node* t_node = x_Node->prev;
	while (t_node) {
		if (t_node->m_Act && m_x - t_node->m_Act->m_x > Max_Link_View_Size)
			break;
		x_set.insert(t_node->m_Act);
		t_node = t_node->prev;

		//// 过载保护
		//if (x_set.size() >= Max_View_Num * 2)
		//	break;
	}

	t_node = x_Node->next;
	while (t_node) {
		if (t_node->m_Act && t_node->m_Act->m_x - m_x > Max_Link_View_Size)
			break;
		x_set.insert(t_node->m_Act);
		t_node = t_node->next;

		//// 过载保护
		//if (x_set.size() >= Max_View_Num * 2)
		//	break;
	}

	if (x_set.empty())
		return r_set;
	// 优化项，如果x轴在视野内的数量少于y轴总数量的1/3，则不用遍历y轴，直接查看x轴视野内的是否符合y轴视野条件
	if (int(x_set.size()) < dynamic_cast<Map_Mgr_Link*>(Map_Mgr)->GetLinkListSize_y() / 3) {
		for (auto iter = x_set.begin(); iter != x_set.end(); ++iter) {
			if (abs((*iter)->m_y - m_y) > Max_Link_View_Size) 
				continue;

			r_set.insert(*iter);
		}
		return r_set;
	}

	t_node = y_Node->prev;
	while (t_node) {
		if (t_node->m_Act && m_y - t_node->m_Act->m_y > Max_Link_View_Size)
			break;
		y_set.insert(t_node->m_Act);
		t_node = t_node->prev;

		//// 过载保护
		//if (y_set.size() >= Max_View_Num * 2)
		//	break;
	}

	t_node = y_Node->next;
	while (t_node) {
		if (t_node->m_Act && t_node->m_Act->m_y - m_y > Max_Link_View_Size)
			break;
		y_set.insert(t_node->m_Act);
		t_node = t_node->next;

		//// 过载保护
		//if (y_set.size() >= Max_View_Num * 2)
		//	break;
	}
	
	set_intersection(x_set.begin(), x_set.end(), y_set.begin(), y_set.end(), inserter(r_set, r_set.begin()));
	return r_set;
}

void Auctor_Link::EnterView(set<Auctor_Base*> viewset)
{
	for (auto iter = viewset.begin(); iter != viewset.end(); ++iter)
	{
		if (IsViewFull())
			return;

		Auctor_Link* t_Act = dynamic_cast<Auctor_Link*>(*iter);
		if (!t_Act)
			continue;
		if (FindActInView(t_Act) || t_Act->IsViewFull() || t_Act->FindActInView(this))
			continue;
		AddLinkView(t_Act);
		t_Act->AddLinkView(this);

		SendLinkMsg("enter", t_Act);
		Map_Mgr->Add_Enter_Count();
	}
}

void Auctor_Link::LeaveView(set<Auctor_Base*> viewset)
{
	for (auto iter = viewset.begin(); iter != viewset.end(); ++iter)
	{
		Auctor_Link* t_Act = dynamic_cast<Auctor_Link*>(*iter);
		if (!t_Act)
			continue;
		DelLinkView(t_Act);
		t_Act->DelLinkView(this);

		SendLinkMsg("leave", t_Act);
		Map_Mgr->Add_Leave_Count();
	}
}

void Auctor_Link::MoveView(set<Auctor_Base*> viewset)
{
	for (auto iter = viewset.begin(); iter != viewset.end(); ++iter)
	{
		Auctor_Link* t_Act = dynamic_cast<Auctor_Link*>(*iter);
		if (!t_Act)
			continue;
		SendLinkMsg("move", t_Act);
		Map_Mgr->Add_Move_Count();
	}
}

void Auctor_Link::SendLinkMsg(const char* logmsg, Auctor_Base* Act)
{
	if (!Act)
		return;
	cout << m_uid << " " << logmsg << " link to other" << Act->m_uid << endl;
	Map_Mgr->Add_Send_Move_Msg_Count();

	if (strcmp(logmsg, "move") == 0)
		return;
	cout << m_uid << " " << logmsg << " link to act" << Act->m_uid << endl;
	Map_Mgr->Add_Send_Move_Msg_Count();
}

Map_Mgr_Link::~Map_Mgr_Link()
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Link* Act = dynamic_cast<Auctor_Link*>(*iter);
		if (!Act)
			continue;
		delete Act;
		New_Count--;
	}
}

void Map_Mgr_Link::Move(int TickTime)
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Link* Act = dynamic_cast<Auctor_Link*>(*iter);
		if (!Act)
			continue;

		if (!Act->Move(TickTime))
			continue;
	}
}

void Map_Mgr_Link::AddActor(Auctor_Base* act)
{
	Auctor_Link* Act = dynamic_cast<Auctor_Link*>(act);
	if (!Act)
		return;
	Auctor_Set.push_back(act);
	
	Act->x_Node = x_LinkList.Add_in_sort(act, "x");
	Act->y_Node = y_LinkList.Add_in_sort(act, "y");

	set<Auctor_Base*> r_set = Act->Get_Link_View_Data();
	Act->EnterView(r_set);
}

void Map_Mgr_Link::MoveActor(Auctor_Link* act)
{
	x_LinkList.MoveNode(act->x_Node, "x");
	y_LinkList.MoveNode(act->y_Node, "y");
}