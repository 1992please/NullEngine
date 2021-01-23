#pragma once
#include "Core/CoreTypes.h"

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
#if !NE_SHIPPING
	#define NE_CORE_LOG(...)       FLogger::Log_Exec(FLogger::Engine, FLogger::Trace, __VA_ARGS__);
	#define NE_CORE_WARN(...)      FLogger::Log_Exec(FLogger::Engine, FLogger::Warn, __VA_ARGS__);
	#define NE_CORE_ERROR(...)     FLogger::Log_Exec(FLogger::Engine, FLogger::Error, __VA_ARGS__);
	#define NE_CORE_FATAL(...)     FLogger::Log_Exec(FLogger::Engine, FLogger::Fatal, __VA_ARGS__);

	#define NE_LOG(...)       FLogger::Log_Exec(FLogger::Game, FLogger::Trace, __VA_ARGS__);
	#define NE_WARN(...)      FLogger::Log_Exec(FLogger::Game, FLogger::Warn, __VA_ARGS__);
	#define NE_ERROR(...)     FLogger::Log_Exec(FLogger::Game, FLogger::Error, __VA_ARGS__);
#else
	#define NE_CORE_LOG(...)
	#define NE_CORE_WARN(...) 
	#define NE_CORE_ERROR(...)
	#define NE_CORE_FATAL(...)
	
	#define NE_LOG(...)  
	#define NE_LOG(...) 
	#define NE_LOG(...)
#endif

#if NE_DEBUG
// check the expression and fail if it is false
#define NE_ASSERT(expr) \
	if (expr) { } \
	else \
	{ \
		NE_CORE_FATAL("%s\n %s\n %d\n", #expr, __FILE__, __LINE__);\
		__debugbreak(); \
	}

#define NE_ASSERT_F(expr, format, ...) \
	if (expr) {} \
	else \
	{ \
		char ErrorString[MAX_SPRINTF]; \
		FCString::Sprintf(ErrorString, format, ##__VA_ARGS__); \
		NE_CORE_FATAL("%s\n %s\n %d\n %s", #expr, __FILE__, __LINE__, ErrorString); \
		NE_DEBUGBREAK(); \
	}
#else
	#define NE_ASSERT(expr) // evaluates to nothing
	#define NE_ASSERT_F(expr, format, ...) // evaluates to nothing
#endif