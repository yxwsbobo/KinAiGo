// GTPTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <KinAiGo\GTPProgram.h>
#include <KinAiGo\GTPController.h>
#include <KinAiGo\GameController.h>
#include <iostream>
#include <chrono>
using namespace KinAiGo;
using namespace std::chrono;
constexpr char* GTPPath = "C:\\LeelaZero\\leelaz.exe";
constexpr char* GTPArg = "--gtp --noponder --visits 10 --weights \"C:\\LeelaZero\\last\"";
constexpr char* GTPArgLOG = "--gtp --noponder --visits 1600 -l \"E:\\temp.sgf\" --weights \"C:\\LeelaZero\\last\"";

void ptGet(std::string s)
{
	if (s.find("resign") != std::string::npos)
	{
		if (s == "resign")
		{
			std::cout << "Region ))))" << std::endl;
			//return;
		}
	}

	std::cout << " get :" << s << std::endl;
}

void On1Play(uint8_t x, uint8_t y)
{
	if ((x == 105) && (y == 106))
	{
		std::cout << "region !!!!!!!!!!!!!!" << std::endl;
		return;
	}
	std::cout << " p1 Play  X: " << (int)x << "   Y: " << (int)y << std::endl;
}

void On2Play(uint8_t x, uint8_t y)
{
	if ((x == 105) && (y == 106))
	{
		std::cout << "region !!!!!!!!!!!!!!" << std::endl;
		return;
	}
	std::cout << " p2 Play  X: " << (int)x << "   Y: " << (int)y << std::endl;
}


int main()
{
	auto p1 = std::make_shared<GTPController>(GTPPath, GTPArg);
	auto p2 = std::make_shared<GTPController>(GTPPath, GTPArg);


	p1->OnGamerPlay.Add(On1Play);
	p2->OnGamerPlay.Add(On2Play);
	p1->GetEngine()->OnRecieved.Add(ptGet);
	p2->GetEngine()->OnRecieved.Add(ptGet);
	GameController Game{ p1,p2 };

	Game.Run();
	
	//while (true)
	//{
	//	getchar();
	//	Game.Stop();
	//	getchar();
	//	Game.Run();
	//}
	//getchar();

	//Game.Stop();
	//p1 = nullptr;
	//p2 = nullptr;
	//p1 = std::make_shared<GTPController>(GTPPath, GTPArg);
	//Game.ResetP1(p1);

	//getchar();

	//p1->OnGamerPlay.Add(On1Play);
	//Game.Run();
	getchar();
	Game.Stop();

    return 0;
}

