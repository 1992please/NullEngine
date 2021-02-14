#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/AppTime.h"
#include "Core/Misc/CFile.h"
#include "Core/Containers/String.h"



#define PROFILER_CONTENT_SIZE		1048576
#define PROFILER_FLUSH_THRESHOLD	1047552

class FProfiler
{
public:

	static void Init()
	{
		NE_ASSERT(!Instance);
		// TODO: replace with memory stack push
		Instance = new FProfiler();
		Instance->WriteHeader();
	}

	static void Destruct()
	{
		NE_ASSERT(Instance);
		Instance->WriteFooter();
		// TODO: replace with memory stack pop
		delete Instance;
	}

	FORCEINLINE void WriteHeader()
	{
		Content += "{\"otherData\": {},\"traceEvents\":[{}";
	}

	FORCEINLINE void WriteProfile(const char* InName, const double InStart, const double InElapsedTime)
	{
		/*",\n{ \"args\":{},\"cat\" : \"Function\",\"dur\":%d,\"name\" : \"%s\",\"ph\" : \"M\",\"pid\" : 0,\"tid\" : 0,\"ts\" : %f }"*/
		Content.Appendf(
			",\n{"
			"\"cat\":\"function\","
			"\"dur\": %.3f,"
			"\"name\":\"%s\","
			"\"ph\":\"X\","
			"\"pid\":0,"
			"\"tid\":0,"
			"\"ts\": %.3f"
			"}"
			, InElapsedTime, InName, InStart);

		if (Content.Len() > PROFILER_FLUSH_THRESHOLD)
		{
			Flush();
		}
	}

	FORCEINLINE void WriteFooter()
	{
		Content += "]}";
		Flush();
	}

	FORCEINLINE void Flush()
	{
		File.WriteData(*Content, Content.Len());
		Content.Reset();
	}

	FORCEINLINE static FProfiler* Get() { return Instance; }
private:
	FProfiler()
		: File("ProfilerOutput.json")
	{
		Content.Reset(PROFILER_CONTENT_SIZE);
	}

	~FProfiler()
	{
		Flush();
	}

private:
	static FProfiler* Instance;
	FCFile File;
	FString Content;
};

class FProfilingTimer
{
public:
	FProfilingTimer(const char* InName)
		: Name(InName)
	{
		StartTime = FAppTime::GetTimeNow();
	}
	~FProfilingTimer()
	{
		const double EndTime = FAppTime::GetTimeNow();
		const double ElapsedTime = EndTime - StartTime;

		FProfiler::Get()->WriteProfile(Name, (StartTime * 1000000), (ElapsedTime * 1000000));
	}
private:
	const char* Name;
	double StartTime;
};


namespace ProfilerUtils {

	template <size_t N, size_t K>
	constexpr char* CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
	{
		char Data[N];

		size_t srcIndex = 0;
		size_t dstIndex = 0;
		while (srcIndex < N)
		{
			size_t matchIndex = 0;
			while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
				matchIndex++;
			if (matchIndex == K - 1)
				srcIndex += matchIndex;
			Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
			srcIndex++;
		}
		return Data;
	}
}

#define NE_PROFILE 0

#if NE_PROFILE
	#define NE_PROFILE_INIT() FProfiler::Init();
	#define NE_PROFILE_FINISH() FProfiler::Destruct();
	#define NE_PROFILE_SCOPE_LINE(name, line) FProfilingTimer ProfilingTimer##line(name);
	#define NE_PROFILE_SCOPE(name) NE_PROFILE_SCOPE_LINE(name, __LINE__)
	#define NE_PROFILE_FUNCTION() NE_PROFILE_SCOPE(__FUNCSIG__)
#else		
	#define NE_PROFILE_INIT()
	#define NE_PROFILE_FINISH()
	#define NE_PROFILE_SCOPE(name)
	#define NE_PROFILE_FUNCTION()
#endif