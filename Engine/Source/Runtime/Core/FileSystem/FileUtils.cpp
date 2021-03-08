#include "NullPCH.h"
#include "FileUtils.h"
#include "Core/Application/Application.h"
#include "Core/Application/ApplicationWindow.h"

bool FFileUtils::OpenFileDialog(const FString& Starting, const char* InFilter, FString& OutFilePath)
{
	char OutFileName[256] = { 0 };

	OPENFILENAMEA OFN;
	FMemory::Memzero(&OFN, sizeof(OFN));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = (HWND)FApplication::GetApplication()->GetWindow()->GetOSWindow();
	//OFN.hInstance;
	OFN.lpstrFilter = InFilter;
	//OFN.lpstrCustomFilter;
	//OFN.nMaxCustFilter;
	OFN.nFilterIndex = 1;
	OFN.lpstrFile = OutFileName;
	OFN.nMaxFile = sizeof(OutFileName);
	OFN.lpstrFileTitle;
	//OFN.nMaxFileTitle;
	OFN.lpstrInitialDir = Starting.GetCharArray().GetData();
	//OFN.lpstrTitle;
	OFN.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//OFN.nFileOffset;
	//OFN.nFileExtension;
	//OFN.lpstrDefExt;
	//OFN.lCustData;
	//OFN.lpfnHook;
	//OFN.lpTemplateName;
	//OFN.lpEditInfo;
	//OFN.lpstrPrompt;
	//OFN.pvReserved;
	//OFN.dwReserved;
	//OFN.FlagsEx;
	if (GetOpenFileNameA(&OFN) != 0)
	{
		OutFilePath = OFN.lpstrFile;
		return true;
	}
	return false;
}

bool FFileUtils::SaveFileDialog(const FString& Starting, const char* InFilter, FString& OutFilePath)
{
	char OutFileName[256] = { 0 };

	OPENFILENAMEA OFN;
	FMemory::Memzero(&OFN, sizeof(OFN));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = (HWND)FApplication::GetApplication()->GetWindow()->GetOSWindow();
	//OFN.hInstance;
	OFN.lpstrFilter = InFilter;
	//OFN.lpstrCustomFilter;
	//OFN.nMaxCustFilter;
	OFN.nFilterIndex = 1;
	OFN.lpstrFile = OutFileName;
	OFN.nMaxFile = sizeof(OutFileName);
	OFN.lpstrFileTitle;
	//OFN.nMaxFileTitle;
	OFN.lpstrInitialDir = Starting.GetCharArray().GetData();
	//OFN.lpstrTitle;
	OFN.Flags = OFN_DONTADDTORECENT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	//OFN.nFileOffset;
	//OFN.nFileExtension;
	OFN.lpstrDefExt = FCString::StrChar(InFilter, '\0') + 1;
	//OFN.lCustData;
	//OFN.lpfnHook;
	//OFN.lpTemplateName;
	//OFN.lpEditInfo;
	//OFN.lpstrPrompt;
	//OFN.pvReserved;
	//OFN.dwReserved;
	//OFN.FlagsEx;
	if (GetSaveFileNameA(&OFN) != 0)
	{
		OutFilePath = OFN.lpstrFile;
		return true;
	}
	return false;
}

FString FFileUtils::GetCurrentDirectory()
{
	char currentDir[256] = { 0 };
	GetCurrentDirectoryA(sizeof(currentDir), currentDir);
	return currentDir;
}
