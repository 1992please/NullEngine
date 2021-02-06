#include "CFile.h"
#include <windows.h>
#include "Core/Assert/Assert.h"
#include "Core/Misc/ProfilingStats.h"

FCFile::FCFile(const char* InFilePath)
{
	NE_ASSERT(InFilePath)

	FileHandle = CreateFileA(InFilePath,                // name of the write
		GENERIC_WRITE | GENERIC_READ,          // open for writing
		FILE_SHARE_READ,                      // SHARE READING
		NULL,                   // default security
		OPEN_ALWAYS,             // open or create file if it doesn't exist
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template
}

void FCFile::WriteData(void* InData, uint32 InDataSize)
{
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD BytesWritten;
		WriteFile(FileHandle, InData, InDataSize, &BytesWritten, NULL);
	}
}

FCFile::~FCFile()
{
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(FileHandle);
	}
}

