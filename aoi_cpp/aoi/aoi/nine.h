#ifndef _NINE_H
#define _NINE_H

#include <vector>
#include "common.h"


class Auctor_Nine;
class Map_Nine;
class Map_Mgr_Nine;

struct View_Data
{
	View_Data(Auctor_Nine* v_act, int index) :View_Act(v_act), Index(index) {}
	Auctor_Nine* View_Act;
	int Index;
};

class ViewComponent
{
public:
	ViewComponent(Auctor_Nine* Act) :View_Owner_Act(Act) {}
	~ViewComponent() {
		View_Owner_Act = NULL;
		View_List.clear();
	}

	bool IsFull();
	bool Is_In_View(Auctor_Nine* v_Act);

	bool Add_View(Auctor_Nine* v_Act);
	bool Del_View(Auctor_Nine* v_Act);
	bool Del_View_By_Index(int Index);

public:
	Auctor_Nine* View_Owner_Act;
	vector<View_Data> View_List;
};

class Auctor_Nine : public Auctor_Base
{
public:
	Auctor_Nine(int uid, int x, int y);
	~Auctor_Nine();

	bool Move(int TickTime);
	void Leave() {};
	void SendMoveMsg();
public:
	Map_Nine* Now_Map;

	ViewComponent* ViewComp;
};

class Map_Nine :public Map_Base
{
public:
	Map_Nine(int Center_x, int Center_y) :Map_Base(Center_x, Center_y) {}
	~Map_Nine() {}
	void Enter_View(Auctor_Nine* v_Act);
	void Leave_View(Auctor_Nine* v_Act);


};

class Map_Mgr_Nine : public Map_Mgr_Base
{
public:
	Map_Mgr_Nine() {}
	~Map_Mgr_Nine();
	bool Create_Map_In_Begin();
	Map_Nine* GetMapLightByUid(int Uid);

	void Show();
	void Move(int TickTime);
	void AddActor(Auctor_Base* act);

private:
	map<int, Map_Nine*> Map_Manger;
};

#endif


