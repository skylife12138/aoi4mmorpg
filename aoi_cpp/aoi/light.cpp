#include "light.h"
extern int New_Count;
extern Map_Mgr_Base* Map_Mgr;

Auctor_Light::Auctor_Light(int uid, int x, int y):Auctor_Base(uid, x, y)
{
	Now_Map = NULL;
}

Auctor_Light::~Auctor_Light()
{
	Now_Map = NULL;

	View_Set.clear();
}

bool Auctor_Light::FindActInView(Auctor_Base* act)
{
	return View_Set.find(act) != View_Set.end();
}

void Auctor_Light::AddLightView(Auctor_Base* act)
{
	View_Set.insert(act);
}

void Auctor_Light::DelLightView(Auctor_Base* act)
{
	auto iter = View_Set.find(act);
	if (iter != View_Set.end())
		View_Set.erase(iter);
}

void Auctor_Light::EntetView()
{
	if (!Now_Map)
		return;
	for (auto iter = Now_Map->Observer_Set.begin(); iter != Now_Map->Observer_Set.end(); iter++)
	{
		if (IsViewFull())
			return;
		Auctor_Light* t_Act = dynamic_cast<Auctor_Light*>(iter->second);
		if (!t_Act || this == iter->second)
			continue;
		
		if (FindActInView(t_Act) || t_Act->IsViewFull() || t_Act->FindActInView(this))
			continue;
		AddLightView(t_Act);
		t_Act->AddLightView(this);
		Map_Mgr->Add_Enter_Count();
		cout << m_uid << " enter new map " << t_Act->m_uid << " mapid = " << Now_Map->m_uuid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Send_Move_Msg_Count();
	}
}

void Auctor_Light::LeaveView(Map_Light* t_map)
{
	if (!t_map)
		return;
	for (auto iter = t_map->Act_in_Area.begin(); iter != t_map->Act_in_Area.end(); iter++)
	{
		Auctor_Light* t_Act = dynamic_cast<Auctor_Light*>(iter->second);
		if (!t_Act || this == t_Act)
			continue;

		if (!FindActInView(t_Act) || !t_Act->FindActInView(this))
			continue;
		DelLightView(t_Act);
		t_Act->DelLightView(this);
		Map_Mgr->Add_Leave_Count();
		cout << m_uid << " leave old map " << t_Act->m_uid << " mapid = " << t_map->m_uuid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Send_Move_Msg_Count();
	}
}

void Auctor_Light::MoveView()
{
	for (auto iter = View_Set.begin(); iter != View_Set.end(); iter++)
	{
		Auctor_Light* t_Act = dynamic_cast<Auctor_Light*>(*iter);
		if (!t_Act)
			continue;
		cout << m_uid << " move map " << t_Act->m_uid << " mapid = " << Now_Map->m_uuid << endl;
		Map_Mgr->Add_Send_Move_Msg_Count();
		Map_Mgr->Add_Move_Count();
	}
}

bool Auctor_Light::Move(int TickTime)
{
	if (!Auctor_Base::Move(TickTime))
		return false;
	int Now_Map_Uid = Calc_Map_Uuid_By_Coordinate(m_x, m_y);
	if (!Now_Map) {
		cout << "Act Now map error!!!" << endl;
		return false;
	}
	if (Now_Map->m_uuid == Now_Map_Uid) {
		MoveView();
	}
	else
	{
		Map_Light* t_Now_Map = dynamic_cast<Map_Mgr_Light*>(Map_Mgr)->GetMapLightByUid(Now_Map_Uid);
		if (!t_Now_Map)
			return false;
		if (t_Now_Map == Now_Map) {
			cout << "Now Map Error 2!!!" << endl;
			return false;
		}
		Map_Light* Old_Map = Now_Map;
		Old_Map->Del_Act(this);
		Now_Map = t_Now_Map;
		Now_Map->Add_Act(this);

		for (int i = 0; i < Max_Neighbor_Size; i++) {
			int Old_Uuid = Old_Map->m_neighbor[i];
			bool IsFind = false;
			for (int j = 0; j < Max_Neighbor_Size; j++) {
				if (Now_Map->m_neighbor[j] == Old_Uuid) {
					IsFind = true;
					break;
				}	
			}
			if (IsFind || !Old_Uuid)
				continue;
			Map_Light* t_Map = dynamic_cast<Map_Mgr_Light*>(Map_Mgr)->GetMapLightByUid(Old_Uuid);
			if (!t_Map)
				continue;
			t_Map->Del_Observer_Act(this);
			LeaveView(t_Map);
		}

		for (int i = 0; i < Max_Neighbor_Size; i++) {
			int New_Uuid = Now_Map->m_neighbor[i];
			bool IsFind = false;
			for (int j = 0; j < Max_Neighbor_Size; j++) {
				if (Old_Map->m_neighbor[j] == New_Uuid) {
					IsFind = true;
					break;
				}
			}
			if (IsFind || !New_Uuid)
				continue;
			Map_Light* t_Map = dynamic_cast<Map_Mgr_Light*>(Map_Mgr)->GetMapLightByUid(New_Uuid);
			if (!t_Map)
				continue;
			t_Map->Add_Observer_Act(this);
		}

		MoveView();
		EntetView();
	}
	return true;
}

