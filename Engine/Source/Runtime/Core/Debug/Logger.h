#pragma once
#include "Core/CoreTypes.h"

class FLogger
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