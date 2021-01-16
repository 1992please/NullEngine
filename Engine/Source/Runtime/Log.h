#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"

class NE_API Log
{
public:

	static void Init();
	static std::shared_ptr<spdlog::logger> EngineLogger;
	static std::shared_ptr<spdlog::logger> GameLogger;
};

#define NE_CORE_TRACE(...)     Log::EngineLogger->trace(__VA_ARGS__)
#define NE_CORE_LOG(...)       Log::EngineLogger->info(__VA_ARGS__)
#define NE_CORE_WARN(...)      Log::EngineLogger->warn(__VA_ARGS__)
#define NE_CORE_ERROR(...)     Log::EngineLogger->error(__VA_ARGS__)

#define NE_TRACE(...)          Log::GameLogger->trace(__VA_ARGS__)
#define NE_LOG(...)            Log::GameLogger->info(__VA_ARGS__)
#define NE_WARN(...)           Log::GameLogger->warn(__VA_ARGS__)
#define NE_ERROR(...)          Log::GameLogger->error(__VA_ARGS__)
