#include "Logger.h"
#include "Misc/CString.h"
#include "Memory/NullMemory.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Templates/NullTemplate.h"

static char	MsgLogfStaticBuffer[4096];

static std::shared_ptr<spdlog::logger> EngineLogger;
static std::shared_ptr<spdlog::logger> GameLogger;

void FLogger::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");

	EngineLogger = spdlog::stdout_color_mt("Engine");
	GameLogger = spdlog::stdout_color_mt("Game");

	EngineLogger->set_level(spdlog::level::trace);
	GameLogger->set_level(spdlog::level::trace);
}

// This starting size catches 99.97% of printf calls - there are about 700k printf calls per level
#define STARTING_BUFFER_SIZE		512

void FLogger::Log_Exec(LogCategory InLogCategory, LogType InLogType, const char* Fmt, ...)
{
	const int32 BufferSize = UE_ARRAY_COUNT(MsgLogfStaticBuffer);
	char* Buffer = MsgLogfStaticBuffer;
	int32 Result = -1;

	// First try to print to a stack allocated location 
	GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);

	// If that fails, start allocating regular memory
	if (Result == -1)
	{
		EngineLogger->log(spdlog::level::level_enum::critical, "Log exceeded the buffer size");
		return;
	}

	Buffer[Result] = 0;

	switch (InLogCategory)
	{
		case Engine:
		{
			EngineLogger->log((spdlog::level::level_enum) InLogType, Buffer);
		}
		break;
		case Game:
		{
			GameLogger->log((spdlog::level::level_enum)InLogType, Buffer);
		}
		break;
	}

}
