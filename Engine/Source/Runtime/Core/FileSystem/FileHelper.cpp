#include "NullPCH.h"
#include "FileHelper.h"

bool FFileHelper::ReadFromTextFile(const FString& FileName, FString& OutFileContent)
{
	bool Result = false;

	NE_CHECK(!FileName.IsEmpty())

	HANDLE FileHandle = CreateFileA(*FileName,				 // file to open
		GENERIC_READ,									 // open for reading
		FILE_SHARE_READ,								 // share for reading
		NULL,											 // default security
		OPEN_EXISTING,									 // existing file only
		0,	 // normal file
		NULL);

	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize))
		{
			NE_CHECK(FileSize.QuadPart <= MAX_int32);
			int32 FileSize32 = int32(FileSize.QuadPart);
			OutFileContent.Reset();
			OutFileContent.GetCharArray().AddUninitialized(FileSize32 + 1);
			DWORD BytesRead;
			if (ReadFile(FileHandle, OutFileContent.GetCharArray().GetData(), FileSize32, &BytesRead, 0) && BytesRead == FileSize32)
			{
				OutFileContent.GetCharArray().Last() = 0;
				Result = true;
			}
		}
		CloseHandle(FileHandle);
	}

	return Result;
}

bool FFileHelper::WriteToTextFile(const FString& FileName, const FString& InFileContent)
{
	bool Result = false;

	NE_CHECK(!InFileContent.IsEmpty() && !FileName.IsEmpty())

	HANDLE FileHandle = CreateFileA(*FileName,                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_NEW,             // create new file only
		0,  // normal file
		NULL);                  // no attr. template

	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD BytesWritten;
		if (WriteFile(FileHandle, *InFileContent, InFileContent.GetCharArray().Num(), &BytesWritten, 0))
		{
			Result = BytesWritten == InFileContent.GetCharArray().Num();
		}
		CloseHandle(FileHandle);
	}

	return Result;
}

