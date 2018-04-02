#pragma once
#include "AutoGo.hpp"
#include <Windows.h>
namespace KinAutoGo
{

	class AutoGo
	{
	public:
		AutoGo();
		~AutoGo();

		static  DWORD WINAPI  Run(void*);
	};
}

