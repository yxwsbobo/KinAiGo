#pragma once
#include "GameController.hpp"
#include "GoGamer.hpp"
#include <memory>
#include <atomic>
namespace KinAiGo
{

	class GameController
	{
	public:
		GameController(std::shared_ptr<GoGamer> p1, std::shared_ptr<GoGamer> p2);
		~GameController();

		void Run(int Player = 1, double Komi = 6.5);
		void pause();
		void Stop();

		void ResetP1(std::shared_ptr<GoGamer> Player);

		std::shared_ptr<GoGamer> Player1;
		std::shared_ptr<GoGamer> Player2;
	private:
		void Player1Played(uint8_t x, uint8_t y);
		void Player2Played(uint8_t x, uint8_t y);
	private:


		std::atomic_char32_t RunFlag = 0;
	};
}

