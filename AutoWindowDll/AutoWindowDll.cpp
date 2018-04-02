// AutoWindowDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AutoWindowDll.h"
#include<string>
#include<thread>
#include <chrono>
#include <vector>
#include <Wingdi.h>
#include <Windows.h>
using namespace std::chrono;
// 这是导出变量的一个示例
AUTOWINDOWDLL_API int nAutoWindowDll=0;

// 这是导出函数的一个示例。
AUTOWINDOWDLL_API void Install(void)
{
	//OutputDebugString(L"KinGo: In Install");
	hhook = SetWindowsHookEx(WH_CBT, OnWindowCBT, GetModuleHandle(TEXT("AutoWindowDll.dll")), 0);

}

AUTOWINDOWDLL_API void Unstall(void)
{
	//OutputDebugString(L"KinGo: In Unstall");
	if (hhook)
	{
		//OutputDebugString(L"KinGo: In Unstall Run Fun");
		UnhookWindowsHookEx(hhook);
		hhook = nullptr;
	}

}
static int QuiteFlag = 1;
HWND g_hMain = nullptr;
void CloseCondition(HWND hWnd, std::vector<std::wstring> Child)
{
	for (const auto var : Child)
	{
		auto cWnd = FindWindowEx(hWnd, 0, nullptr, var.c_str());
		if (!cWnd)
		{
			return;
		}
	}
	SendMessage(hWnd, WM_CLOSE, 0, 0);
}

void SendClick(HWND hWnd, std::wstring cWindow)
{

	std::this_thread::sleep_for(2s);
	auto cWnd = FindWindowEx(hWnd, 0, nullptr, cWindow.c_str());
	if (!cWnd)
	{
		return;
	}
	if (IsWindowVisible(cWnd) == false)
	{
		return;
	}
	SendMessage(cWnd, WM_LBUTTONDOWN, MK_LBUTTON, 0);
	SendMessage(cWnd, WM_LBUTTONUP, 0, 0);

	TCHAR ch[256];
	GetWindowText(cWnd, ch, 256);
	std::wstring Info{ TEXT("KinWindow: In Send Click : ") };
	Info += ch;
	OutputDebugString(Info.c_str());
}


void DelayClick(POINT pt)
{
	SetCursorPos(pt.x, pt.y);
	std::this_thread::sleep_for(1s);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}


void AutoInvite(HWND hMain)
{
	std::this_thread::sleep_for(5s);

	auto hChild = FindWindowEx(hMain, 0, 0, 0);
	if (hChild)
	{
		hChild = FindWindowEx(hChild, 0, 0, TEXT("CHallPanel"));
		if (hChild)
		{
			hChild = FindWindowEx(hChild, 0, 0, TEXT("CHallActionPanel"));
			POINT pt;
			pt.x = 100;
			pt.y = 30;

			ClientToScreen(hChild, &pt);
			DelayClick(pt);
		}
	}
}


void ClickLeaveRoom()
{
	auto hChild = FindWindowEx(g_hMain, 0, 0, 0);
	if (hChild && QuiteFlag)
	{
		hChild = FindWindowEx(hChild, 0, 0, TEXT("CRoomPanel"));
		if (hChild)
		{
			auto hNeed = FindWindowEx(hChild, 0, 0, 0);
			while (hNeed)
			{
				RECT rt;
				GetWindowRect(hNeed, &rt);
				auto Width = rt.right - rt.left;
				auto Height = rt.bottom - rt.top;

				if ((Width == 350) && (Height == 28))
				{
					std::wstring ai{ TEXT("KinWindow : FindLeaveRoomWindow") };
					ai += std::to_wstring((uint32_t)hNeed);
					OutputDebugString(ai.c_str());

					POINT pt;
					pt.x = 262;
					pt.y = 14;

					ClientToScreen(hNeed, &pt);
					QuiteFlag = 0;
					std::thread(DelayClick, pt).detach();
					std::thread(AutoInvite, g_hMain).detach();
					break;
				}

				hNeed = FindWindowEx(hChild, hNeed, 0, 0);
			}
		}
	}
}
static int CallPlayFlag = 0;