Map_Light::~Map_Light()
{
	Observer_Set.clear();
}

void Map_Light::Add_Observer_Act(Auctor_Base* Act)
{
	if (!Act)
		return;
	Observer_Set.insert(pair<int, Auctor_Base*>(Act->m_uid, Act));
}

void Map_Light::Del_Observer_Act(Auctor_Base* Act)
{
	if (!Act)
		return;
	auto iter = Observer_Set.find(Act->m_uid);
	if (iter != Observer_Set.end())
		Observer_Set.erase(iter);
}

Map_Light* Map_Mgr_Light::GetMapLightByUid(int Uid)
{
	auto iter = Map_Manger.find(Uid);
	if (iter == Map_Manger.end()) {
		cout << "find map by uid error!!! " << Uid << endl;
		return NULL;
	}
	return iter->second;
}

Map_Mgr_Light::~Map_Mgr_Light()
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Light* Act = dynamic_cast<Auctor_Light*>(*iter);
		if (!Act)
			continue;
		delete Act;
		New_Count--;
	}

	for (auto iter = Map_Manger.begin(); iter != Map_Manger.end(); ++iter)
	{
		Map_Light* t_map = iter->second;
		if (!t_map)
			continue;
		delete t_map;
		New_Count--;
	}
	Map_Manger.clear();
}

bool Map_Mgr_Light::Create_Map_In_Begin()
{
	int Ind_x = 1;
	while (Ind_x < Max_Grid_Size)
	{
		int Ind_y = 1;
		while (Ind_y < Max_Grid_Size)
		{
			Map_Light* New_Map = new Map_Light(Ind_x, Ind_y);
			CHECK_NULL(New_Map, "Create New Map Error!!!");
			New_Count++;
			if (Map_Manger.find(New_Map->m_uuid) != Map_Manger.end())
				cout << "ERROR" << New_Map->m_uuid << endl;
			Map_Manger.insert(pair<int, Map_Light*>(New_Map->m_uuid, New_Map));

			Ind_y += 3;
		}
		Ind_x += 3;
	}
	cout << "New_Map Uuid = " << Map_Manger.size() << endl;
	return true;
}


void Map_Mgr_Light::Move(int TickTime)
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Light* Act = dynamic_cast<Auctor_Light*>(*iter);
		if (!Act)
			continue;
		
		if (!Act->Move(TickTime))
			continue;
	}
}

void Map_Mgr_Light::Show()
{
	int All = 0;
	for (auto iter = Map_Manger.begin(); iter != Map_Manger.end(); ++iter)
	{
		Map_Light* t_map = iter->second;
		if (!t_map)
			continue;
		//cout << "light map real actnum = " << t_map->Act_in_Area.size() << " Observersize = " << t_map->Observer_Set.size() << endl;
		All += t_map->Act_in_Area.size();
	}
	//cout << "all real actnum = " << All << endl;
}

void Map_Mgr_Light::AddActor(Auctor_Base* act)
{
	Auctor_Light* Act = dynamic_cast<Auctor_Light*>(act);
	if (!Act)
		return;
	Auctor_Set.push_back(act);
	int Map_Uid = Calc_Map_Uuid_By_Coordinate(Act->m_x, Act->m_y);
	
	Map_Light* Now_Map = GetMapLightByUid(Map_Uid);
	if (!Now_Map)
		return;
	Act->Now_Map = Now_Map;

	Now_Map->Add_Act(Act);
	Act->EntetView();

	for (int idx = 0; idx < Max_Neighbor_Size; idx++) {
		int Nei_Map_Uid = Now_Map->m_neighbor[idx];
		if (Nei_Map_Uid == 0) {
			continue;
		}
		Map_Light* Nei_Map = GetMapLightByUid(Nei_Map_Uid);
		if (!Nei_Map) {
			cout << "Get Neighbor Map Error!!!" << Nei_Map_Uid << endl;
			continue;
		}
		Nei_Map->Add_Observer_Act(Act);
	}
}