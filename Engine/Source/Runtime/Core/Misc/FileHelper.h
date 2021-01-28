#pragma once

#include "Core/CoreTypes.h"
#include "Core/Containers/String.h"

class FFileHelper
{
public:
	static bool ReadFromTextFile(const FString& FileName, FString& OutFileContent);

	static bool WriteToTextFile(const FString& FileName, const FString& InFileContent);
};
