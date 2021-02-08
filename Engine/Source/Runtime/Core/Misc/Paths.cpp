#include "NullPCH.h"
#include "Paths.h"

auto IsSlashOrBackslash = [](char C) { return C == '/' || C == '\\'; };
auto IsNotSlashOrBackslash = [](char C) { return C != '/' && C != '\\'; };

FString FPaths::GetExtension(const FString& InPath, bool bIncludeDot)
{
	const FString Filename = GetCleanFilename(InPath);
	int32 DotPos = Filename.Find(".", ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (DotPos != INDEX_NONE)
	{
		return Filename.Mid(DotPos + (bIncludeDot ? 0 : 1));
	}

	return "";
}

FString FPaths::GetCleanFilename(const FString& InPath)
{
	int32 EndPos   = InPath.FindLastCharByPredicate(IsNotSlashOrBackslash) + 1;
	int32 StartPos = InPath.FindLastCharByPredicate(IsSlashOrBackslash) + 1;

	FString Result = (StartPos <= EndPos) ? InPath.Mid(StartPos, EndPos - StartPos) : "";
	return Result;
}

FString FPaths::GetBaseFilename(const FString& InPath, bool bRemovePath)
{
	FString Wk = bRemovePath ? FPaths::GetCleanFilename(InPath) : InPath;

	// remove the extension
	const int32 ExtPos = Wk.Find(".", ESearchCase::CaseSensitive, ESearchDir::FromEnd);

	if (ExtPos != INDEX_NONE)
	{
		// determine the position of the path/leaf separator
		int32 LeafPos = INDEX_NONE;
		if (!bRemovePath)
		{
			LeafPos = Wk.FindLastCharByPredicate(IsSlashOrBackslash);
		}

		if (LeafPos == INDEX_NONE || ExtPos > LeafPos)
		{
			Wk.LeftInline(ExtPos);
		}
	}

	return Wk;
}

FString FPaths::GetPath(const FString& InPath)
{
	int32 Pos = InPath.FindLastCharByPredicate(IsSlashOrBackslash);

	FString Result;
	if (Pos != INDEX_NONE)
	{
		Result = InPath.Left(Pos);
	}

	return Result;
}
