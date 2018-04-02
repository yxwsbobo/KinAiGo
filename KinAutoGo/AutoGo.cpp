#include "AutoGo.h"
#include<KinAiGo\FoxGoController.h>
#include <KinAiGo\GTPController.h>
#include <KinAiGo\GameController.h>
#include <fstream>
using namespace KinAiGo;

std::string GTPPath;
std::string GTPArg ;
KinAutoGo::AutoGo::AutoGo()
{
}


KinAutoGo::AutoGo::~AutoGo()
{
}

GameController* GameMain;
void (*Install)(void);
void(*Unstall)(void);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_USER + 1:
	{

		//auto p1 = std::make_shared<GTPController>(GTPPath, GTPArg);

		//GameMain->ResetP1(p1);
		auto Color = GameMain->Player2->GetColor();
		GameMain->Stop();
		if (Color == 1)
		{
			GameMain->Run(2);
		}
		if (Color == 2)
		{
			GameMain->Run(1);
		}
		//GameMain->Run(1);
		break;
	}
	case WM_USER + 2:
	{
		Unstall();
		Install();
		//auto p1 = std::make_shared<GTPController>(GTPPath, GTPArg);
		//GameMain->ResetP1(p1);
		//GameMain->Run(2);
		break;
	}
	case WM_USER + 3:
	{
		GameMain->Stop();
		break;
	}
	case WM_USER + 4:
	{
		Unstall();
		//GameMain->Player2->Resign();
		break;
	}
	case WM_DESTROY:
		Unstall();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void InitPara(void* hModule)
{
	char fp[256];
	GetModuleFileNameA((HMODULE)hModule, fp, 256);

	std::string Dir;
	std::string FilePath = fp;
	FilePath = FilePath.substr(0, FilePath.find_last_of('\\')+1);
	Dir = FilePath;
	FilePath += "Info.txt";
	OutputDebugStringA((std::string("Kin:") + FilePath).c_str());
	std::fstream fs{FilePath,std::fstream::in };

	std::getline(fs, GTPPath);
	std::getline(fs, GTPArg);
	OutputDebugStringA((std::string("Kin: ") + GTPPath + "    " + GTPArg).c_str());

	FilePath = Dir + "AutoWindowDll.dll";
	auto awdll = LoadLibraryA(FilePath.c_str());
	if (!awdll)
	{
		OutputDebugStringA("KinGo: AutoWindowDll Failed !!");
	}
	Install = (void(*)(void))GetProcAddress(awdll, "Install");
	if (!Install)
	{
		std::string Info{ "KinGo: GetProcAddress Install Failed !! Error Code :" };
		Info += std::to_string(GetLastError());
		OutputDebugStringA(Info.c_str());

	}
	Unstall = (void(*)(void))GetProcAddress(awdll, "Unstall");
	if (!Unstall)
	{
		std::string Info{ "KinGo: GetProcAddress Unstall Failed !! Error Code :" };
		Info += std::to_string(GetLastError());
		OutputDebugStringA(Info.c_str());
	}
}

 DWORD WINAPI  KinAutoGo::AutoGo::Run(void* hModule)
{
	 InitPara(hModule);

	 OutputDebugStringA("KinGo: Dll Run Success !!");

	 auto p1 = std::make_shared<GTPController>(GTPPath, GTPArg);
	 auto p2 = std::make_shared<FoxGoController>();

	 GameController Game{ p1,p2 };
	 GameMain = &Game;


	 WNDCLASSEXA wcex;
	 wcex.cbSize = sizeof(WNDCLASSEXA);
	 wcex.style = 0;
	 wcex.lpfnWndProc = WndProc;
	 wcex.cbClsExtra = 0;
	 wcex.cbWndExtra = 0;
	 wcex.hInstance = (HINSTANCE)hModule;
	 wcex.hIcon = NULL;
	 wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	 wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	 wcex.lpszMenuName = NULL;
	 wcex.lpszClassName = "KinFoxAiGoClass";
	 wcex.hIconSm = NULL;

	 RegisterClassExA(&wcex);

	 HWND hWnd = CreateWindowA("KinFoxAiGoClass", "KinFoxAiGoTittle", WS_OVERLAPPEDWINDOW,
		 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, nullptr, (HINSTANCE)hModule, nullptr);
	 if (!hWnd)
	 {
		 OutputDebugStringA("KinGo: CreateWindowFailed");
	 }
	 else
	 {
		 std::string ifo = "KinGo: CreateWindow Successed! hWnd is :" + std::to_string((uint32_t)hWnd) + "\n";
		 OutputDebugStringA(ifo.c_str());
	 }

	 MSG msg;
	 while (GetMessage(&msg, NULL, 0, 0))
	 {
		 TranslateMessage(&msg);
		 DispatchMessage(&msg);
	 }

	return 0;
}
