#pragma once
#include "GoGamer.hpp"
#include <KinBase\CallBackFunction.h>
#include <atomic>
namespace KinAiGo
{

	struct GoPoint
	{
		int8_t x;
		int8_t y;
		bool valid(uint8_t max)
		{
			return (x >= 0 && x < max) && (y >= 0 && y < max);
		}
	};

	class GoGamer
	{
	public:
		GoGamer() = default;
		virtual ~GoGamer() = default;

		virtual void Play(int8_t x, int8_t y) =0;
		virtual void GamerPlay() =0;
		virtual void Pass() =0;
		virtual int Start() =0;
		virtual void Stop() = 0;
		virtual int GetColor()
		{
			return 0;
		}
		virtual void Resign()
		{}

	public:
		CallBackFunctions<void(uint8_t, uint8_t)> OnGamerPlay;
		int BoardSize = 19;
		double Komi = 6.5;


	protected:
		std::atomic_char32_t CurrentSteps = 0;
		std::atomic_char32_t RunFlag = 0;
	};
}

