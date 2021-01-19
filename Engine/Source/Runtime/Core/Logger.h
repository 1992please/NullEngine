#pragma once
#include "CoreTypes.h"

class NE_API FLogger
{
public:
	enum LogType
	{
		Trace = 0,
		Log = 2,
		Warn = 3,
		Error = 4,
		Fatal = 5
	};

	enum LogCategory
	{
		Engine,
		Game
	};

	static void Init();
	static void Log_Exec(LogCategory InLogCategory, LogType InLogType, const char* Fmt, ...);
};

#define NE_CORE_LOG(...)       FLogger::Log_Exec(FLogger::Engine, FLogger::Trace, __VA_ARGS__)
#define NE_CORE_WARN(...)      FLogger::Log_Exec(FLogger::Engine, FLogger::Warn, __VA_ARGS__)
#define NE_CORE_ERROR(...)     FLogger::Log_Exec(FLogger::Engine, FLogger::Error, __VA_ARGS__)
#define NE_CORE_FATAL(...)     FLogger::Log_Exec(FLogger::Engine, FLogger::Fatal, __VA_ARGS__)

#define NE_GAME_LOG(...)       FLogger::Log_Exec(FLogger::Game, FLogger::Trace, __VA_ARGS__)
#define NE_GAME_WARN(...)      FLogger::Log_Exec(FLogger::Game, FLogger::Warn, __VA_ARGS__)
#define NE_GAME_ERROR(...)     FLogger::Log_Exec(FLogger::Game, FLogger::Error, __VA_ARGS__)

#ifdef NE_DEBUG
static void reportAssertionFailure(const char* expr, const char* file, int line)
{
	NE_CORE_FATAL("%s\n %s\n %d\n", expr, file, line);
}

// check the expression and fail if it is false
#define ASSERT(expr) \
	if (expr) { } \
	else \
	{ \
	reportAssertionFailure(#expr, \
	__FILE__, __LINE__); \
	__debugbreak(); \
	}
#else
#define ASSERT(expr) // evaluates to nothing
#endif