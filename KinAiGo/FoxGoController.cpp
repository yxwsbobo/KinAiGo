#include "FoxGoController.h"
#include <thread>
#include <string>
#include <chrono>
#include <KinBase\KinBase.h>
#include <codecvt>

using namespace std::chrono;

KinAiGo::FoxGoController::FoxGoController()
{
	auto ModuleAddress = (char*)GetModuleHandleA("foxwq.exe");
	BaseAddress = ModuleAddress + (uint32_t)BaseAddress;
	NameAddress = ModuleAddress + (uint32_t)NameAddress;
}


KinAiGo::FoxGoController::~FoxGoController()
{
}

void KinAiGo::FoxGoController::Play(int8_t x, int8_t y)
{
	auto pt = Board.GetPointFormStone(x, y);
	ClientToScreen(Board.hBoardWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	std::this_thread::sleep_for(200ms);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	CurrentSteps++;
}

void KinAiGo::FoxGoController::GamerPlay()
{
	CurrentSteps++;

	{
		std::lock_guard<std::mutex> lg(PlayFlagMutx);
		PlayeFlag = 1;
	}

}

void KinAiGo::FoxGoController::Pass()
{
	CurrentSteps++;
}

int KinAiGo::FoxGoController::Start()
{
	CurrentSteps = 0;
	RunFlag = 1;
	{
		std::lock_guard<std::mutex> lg(PlayFlagMutx);
		PlayeFlag = 0;
	}

	if (Board.updataBoardInfo() == 0)
	{
		OutputDebugStringA("KinGo: updataBoardInfo Failed");
		return 0;
	}
	OutputDebugStringA("KinGo: updataBoardInfo Success");

	if (updataRoomAddress() == 0)
	{
		OutputDebugStringA("KinGo: updataRoomAddress Failed");
		return 0;
	}
	OutputDebugStringA("KinGo: updataRoomAddress Success");
	std::thread(std::bind(&FoxGoController::CheckGamerPlay, this)).detach();
	return 1;
}

void KinAiGo::FoxGoController::Stop()
{

	RunFlag = 0;
	while (PlayOver)
	{
		std::this_thread::yield();
	}

}

#if _MSC_VER >= 1900

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	return convert.to_bytes(p, p + utf16_string.size());
}

#else

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16_string);
}

#endif

int KinAiGo::FoxGoController::GetColor()
{
	updataRoomAddress();
	//OutputDebugStringA((std::string("KinGo : PlayerNameAddress :") + std::to_string((uint32_t)NameAddress)).c_str());
	std::u16string PlayerName{ (char16_t*)NameAddress };
	auto WhiteName = GetWhitePlayerName();
	//OutputDebugStringA((std::string("KinGo : PlayerName is :") + utf16_to_utf8(PlayerName)
		//+ std::string("WhiteName is :") + utf16_to_utf8(WhiteName)).c_str());
	if (PlayerName == WhiteName)
	{
		return 1;
	}
	return 2;
}

void KinAiGo::FoxGoController::Resign()
{
	Board.ClickResign();
}

int KinAiGo::FoxGoController::updataRoomAddress()
{
	try
	{
		if (BaseAddress == 0)
		{
			return 0;
		}
		//OutputDebugStringA((std::string("KinAddress 1:") + std::to_string((uint32_t)BaseAddress)).c_str());
		char* RoomStart = *(char**)BaseAddress + 0x44;
		if (RoomStart == 0)
		{
			return 0;
		}
		//OutputDebugStringA((std::string("KinAddress 2:") + std::to_string((uint32_t)RoomStart)).c_str());
		//endRoomAddress is *(char**)BaseAddress + 0x48;

		//just first Room
		RoomStart = *(char**)RoomStart;
		if (RoomStart == 0)
		{
			return 0;
		}
		//OutputDebugStringA((std::string("KinAddress FirstRoom:") + std::to_string((uint32_t)RoomStart)).c_str());
		RoomStart = *(char**)RoomStart;
		if (RoomStart == 0)
		{
			return 0;
		}
		//OutputDebugStringA((std::string("KinAddress FirstRoomEntry:") + std::to_string((uint32_t)RoomStart)).c_str());
		RoomAddress = RoomStart;


		return 1;
	}
	catch (...)
	{
		return 0;
	}

}

void KinAiGo::FoxGoController::CheckGamerPlay()
{
	//KinAiGo::GoPoint pt;
	PlayOver = 1;
	while (RunFlag)
	{
		if (PlayeFlag)
		{
			if (CurrentSteps == GetNextStepNumber() - 1)
			{
				auto pt = GetLastPlayPoint();
				if (pt.valid(BoardSize))
				{
					{
						std::lock_guard<std::mutex> lg(PlayFlagMutx);
						PlayeFlag = 0;
					}
					//std::string Info = "KinGo In Fox:  Fox Played X:" + std::to_string(pt.x) + "  Y:" + std::to_string(pt.y) + "\n";
					//OutputDebugStringA(Info.c_str());
					OnGamerPlay(pt.x, pt.y);
				}
				else
				{
					OutputDebugStringA("KinGo: Invalid pt in FoxGoController::CheckGamerPlay");
				}
			}
		}

		std::this_thread::sleep_for(150ms);
	}
	OutputDebugStringA("KinGo: In CheckGamerPlay Over");
	PlayOver = 0;
}


