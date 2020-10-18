#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#include "define.h"
#include "all.h"
#include "light.h"
#include "nine.h"
#include "link.h"

void sleepcp(int milliseconds) 
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif 
}

extern int New_Count;
extern Map_Mgr_Base* Map_Mgr;

enum E_MoudelType
{
	EMT_All = 0,
	EMT_Light = 1,
	EMT_Nine = 2,
	EMT_Link = 3,

	EMT_Max
};



int main(int argc, char* argv[])
{
	/*if (argc != 3) {
		cout << "Error argc " << argc << endl;
		return 0;
	}*/
	//srand(unsigned int(time(0)));
	srand(11);
	for (int i = 0; i < argc; i++)
		cout << "Argument is " << argv[i] << endl;

	int MoudleType = 1;//atoi(argv[1]);
	if (MoudleType > EMT_Max || MoudleType < EMT_All) {
		cout << "moudle tpye " << MoudleType << endl;
		return 0;
	}
	int ActNum = 10;// atoi(argv[2]);

	
	switch (MoudleType)
	{
		case EMT_All:
		{
			Map_Mgr = new Map_Mgr_All();
			CHECK_NULL(Map_Mgr, "Create Map_Mgr_All Error!!!");
			New_Count++;
			Auctor_All data(1, 1, 1);
			cout << sizeof(data) << endl;
			for (int index = 0; index < ActNum; index++) {
				int x = RandInt(0, Max_Grid_Size-1);
				int y = RandInt(0, Max_Grid_Size-1);
				cout << "x = " << x << " y = " << y << endl;
				Auctor_All* Act = new Auctor_All(index, x, y);
				CHECK_NULL(Act, "Create Actor_All Error!!!");
				New_Count++;
				Map_Mgr->AddActor(Act);
			}
		}
		break;
		case EMT_Light:
		{
			Map_Mgr = new Map_Mgr_Light();
			CHECK_NULL(Map_Mgr, "Create Map_Mgr_Light Error!!!");

			if (!dynamic_cast<Map_Mgr_Light*>(Map_Mgr)->Create_Map_In_Begin()) {
				return 0;
			}
			New_Count++;
			/*Auctor_Light* Act1 = new Auctor_Light(10000, 1, 1);
			CHECK_NULL(Act1, "Create Auctor_Light Error!!!");
			Map_Mgr->AddActor(Act1);

			Auctor_Light* Act2 = new Auctor_Light(10001, 0, 1);
			CHECK_NULL(Act2, "Create Auctor_Light Error!!!");
			Map_Mgr->AddActor(Act2);

			Auctor_Light* Act3 = new Auctor_Light(10002, 2, 1);
			CHECK_NULL(Act3, "Create Auctor_Nine Error!!!");
			Map_Mgr->AddActor(Act3);*/

			for (int index = 0; index < ActNum; index++) {
				int x = RandInt(0, Max_Grid_Size-1);
				int y = RandInt(0, Max_Grid_Size-1);
				cout << "index = " << index << " x = " << x << " y = " << y << endl;
				Auctor_Light* Act = new Auctor_Light(index, x, y);
				CHECK_NULL(Act, "Create Auctor_Light Error!!!");
				New_Count++;
				Map_Mgr->AddActor(Act);
			}
		}
		break;
		case EMT_Nine:
		{
			Map_Mgr = new Map_Mgr_Nine();
			CHECK_NULL(Map_Mgr, "Create Map_Mgr_Nine Error!!!");

			if (!dynamic_cast<Map_Mgr_Nine*>(Map_Mgr)->Create_Map_In_Begin()) {
				return 0;
			}
			New_Count++;
			/*Auctor_Nine* Act1 = new Auctor_Nine(10000, 1, 1);
			CHECK_NULL(Act1, "Create Auctor_Nine Error!!!");
			Map_Mgr->AddActor(Act1);

			Auctor_Nine* Act2 = new Auctor_Nine(10001, 0, 1);
			CHECK_NULL(Act2, "Create Auctor_Nine Error!!!");
			Map_Mgr->AddActor(Act2);

			Act2->Now_Map->Leave_View(Act2);

			Auctor_Nine* Act3 = new Auctor_Nine(10002, 2, 1);
			CHECK_NULL(Act3, "Create Auctor_Nine Error!!!");
			Map_Mgr->AddActor(Act3);*/

			for (int index = 0; index < ActNum; index++) {
				int x = RandInt(0, Max_Grid_Size - 1);
				int y = RandInt(0, Max_Grid_Size - 1);
				cout << "index = " << index << " x = " << x << " y = " << y << endl;
				Auctor_Nine* Act = new Auctor_Nine(index, x, y);
				CHECK_NULL(Act, "Create Auctor_Nine Error!!!");
				New_Count++;
				Map_Mgr->AddActor(Act);
			}
		}
		break;
		case EMT_Link:
		{
			Map_Mgr = new Map_Mgr_Link();
			CHECK_NULL(Map_Mgr, "Create Map_Mgr_Link Error!!!");

			/*Auctor_Link* Act1 = new Auctor_Link(10000, 1, 1);
			CHECK_NULL(Act1, "Create Auctor_Link Error!!!");
			Map_Mgr->AddActor(Act1);

			Auctor_Link* Act2 = new Auctor_Link(10001, 0, 1);
			CHECK_NULL(Act2, "Create Auctor_Link Error!!!");
			Map_Mgr->AddActor(Act2);

			Auctor_Link* Act3 = new Auctor_Link(10002, 2, 1);
			CHECK_NULL(Act3, "Create Auctor_Link Error!!!");
			Map_Mgr->AddActor(Act3);*/
			New_Count += sizeof(Map_Mgr);

			for (int index = 0; index < ActNum; index++) {
				int x = RandInt(0, Max_Grid_Size - 1);
				int y = RandInt(0, Max_Grid_Size - 1);
				cout << "x = " << x << " y = " << y << endl;
				Auctor_Link* Act = new Auctor_Link(index, x, y);
				CHECK_NULL(Act, "Create Auctor_Link Error!!!");
				New_Count += sizeof(Auctor_Link);
				Map_Mgr->AddActor(Act);
			}
		}
		break;
		default:
			break;
	}

	int Now_Tick = 1;
	cout << "Now_Tick =" << Now_Tick << " Send_Move_Msg_Count = " << Map_Mgr->Get_Send_Move_Msg_Count() << " ,Move Count = " << Map_Mgr->Get_Move_Count() << " ,Enter Count = " << Map_Mgr->Get_Enter_Count() << " ,Leave Count = " << Map_Mgr->Get_Leave_Count() << endl;

	while (Now_Tick <= Max_Move_Count)
	{
		Map_Mgr->Move(Now_Tick);
		Now_Tick++;
		cout << "Now_Tick =" << Now_Tick << " Send_Move_Msg_Count = " << Map_Mgr->Get_Send_Move_Msg_Count() << " ,Move Count = " << Map_Mgr->Get_Move_Count() << " ,Enter Count = " << Map_Mgr->Get_Enter_Count() << " ,Leave Count = " << Map_Mgr->Get_Leave_Count() << endl;
		Map_Mgr->Show();
		sleepcp(66);
	}
	if (Map_Mgr) {
		delete Map_Mgr;
		New_Count--;
	}
	cout << "Finally New Count = " << New_Count << endl;
	system("pause");
	return 0;
}