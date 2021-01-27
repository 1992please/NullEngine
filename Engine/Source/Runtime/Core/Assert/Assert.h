#include "Core/CoreTypes.h"
#include "Core/Logging/Logger.h"
#include "Core/Misc/CString.h"

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