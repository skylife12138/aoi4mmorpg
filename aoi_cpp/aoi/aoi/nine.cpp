#include "nine.h"

extern int New_Count;
extern Map_Mgr_Base* Map_Mgr;

bool ViewComponent::IsFull()
{
	if (View_List.size() >= Max_View_Num)
		return true;
	return false;
}

bool ViewComponent::Is_In_View(Auctor_Nine* v_Act)
{
	for (auto iter = View_List.begin(); iter != View_List.end(); ++iter)
	{
		if (iter->View_Act == v_Act)
			return true;
	}
	return false;
}

bool ViewComponent::Add_View(Auctor_Nine* v_Act)
{
	if (!v_Act || View_Owner_Act == v_Act)
		return false;
	if (IsFull() || !v_Act->ViewComp || v_Act->ViewComp->IsFull())
		return false;
	if (Is_In_View(v_Act) || v_Act->ViewComp->Is_In_View(v_Act))
		return false;
	v_Act->ViewComp->View_List.push_back(View_Data(View_Owner_Act, View_List.size()));
	View_List.push_back(View_Data(v_Act, v_Act->ViewComp->View_List.size() - 1));
	return true;
}

bool ViewComponent::Del_View(Auctor_Nine* v_Act)
{
	if (!v_Act || !v_Act->ViewComp)
		return false;
	int v_index = -1;
	for (auto iter = View_List.begin(); iter != View_List.end(); ++iter)
	{
		if (iter->View_Act == v_Act) {
			v_index = iter->Index;
			break;
		}
	}
	if (v_index == -1)
		return false;
	if (v_index < 0 || v_index >= int(v_Act->ViewComp->View_List.size()) || v_Act->ViewComp->View_List[v_index].View_Act != View_Owner_Act) {
		cout << "del view error !!!" << endl;
		return false;
	}
	
	return Del_View_By_Index(v_Act->ViewComp->View_List[v_index].Index) && v_Act->ViewComp->Del_View_By_Index(v_index);
}

bool ViewComponent::Del_View_By_Index(int v_Index)
{
	if (View_List.empty() || v_Index < 0 || v_Index >= int(View_List.size()))
		return false;
	if (int(View_List.size()) - 1 == v_Index) {
		View_List.pop_back();
	}
	else {
		View_List[v_Index] = View_List.back();
		View_List.pop_back();

		Auctor_Nine* v_Act = View_List[v_Index].View_Act;
		int Idx = View_List[v_Index].Index;
		if (!v_Act || Idx < 0 || Idx >=  int(v_Act->ViewComp->View_List.size())) {
			cout << "View Act Error!!!" << endl;
			return false;
		}
		
		v_Act->ViewComp->View_List[Idx].Index = v_Index;
	}
	return true;

}

Auctor_Nine::Auctor_Nine(int uid, int x, int y) :Auctor_Base(uid, x, y)
{
	ViewComp = new ViewComponent(this);
	New_Count++;
	if (!ViewComp) {
		cout << "Create View Error!!!" << endl;
		return;
	}
		
	Now_Map = NULL;
}

Auctor_Nine::~Auctor_Nine() 
{
	if (ViewComp) {
		delete ViewComp;
		New_Count--;
	}
	ViewComp = NULL;
	Now_Map = NULL;
}

void Auctor_Nine::SendMoveMsg()
{
	for (auto iter = ViewComp->View_List.begin(); iter != ViewComp->View_List.end(); ++iter)
	{
		Auctor_Nine* v_act = iter->View_Act;
		if (!v_act)
			continue;
		cout << m_uid << " send move msg to view " << v_act->m_uid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Move_Count();
	}
}

bool Auctor_Nine::Move(int TickTime)
{
	if (!Auctor_Base::Move(TickTime))
		return false;
	int Now_Map_Uid = Calc_Map_Uuid_By_Coordinate(m_x, m_y);
	if (!Now_Map) {
		cout << "Act Now map error!!!" << endl;
		return false;
	}
	if (Now_Map->m_uuid == Now_Map_Uid) {
		SendMoveMsg();
	}
	else
	{
		Map_Nine* t_Now_Map = dynamic_cast<Map_Mgr_Nine*>(Map_Mgr)->GetMapLightByUid(Now_Map_Uid);
		if (!t_Now_Map)
			return false;
		if (t_Now_Map == Now_Map) {
			cout << "Now Map Error 2!!!" << endl;
			return false;
		}
		Map_Nine* Old_Map = Now_Map;
		Old_Map->Del_Act(this);
		Now_Map = t_Now_Map;
		Now_Map->Add_Act(this);

		for (int i = 0; i < Max_Neighbor_Size; i++) {
			int Old_Uuid = Old_Map->m_neighbor[i];
			if (!Old_Uuid)
				continue;
			bool IsFind = false;
			for (int j = 0; j < Max_Neighbor_Size; j++) {
				if (Now_Map->m_neighbor[j] == Old_Uuid) {
					IsFind = true;
					break;
				}
			}
			if (IsFind)
				continue;
			Map_Nine* t_Map = dynamic_cast<Map_Mgr_Nine*>(Map_Mgr)->GetMapLightByUid(Old_Uuid);
			if (!t_Map)
				continue;
			t_Map->Leave_View(this);
		}

		SendMoveMsg();

		for (int i = 0; i < Max_Neighbor_Size; i++) {
			int New_Uuid = Now_Map->m_neighbor[i];
			if (!New_Uuid)
				continue;
			bool IsFind = false;
			for (int j = 0; j < Max_Neighbor_Size; j++) {
				if (Old_Map->m_neighbor[j] == New_Uuid) {
					IsFind = true;
					break;
				}
			}
			if (IsFind && ViewComp->IsFull())
				continue;
			Map_Nine* t_Map = dynamic_cast<Map_Mgr_Nine*>(Map_Mgr)->GetMapLightByUid(New_Uuid);
			if (!t_Map)
				continue;
			t_Map->Enter_View(this);
		}

	}
	return true;
}

