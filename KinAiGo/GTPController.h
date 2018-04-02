#pragma once
#include "GTPController.hpp"
#include <string>
#include <memory>
#include "GTPProgram.hpp"
#include "GoGamer.h"
namespace KinAiGo
{

	class GTPController : public GoGamer
	{
	public:
		GTPController(std::string Path, std::string cmd);
		~GTPController();

		virtual void Play(int8_t x, int8_t y) override;
		virtual void GamerPlay() override;
		virtual void Pass() override;
		virtual int Start() override;
		virtual void Stop() override;

		std::shared_ptr<GTPProgram> GetEngine();

	private:
		void InitBoard();

	private:
		void EngineInfo(std::string Info);
		std::string KinAiGo::GTPController::PositionToString(const GoPoint Pos);
		KinAiGo::GoPoint KinAiGo::GTPController::StringToPosition(std::string Pos);


	private:
		std::shared_ptr<GTPProgram> Engine;
		std::string WinRate;
		
	};
}

