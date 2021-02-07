#include "CFile.h"
#include <windows.h>
#include "Core/Assert/Assert.h"
#include "Core/Misc/ProfilingStats.h"

FCFile::FCFile(const char* InFilePath)
{
	NE_ASSERT(InFilePath)

	FileHandle = CreateFileA(InFilePath,                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // SHARE READING
		NULL,                   // default security
		CREATE_ALWAYS,             // open or create file if it doesn't exist
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template
}

void FCFile::WriteData(const void* InData, uint32 InDataSize)
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

