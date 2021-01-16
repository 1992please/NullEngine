#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::EngineLogger;
std::shared_ptr<spdlog::logger> Log::GameLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");

	EngineLogger = spdlog::stdout_color_mt("Engine");
	GameLogger = spdlog::stdout_color_mt("Game");

	EngineLogger->set_level(spdlog::level::trace);
	GameLogger->set_level(spdlog::level::trace);
}