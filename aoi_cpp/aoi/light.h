#ifndef _LIGHT_H
#define _LIGHT_H

#include "common.h"

class Map_Light;
class Map_Mgr_Light;

class Auctor_Light : public Auctor_Base
{
public:
	Auctor_Light(int uid, int x, int y);
	~Auctor_Light();

	bool Move(int TickTime);
	void Leave() {};

	bool IsViewFull() { return int(View_Set.size()) >= Max_View_Num; }
	bool FindActInView(Auctor_Base* act);
	void AddLightView(Auctor_Base* act);
	void DelLightView(Auctor_Base* act);
	

	void EntetView();
	void LeaveView(Map_Light* t_map);
	void MoveView();
public:
	Map_Light* Now_Map;

	// ��һ�ε���Ұ����(����֪ͨ�뿪��Ұ����֤����Ұ�ڶ�������������Ұ����ʱ�������ÿһ֡��Ұ�ڶ���һ������)
	set<Auctor_Base*> View_Set;
};

class Map_Light :public Map_Base
{
public:
	Map_Light(int Center_x, int Center_y) :Map_Base(Center_x, Center_y) {}
	~Map_Light();
	void Add_Observer_Act(Auctor_Base* Act);
	void Del_Observer_Act(Auctor_Base* Act);
public:
	// �����Ĺ۲��б�
	map<int, Auctor_Base*> Observer_Set;
};

class Map_Mgr_Light : public Map_Mgr_Base
{
public:
	Map_Mgr_Light() {}
	~Map_Mgr_Light();
	bool Create_Map_In_Begin();
	Map_Light* GetMapLightByUid(int Uid);

	void Show();
	void Move(int TickTime);
	void AddActor(Auctor_Base* act);

private:
	map<int, Map_Light*> Map_Manger;
};

#endif

