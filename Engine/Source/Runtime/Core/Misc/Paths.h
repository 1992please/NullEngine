#pragma once
#include "Core/CoreTypes.h"
#include "Core/Containers/String.h"

class FPaths
{
public:
	static FString GetExtension( const FString& InPath, bool bIncludeDot=false );

	// Returns the filename (with extension), minus any path information.
	static FString GetCleanFilename(const FString& InPath);

    // Returns the same thing as GetCleanFilename, but without the extension
	static FString GetBaseFilename(const FString& InPath, bool bRemovePath=true );

    // Returns the path in front of the filename
	static FString GetPath(const FString& InPath);
};