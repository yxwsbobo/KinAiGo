#pragma once
#include "GTPProgram.hpp"
#include <string>
#include <Windows.h>
#include <sstream>
#include <mutex>
#include <KinBase\CallBackFunction.h>
#include <atomic>
namespace KinAiGo
{

	class GTPProgram
	{
	public:
		GTPProgram(std::string Path, std::string cmd);
		~GTPProgram();

		GTPProgram &operator<<(const std::string info);

		CallBackFunctions<void(std::string)> OnRecieved;

		void AutoRead();
	private:
		void AutoReadPipe();

	private:
		HANDLE Send = nullptr;
		HANDLE Receive = nullptr;
		PROCESS_INFORMATION ProgramInfo;

		std::atomic_char32_t ReadFlag = 0;
		std::atomic_char32_t OverFlag = 1;
		std::string ReadBuffer;
	};
}

