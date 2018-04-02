#include "GameController.h"
#include "GoGamer.h"
#include <Windows.h>
#include <string>

KinAiGo::GameController::GameController(std::shared_ptr<GoGamer> p1, std::shared_ptr<GoGamer> p2)
	:Player1{ p1 }, Player2{ p2 }
{
	Player1->OnGamerPlay.Add(std::bind(&GameController::Player1Played, this, std::placeholders::_1, std::placeholders::_2));
	Player2->OnGamerPlay.Add(std::bind(&GameController::Player2Played, this, std::placeholders::_1, std::placeholders::_2));
}


KinAiGo::GameController::~GameController()
{
}

void KinAiGo::GameController::Run(int Player, double Komi)
{

	RunFlag = 1;
	Player1->Komi = Komi;
	Player2->Komi = Komi;

	if (Player1->Start() == 0)
	{
		return;
	}



	if (Player2->Start() == 0)
	{
		return;
	}
	OutputDebugStringA("KinGo: GameController Run");

	try
	{
		if (Player == 1)
		{
			Player1->GamerPlay();
		}
		else
		{
			Player2->GamerPlay();
		}
	}
	catch (...)
	{
		OutputDebugStringA("KinGo: FirstPlay Error");
	}


}

void KinAiGo::GameController::pause()
{
	auto a = Player2->GetColor();
	if (a == 1)
	{
		OutputDebugString(L"KinGo : 对手是黑色");
	}
	if (a == 2)
	{
		OutputDebugString(L"KinGo: 对手是白色");
	}
}

void KinAiGo::GameController::Stop()
{

	RunFlag = 0;
	Player1->Stop();
	Player2->Stop();

}

void KinAiGo::GameController::ResetP1(std::shared_ptr<GoGamer> Player)
{
	Player1 = Player;
	Player1->OnGamerPlay.Add(std::bind(&GameController::Player1Played, this, std::placeholders::_1, std::placeholders::_2));
}

void KinAiGo::GameController::Player1Played(uint8_t x, uint8_t y)
{
	try
	{
		if (RunFlag == 0)
		{
			return;
		}
		//std::string Info = "KinGo:  Player1 Played X:" + std::to_string(x) + "  Y:" + std::to_string(y) + "\n";
		//OutputDebugStringA(Info.c_str());

		if ((x == 105) && (y == 106))
		{
			Player2->Resign();
			OutputDebugStringA("KinGo: Region");
			return;
		}


		Player2->Play(x, y);
		Player2->GamerPlay();
	}
	catch (...)
	{
		OutputDebugStringA("KinGo :Player1 Played Error");
	}

}

void KinAiGo::GameController::Player2Played(uint8_t x, uint8_t y)
{
	try
	{
		if (RunFlag == 0)
		{
			return;
		}
		//std::string Info = "KinGo:  Player2 Played X:" + std::to_string(x) + "  Y:" + std::to_string(y) + "\n";
		//OutputDebugStringA(Info.c_str());
		Player1->Play(x, y);
		Player1->GamerPlay();
	}
	catch (...)
	{
		OutputDebugStringA("KinGo : Player2 Played Error");
	}

}
