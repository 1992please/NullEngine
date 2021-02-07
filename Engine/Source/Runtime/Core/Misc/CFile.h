#pragma once
#include "Core/CoreTypes.h"

class FCFile
{
public:
    FCFile(const char* InFilePath);
    void WriteData(const void* InData, uint32 InDataSize);
    ~FCFile();
private:
	void* FileHandle;
};