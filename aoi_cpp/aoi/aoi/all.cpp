
#include "all.h"
extern int New_Count;
extern Map_Mgr_Base* Map_Mgr;

bool Auctor_All::Move(int TickTime)
{
	return Auctor_Base::Move(TickTime);
}

Map_Mgr_All::~Map_Mgr_All()
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_All* Act = dynamic_cast<Auctor_All*>(*iter);
		if (!Act)
			continue;
		delete Act;
		New_Count--;
	}
}

void Map_Mgr_All::Move(int TickTime)
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_All* Act = dynamic_cast<Auctor_All*>(*iter);
		if(!Act)
			continue;

		if(!Act->Move(TickTime))
			continue;
		Send_Move_Msg(Act);
	}
}

void Map_Mgr_All::AddActor(Auctor_Base* act)
{ 
	Auctor_Set.push_back(act);
	Send_Move_Msg(act, true);
}

void Map_Mgr_All::Send_Move_Msg(Auctor_Base* Act, bool isAdd)
{
	for (auto iter = Auctor_Set.begin(); iter != Auctor_Set.end(); ++iter)
	{
		Auctor_Base* m_Act = *iter;
		if (m_Act == Act)
			continue;
		Add_Send_Move_Msg_Count();
		if (isAdd)//添加时相互的，所以发送两遍
			Add_Send_Move_Msg_Count();
	}
}






