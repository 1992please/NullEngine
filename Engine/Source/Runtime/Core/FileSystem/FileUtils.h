#pragma once
#include "Core/CoreTypes.h"

class FFileUtils
{
public:
    static bool OpenFileDialog(const FString& Starting, const char* InFilter, FString& OutFilePath);

    static bool SaveFileDialog(const FString& Starting, const char* InFilter, FString& OutFilePath);

	static FString GetCurrentDirectory();

};