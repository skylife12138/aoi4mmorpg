#ifndef _ALL_H
#define _ALL_H


#include "common.h"

class Auctor_All : public Auctor_Base
{
public:
	Auctor_All(int uid, int x, int y) :Auctor_Base(uid, x, y) {}
	~Auctor_All() {}

	bool Move(int TickTime);
	void Leave() {}
};


class Map_Mgr_All : public Map_Mgr_Base
{
public:
	Map_Mgr_All() {}
	~Map_Mgr_All();
	void AddActor(Auctor_Base* act);

	void Move(int TickTime);
	void Show() {}
	void Send_Move_Msg(Auctor_Base* Act, bool isAdd = false);

};

#endif
