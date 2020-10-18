#ifndef _LINK_H
#define _LINK_H

#include "common.h"


class Auctor_Link;
class Map_Link;
class Map_Mgr_Link;

class Auctor_Link : public Auctor_Base
{
public:
	Auctor_Link(int uid, int x, int y);
	~Auctor_Link();

	bool Move(int TickTime);
	void Leave() {};

	bool IsViewFull() {return int(View_Set.size()) >= Max_View_Num;}
	bool FindActInView(Auctor_Base* act);
	void AddLinkView(Auctor_Base* act);
	void DelLinkView(Auctor_Base* act);
	set<Auctor_Base*> Get_Link_View_Data();

	void EnterView(set<Auctor_Base*> viewset);
	void LeaveView(set<Auctor_Base*> viewset);
	void MoveView(set<Auctor_Base*> viewset);
	void SendLinkMsg(const char* logmsg, Auctor_Base* Act);

	void SetViewSet(set<Auctor_Base*> viewset) { View_Set = viewset; }
public:
	Node* x_Node;
	Node* y_Node;
	
	// 上一次的视野集合(用于通知离开视野，保证当视野内对象数量超过视野上限时不会出现每一帧视野内对象不一致现象)
	set<Auctor_Base*> View_Set; 
};

class Map_Mgr_Link : public Map_Mgr_Base
{
public:
	Map_Mgr_Link() {}
	~Map_Mgr_Link();

	void Show() {}
	void Move(int TickTime);
	void AddActor(Auctor_Base* act);
	void MoveActor(Auctor_Link* act);

	int GetLinkListSize_x() { return x_LinkList.GetLen(); }
	int GetLinkListSize_y() { return y_LinkList.GetLen(); }

private:
	LinkList x_LinkList;
	LinkList y_LinkList;
};

#endif