int KinAiGo::FoxGoController::GetNextPlayColor()
{
	return *(int*)(RoomAddress + 0x308);
}

int KinAiGo::FoxGoController::GetNextStepNumber()
{
	return *(int*)(RoomAddress + 0x3b8);
}

std::u16string KinAiGo::FoxGoController::GetWhitePlayerName()
{
	//OutputDebugStringA((std::string("KinGo : PlayerNameAddress :") + std::to_string((uint32_t)(RoomAddress + 0x178))).c_str());
	std::u16string Result{ (char16_t*)(RoomAddress + 0x178) };

	return Result;
}

char * KinAiGo::FoxGoController::GetStoneList()
{
	return RoomAddress + 0x3cc;
}

KinAiGo::GoPoint KinAiGo::FoxGoController::GetLastPlayPoint()
{
	GoPoint Result;
	Result.x = -1;
	Result.y = -1;

	auto LastNumber = GetNextStepNumber() - 1;
	auto StoneList = GetStoneList();

	for (size_t i = 0; i != BoardSize*BoardSize; ++i)
	{
		auto CurrentStone = StoneList + i * 28;
		if (*(uint32_t*)(CurrentStone+20) == LastNumber)
		{
			Result.x = (int8_t)(i % BoardSize);
			Result.y = (int8_t)(BoardSize - (i / BoardSize) -1);
			break;
		}
	}

	return Result;
}


KinAiGo::FoxBoard::FoxBoard()
{

}

KinAiGo::FoxBoard::~FoxBoard()
{
}

BOOL KinAiGo::FoxBoard::MyEnumWindows(HWND hwnd, LPARAM lParam)
{
	char Buffer[128];
	auto n = GetWindowTextA(hwnd, Buffer, 127);
	if (n)
	{
		Buffer[n] = 0;
		std::string s{ Buffer };
		if (s.find("> [高级房") != std::string::npos)
		{
			auto Board = (FoxBoard*)lParam;

			Board->hWnd = hwnd;
		
			return false;
		}
	}

	return true;
}

void KinAiGo::FoxBoard::TrueClick(POINT pt)
{
	SetCursorPos(pt.x, pt.y);
	std::this_thread::sleep_for(500ms);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void KinAiGo::FoxBoard::Activite()
{
	SetForegroundWindow(hWnd);
}

int KinAiGo::FoxBoard::updataBoardInfo()
{
	try
	{
		hWnd = nullptr;
		hBoardWnd = nullptr;
		EnumWindows(MyEnumWindows, (LPARAM)this);
		必真(hWnd != nullptr, "");

		hBoardWnd = FindWindowExA(hWnd, 0, 0, 0);
		hBoardWnd = FindWindowExA(hBoardWnd, 0, 0, "CRoomPanel");
		hBoardWnd = FindWindowExA(hBoardWnd, 0, 0, "CChessboardPanel");
		必真(hBoardWnd != nullptr, "");

		RECT rc;
		必真(GetClientRect(hBoardWnd, &rc), "");

		int value = 16;

		int BoardSize = (rc.right - rc.left) - 55;
		int BoardInnerSize = BoardSize - (value * 2);
		IncreStep = BoardInnerSize;
		IncreStep /= 18;

		StartPoint.x = 10 + value;
		StartPoint.y = ((rc.bottom - rc.top) - BoardSize) / 2;
		StartPoint.y += value;
		StartPoint.y += BoardInnerSize;
	}
	catch (...)
	{
		return 0;
	}

	return 1;
}

POINT KinAiGo::FoxBoard::GetPointFormStone(int x, int y)
{
	POINT Result;
	Result.x = (LONG)(StartPoint.x + (x * IncreStep));
	Result.y = (LONG)(StartPoint.y - (y * IncreStep));
	return Result;
}



void KinAiGo::FoxBoard::ClickResign()
{
	//OutputDebugStringA("KinGo: In ClickResign");
	auto ChildWnd = FindWindowExA(hWnd, 0, 0, 0);
	ChildWnd = FindWindowExA(ChildWnd, 0, 0, "CRoomPanel");
	RECT rcf;
	GetWindowRect(ChildWnd, &rcf);
	auto fWidth = rcf.right - rcf.left;
	if (!ChildWnd)
	{
		OutputDebugStringA("KinGo : ChildWnd NOt Found");
	}
	auto NeedWnd = FindWindowExA(ChildWnd, 0, 0, 0);
	while (NeedWnd)
	{
		RECT rc;

		GetWindowRect(NeedWnd, &rc);
		auto Width = rc.right - rc.left;
		auto Height = rc.bottom - rc.top;

		std::string Info{ "KinGo : WndRect fWidth = " };

		//Info += std::to_string(fWidth);

		//Info += " Wnd Width :" + std::to_string(Width) + "HWND :" + std::to_string((uint32_t)NeedWnd);
		//Info += " Wnd Height: " + std::to_string(Height);

		//OutputDebugStringA(Info.c_str());

		if ( (Height == 32) && (Width == (fWidth - 351)) )
		{
			POINT pt;
			pt.x = Width - 50;
			pt.y = Height / 2;
			ClientToScreen(NeedWnd, &pt);
			std::thread(TrueClick, pt).detach();
			break;
		}
		NeedWnd = FindWindowExA(ChildWnd, NeedWnd, 0, 0);
	}
}
