#include "Core/CoreTypes.h"
#include "Core/Debug/Logger.h"
#include "Core/Misc/CString.h"

#if NE_DEBUG
// check the expression and fail if it is false
#define NE_CHECK(expr) \
	if (expr) { } \
	else \
	{ \
		NE_CORE_FATAL("%s\n %s\n %d\n", #expr, __FILE__, __LINE__);\
		__debugbreak(); \
	}

#define NE_CHECK_F(expr, format, ...) \
	if (expr) {} \
	else \
	{ \
		char ErrorString[MAX_SPRINTF]; \
		FCString::Sprintf(ErrorString, format, ##__VA_ARGS__); \
		NE_CORE_FATAL("%s\n %s\n %d\n %s", #expr, __FILE__, __LINE__, ErrorString); \
		NE_DEBUGBREAK(); \
	}
#else
	#define NE_CHECK(expr) // evaluates to nothing
	#define NE_CHECK_F(expr, format, ...) // evaluates to nothing
#endif