void CallPlay()
{
	if (!g_hMain)
	{
		return;
	}


	for (int i = 0; i != 20; ++i)
	{
		TCHAR Tittle[256];
		GetWindowText(g_hMain, Tittle, 256);
		std::wstring wTittle{ Tittle };

		if (wTittle.find(TEXT("号对弈房 对弈中")) != std::string::npos)
		{
			auto h = FindWindowExA(HWND_MESSAGE, 0, "KinFoxAiGoClass", NULL);
			if (!h)
			{
				MessageBox(NULL, L"UnFind KinFoxAiGoTittle", 0, 0);
			}

			SendMessage(h, WM_USER + 1, 0, 0);
			CallPlayFlag = 0;
			return;
		}

		std::this_thread::sleep_for(1s);
	}


	CallPlayFlag = 0;
	ClickLeaveRoom();
}

AUTOWINDOWDLL_API LRESULT CALLBACK OnWindowCBT(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode == HCBT_ACTIVATE)
	{

		TCHAR Tittle[256];
		HWND h = (HWND)wParam;


		GetWindowText(h, Tittle, 256);
		std::wstring a{ Tittle };
		OutputDebugString((std::wstring(TEXT("KinWindow : Activate : ")) + a + TEXT(" HWND :") + std::to_wstring((uint32_t)h)).c_str());

		if (a.find(TEXT("> [高级房")) != std::string::npos)
		{
			g_hMain = h;
		}
		if (a.compare( std::wstring(L"申请对局")) == 0)
		{
			std::thread(SendClick, h, TEXT("接受")).detach();
			std::thread(SendClick, h, TEXT("申请")).detach();
			if (!CallPlayFlag)
			{
				CallPlayFlag = 1;
				std::thread(CallPlay).detach();
			}

			QuiteFlag = 1;
		}
		else if (a.compare(std::wstring(L"自动点目")) == 0)
		{
			//OutputDebugString(L"KinWindow :发现自动点目");
			//std::thread(SendClick, h, TEXT("OK")).detach();
			//auto hOK = FindWindowEx(h, 0, nullptr, TEXT("OK"));
			POINT pt;
			pt.x = 20;
			pt.y = 210;

			ClientToScreen(h, &pt);
			std::thread(DelayClick, pt).detach();

		}
		if (a.find(TEXT("号对弈房 对局结束"))!= std::string::npos)
		{
			ClickLeaveRoom();
		}
		if (a.empty())
		{
			RECT rt;
			GetWindowRect(h, &rt);
			auto Width = rt.right - rt.left;
			auto Height = rt.bottom - rt.top;
			if ((Width == 350) && (Height == 220))
			{
				//auto confirm = FindWindowEx(h, 0, TEXT("#32770 (对话框)"), TEXT("确定"));
				//std::thread(SendClick, h, TEXT("确定")).detach();
				//std::thread(SendClick, h, TEXT("取消")).detach();

				POINT pt;
				pt.x = 20;
				pt.y = 210;

				ClientToScreen(h, &pt);
				std::thread(DelayClick, pt).detach();
			}
			else if ((Width == 456) && (Height == 364))
			{
				std::vector<std::wstring> Condition;
				Condition.push_back(L"回复");
				Condition.push_back(L"发件人：");
				Condition.push_back(L"申请会员");
				std::thread(CloseCondition, h, Condition).detach();
			}
		}

	}
	//if (nCode == HCBT_CREATEWND)
	//{
	//	TCHAR Tittle[256];
	//	HWND h = (HWND)wParam;
	//	GetWindowText(h, Tittle, 256);
	//	std::wstring a{ Tittle };
	//	OutputDebugString((std::wstring(TEXT("KinWindow : CreateWindow")) + a).c_str());
	//}
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 AutoWindowDll.h
CAutoWindowDll::CAutoWindowDll()
{
    return;
}
