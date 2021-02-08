#pragma once
#include "Core/CoreTypes.h"

class FCFile
{
public:
	FORCEINLINE FCFile(const char* InFilePath)
	{
		FileHandle = CreateFileA(InFilePath,                // name of the write
			GENERIC_WRITE,          // open for writing
			0,                      // SHARE READING
			NULL,                   // default security
			CREATE_ALWAYS,             // open or create file if it doesn't exist
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template
	}

	FORCEINLINE void WriteData(const void* InData, uint32 InDataSize)
	{
		if (FileHandle != INVALID_HANDLE_VALUE)
		{
			DWORD BytesWritten;
			WriteFile(FileHandle, InData, InDataSize, &BytesWritten, NULL);
		}
	}

	FORCEINLINE ~FCFile()
	{
		if (FileHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(FileHandle);
		}
	}
private:
	void* FileHandle;
};