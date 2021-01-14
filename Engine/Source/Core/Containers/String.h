#pragma once
#include "Core.h"
#include "Array.h"

class FString
{
public:
	FString() = default;
	FString(FString&&) = default;
	FString(const FString&) = default;
	FString& operator=(FString&&) = default;
	FString& operator=(const FString&) = default;


private:
	TArray<char> Data;
};
