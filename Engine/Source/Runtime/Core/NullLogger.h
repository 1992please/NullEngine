#pragma once
#include "Core.h"

class NE_API NullLogger
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

#define NE_CORE_LOG(...)       NullLogger::Log_Exec(NullLogger::Engine, NullLogger::Trace, __VA_ARGS__)
#define NE_CORE_WARN(...)      NullLogger::Log_Exec(NullLogger::Engine, NullLogger::Warn, __VA_ARGS__)
#define NE_CORE_ERROR(...)     NullLogger::Log_Exec(NullLogger::Engine, NullLogger::Error, __VA_ARGS__)
#define NE_CORE_FATAL(...)     NullLogger::Log_Exec(NullLogger::Engine, NullLogger::Fatal, __VA_ARGS__)

#define NE_GAME_LOG(...)       NullLogger::Log_Exec(NullLogger::Game, NullLogger::Trace, __VA_ARGS__)
#define NE_GAME_WARN(...)      NullLogger::Log_Exec(NullLogger::Game, NullLogger::Warn, __VA_ARGS__)
#define NE_GAME_ERROR(...)     NullLogger::Log_Exec(NullLogger::Game, NullLogger::Error, __VA_ARGS__)

#if _DEBUG
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