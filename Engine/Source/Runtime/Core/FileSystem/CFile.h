#pragma once
#include "Core/CoreTypes.h"

class FCFile
{
public:
	FORCEINLINE FCFile(const char* InFilePath, bool ForWriting = true)
	{
		if (ForWriting)
		{
			FileHandle = CreateFileA(InFilePath,                // name of the write
				GENERIC_WRITE,          // open for writing
				0,                      // SHARE READING
				NULL,                   // default security
				CREATE_ALWAYS,             // open or create file if it doesn't exist
				FILE_ATTRIBUTE_NORMAL,  // normal file
				NULL);                  // no attr. template
		}
		else
		{
			FileHandle = CreateFileA(InFilePath,                // name of the write
				GENERIC_READ,          // open for Read
				0,                      // SHARE READING
				NULL,                   // default security
				OPEN_EXISTING,             // Open Only if file exists
				FILE_ATTRIBUTE_NORMAL,  // normal file
				NULL);                  // no attr. template
		}
	}

	FORCEINLINE bool WriteData(const void* InData, uint32 InDataSize)
	{
		if (IsValid())
		{
			DWORD BytesWritten;
			if (WriteFile(FileHandle, InData, InDataSize, &BytesWritten, NULL))
			{
				return true;
			}
		}
		return false;
	}

	FORCEINLINE bool WriteData(const TArray<uint8>& InData)
	{
		if (IsValid())
		{
			DWORD BytesWritten;
			if (WriteFile(FileHandle, InData.GetData(), InData.Num(), &BytesWritten, NULL) && BytesWritten == InData.Num())
			{
				return true;
			}
		}
		return false;
	}

	FORCEINLINE bool ReadData(TArray<uint8>& OutData)
	{
		if (IsValid())
		{
			LARGE_INTEGER FileSize;
			if (GetFileSizeEx(FileHandle, &FileSize))
			{
				NE_CHECK(FileSize.QuadPart <= MAX_int32);
				int32 FileSize32 = int32(FileSize.QuadPart);
				OutData.Reset();
				OutData.AddUninitialized(FileSize32);
				DWORD BytesRead;
				if (ReadFile(FileHandle, OutData.GetData(), FileSize32, &BytesRead, 0) && BytesRead == FileSize32)
				{
					return true;
				}
			}
		}
		return false;
	}

	FORCEINLINE bool IsValid() const
	{
		return (FileHandle != INVALID_HANDLE_VALUE);
	}

	FORCEINLINE ~FCFile()
	{
		if (IsValid())
		{
			CloseHandle(FileHandle);
		}
	}
private:
	void* FileHandle;
};