#include "GTPController.h"
#include "GTPProgram.h"
#include <iostream>
#include <cctype>
#include <sstream>
#include <KinBase\KinBase.h>

KinAiGo::GTPController::GTPController(std::string Path, std::string cmd)
{
	Engine = std::make_shared<GTPProgram>(Path, cmd);
	Engine->OnRecieved.Add(std::bind(&GTPController::EngineInfo, this, std::placeholders::_1));
}


KinAiGo::GTPController::~GTPController()
{
}


void KinAiGo::GTPController::InitBoard()
{
	std::string InitCmd;
	InitCmd = "boardsize ";
	InitCmd += std::to_string(BoardSize);
	*Engine << InitCmd << "clear_board";
	InitCmd = "komi ";
	InitCmd += std::to_string(Komi);
	*Engine << InitCmd;
}

void KinAiGo::GTPController::Play(int8_t x, int8_t y)
{
	GoPoint pt{ x,y };
	Биец(pt.valid(BoardSize), "Error x y");
	std::string Result = "play ";
	if (CurrentSteps & 1)
	{
		Result += "W ";
	}
	else
	{
		Result += "B ";
	}

	Result += PositionToString({ x,y });

	*Engine << Result;

	CurrentSteps++;
}

void KinAiGo::GTPController::GamerPlay()
{
	std::string Result = "genmove ";
	if (CurrentSteps & 1)
	{
		Result += "W ";
	}
	else
	{
		Result += "B ";
	}
	
	*Engine << Result;
	CurrentSteps++;
}

void KinAiGo::GTPController::Pass()
{
	CurrentSteps++;
}

int KinAiGo::GTPController::Start()
{
	CurrentSteps = 0;
	RunFlag = 1;
	InitBoard();
	Engine->AutoRead();

	return 1;
}

void KinAiGo::GTPController::Stop()
{
	RunFlag = 0;
	InitBoard();
}



std::shared_ptr<KinAiGo::GTPProgram> KinAiGo::GTPController::GetEngine()
{
	return Engine;
}

void KinAiGo::GTPController::EngineInfo(std::string Info)
{
	if (Info.find("visits, score") != std::string::npos)
	{
		WinRate = "KinGo GTP Info :" + Info + "\n";
		//OutputDebugStringA(WinRate.c_str());
	}
	//OutputDebugStringA((std::string("KinEngineInfo :") + Info).c_str());

	if (Info.size() >=9)
	{
		return;
	}
	std::stringstream ss(Info);

	char c;
	std::string Pos;

	ss >> c >> Pos;
	if (c != '=')
	{
		return;
	}

	if (Pos == "resign")
	{
		OnGamerPlay(105, 106);
		return;
	}


	if (Pos.size()<=0 || Pos.size() >3)
	{
		return;
	}

	auto Pt = StringToPosition(Pos);
	if (Pt.valid(BoardSize))
	{
		if (RunFlag == 0)
		{
			return;
		}
		OnGamerPlay(Pt.x, Pt.y);
		
	}

}

std::string KinAiGo::GTPController::PositionToString(const GoPoint Pos)
{
	std::string Result;
	int x = Pos.x;
	if (x >= 8)
	{
		x++;
	}
	Result += ('A' + x);
	Result += std::to_string(Pos.y + 1);

	return Result;
}

KinAiGo::GoPoint KinAiGo::GTPController::StringToPosition(std::string Pos)
{
	GoPoint Result;
	Result.x = std::toupper(Pos[0]) - 'A';
	if (Result.x >=9)
	{
		Result.x--;
	}
	Pos.erase(0, 1);
	Result.y = std::atoi(Pos.c_str()) -1;

	return Result;
}
