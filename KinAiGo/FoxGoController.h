#pragma once
#include "FoxGoController.hpp"
#include "GoGamer.h"
#include <Windows.h>
#include <atomic>
#include <mutex>
namespace KinAiGo
{
	class FoxBoard
	{
	public:
		FoxBoard();
		~FoxBoard();


		static BOOL CALLBACK MyEnumWindows(HWND   hwnd, LPARAM lParam);
		static void TrueClick(POINT pt);
		void Activite();
		int updataBoardInfo();
		POINT GetPointFormStone(int x, int y);
		HWND hBoardWnd = nullptr;
		void ClickResign();
	private:
		HWND hWnd = nullptr;

		POINT StartPoint;
		double IncreStep;
	};


	class FoxGoController :
		public GoGamer
	{
	public:
		FoxGoController();
		virtual ~FoxGoController();

		virtual void Play(int8_t x, int8_t y) override;
		virtual void GamerPlay() override;
		virtual void Pass() override;
		virtual int Start() override;
		virtual void Stop() override;
		virtual int GetColor() override;
		virtual void Resign() override;
		
	private:
		int updataRoomAddress();
		void CheckGamerPlay();
		int GetNextPlayColor();
		int GetNextStepNumber();
		std::u16string GetWhitePlayerName();
		char* GetStoneList();
		GoPoint GetLastPlayPoint();

	private:
		char* BaseAddress = (char*)0x4fd4c4;
		char* NameAddress = (char*)0x5043A8;
		char* RoomAddress;
		FoxBoard Board;
		std::mutex PlayFlagMutx;
		std::atomic_char32_t PlayeFlag = 0;
		std::atomic_char32_t PlayOver = 0;
	};
}