void Map_Nine::Enter_View(Auctor_Nine* v_Act)
{
	if (!v_Act || !v_Act->ViewComp || v_Act->ViewComp->IsFull())
		return;
	for (auto iter = Act_in_Area.begin(); iter != Act_in_Area.end(); iter++)
	{
		if (v_Act == iter->second)
			continue;
		if (!v_Act->ViewComp->Add_View(dynamic_cast<Auctor_Nine*>(iter->second)))
			continue;
		cout << v_Act->m_uid << " Enter View Map to act " << iter->first << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		//cout << v_Act->m_uid << " Enter View Map to other " << (*iter)->m_uid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Enter_Count();
	}
}

void Map_Nine::Leave_View(Auctor_Nine* v_Act)
{
	if (!v_Act)
		return;
	for (auto iter = Act_in_Area.begin(); iter != Act_in_Area.end(); iter++)
	{
		if (v_Act == iter->second)
			continue;
		if (!v_Act->ViewComp->Del_View(dynamic_cast<Auctor_Nine*>(iter->second)))
			continue;
		cout << v_Act->m_uid << " Leave View Map from act " << iter->first << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		//cout << v_Act->m_uid << " Leave View Map from other " << (*iter)->m_uid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Leave_Count();
	}
}

Map_Nine* Map_Mgr_Nine::GetMapLightByUid(int Uid)
{
	auto iter = Map_Manger.find(Uid);
	if (iter == Map_Manger.end()) {
		cout << "find map by uid error!!! " << Uid << endl;
		return NULL;
	}
	return iter->second;
}

Map_Mgr_Nine::~Map_Mgr_Nine()
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Nine* Act = dynamic_cast<Auctor_Nine*>(*iter);
		if (!Act)
			continue;
		delete Act;
		New_Count--;
	}

	for (auto iter = Map_Manger.begin(); iter != Map_Manger.end(); ++iter)
	{
		Map_Nine* t_map = iter->second;
		if (!t_map)
			continue;
		delete t_map;
		New_Count--;
	}
	Map_Manger.clear();
}

void Map_Mgr_Nine::Show()
{
	int All = 0;
	for (auto iter = Map_Manger.begin(); iter != Map_Manger.end(); ++iter)
	{
		Map_Nine* t_map = iter->second;
		if (!t_map)
			continue;
		//cout << "nine map real actnum = " << t_map->Act_in_Area.size() << endl;
		All += t_map->Act_in_Area.size();
	}
	//cout << "all real actnum = " << All << endl;
}

bool Map_Mgr_Nine::Create_Map_In_Begin()
{
	int Ind_x = 1;
	while (Ind_x < Max_Grid_Size)
	{
		int Ind_y = 1;
		while (Ind_y < Max_Grid_Size)
		{
			Map_Nine* New_Map = new Map_Nine(Ind_x, Ind_y);
			CHECK_NULL(New_Map, "Create New Map Error!!!");
			New_Count++;

			Map_Manger.insert(pair<int, Map_Nine*>(New_Map->m_uuid, New_Map));
			

			Ind_y += 3;
		}
		Ind_x += 3;
	}
	cout << "New_Map Uuid = " << Map_Manger.size() << endl;
	return true;
}


void Map_Mgr_Nine::Move(int TickTime)
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Nine* Act = dynamic_cast<Auctor_Nine*>(*iter);
		if (!Act)
			continue;

		if (!Act->Move(TickTime))
			continue;
	}
}

void Map_Mgr_Nine::AddActor(Auctor_Base* act)
{
	Auctor_Nine* Act = dynamic_cast<Auctor_Nine*>(act);
	if (!Act)
		return;
	Auctor_Set.push_back(act);
	int Map_Uid = Calc_Map_Uuid_By_Coordinate(Act->m_x, Act->m_y);

	Map_Nine* Now_Map = GetMapLightByUid(Map_Uid);
	if (!Now_Map)
		return;
	Act->Now_Map = Now_Map;

	Now_Map->Add_Act(Act);

	for (int idx = 0; idx < Max_Neighbor_Size; idx++) {
		int Nei_Map_Uid = Now_Map->m_neighbor[idx];
		if (Nei_Map_Uid == 0) {
			continue;
		}
		Map_Nine* Nei_Map = GetMapLightByUid(Nei_Map_Uid);
		if (!Nei_Map) {
			cout << "Get Neighbor Map Error!!!" << Nei_Map_Uid << endl;
			continue;
		}
		if (Act->ViewComp->IsFull())
			return;
		Nei_Map->Enter_View(Act);
	}
}