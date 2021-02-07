#include "String.h"

void FString::AppendInt(int32 InNum)
{
	const char* DigitToChar = "9876543210123456789";
	constexpr int32 ZeroDigitIndex = 9;
	bool bIsNumberNegative = InNum < 0;
	const int32 TempBufferSize = 16; // 16 is big enough
	char TempNum[TempBufferSize];
	int32 TempAt = TempBufferSize; // fill the temp string from the top down.

	// Convert to string assuming base ten.
	do
	{
		TempNum[--TempAt] = DigitToChar[ZeroDigitIndex + (InNum % 10)];
		InNum /= 10;
	} while (InNum);

	if (bIsNumberNegative)
	{
		TempNum[--TempAt] = '-';
	}

	const char* CharPtr = TempNum + TempAt;
	const int32 NumChars = TempBufferSize - TempAt;
	Append(CharPtr, NumChars);
}

bool FString::RemoveFromStart(const char* InPrefix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/)
{
	if (*InPrefix == 0)
	{
		return false;
	}

	if (StartsWith(InPrefix, SearchCase))
	{
		RemoveAt(0, FCString::Strlen(InPrefix));
		return true;
	}

	return false;
}

bool FString::RemoveFromStart(const FString& InPrefix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/)
{
	if (InPrefix.IsEmpty())
	{
		return false;
	}

	if (StartsWith(InPrefix, SearchCase))
	{
		RemoveAt(0, InPrefix.Len());
		return true;
	}

	return false;
}

bool FString::RemoveFromEnd(const char* InSuffix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/)
{
	if (*InSuffix == 0)
	{
		return false;
	}

	if (EndsWith(InSuffix, SearchCase))
	{
		int32 SuffixLen = FCString::Strlen(InSuffix);
		RemoveAt(Len() - SuffixLen, SuffixLen);
		return true;
	}

	return false;
}

bool FString::RemoveFromEnd(const FString& InSuffix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/)
{
	if (InSuffix.IsEmpty())
	{
		return false;
	}

	if (EndsWith(InSuffix, SearchCase))
	{
		RemoveAt(Len() - InSuffix.Len(), InSuffix.Len());
		return true;
	}

	return false;
}

void FString::PathAppend(const char* Str, int32 StrLength)
{
	int32 DataNum = Data.Num();
	if (StrLength == 0)
	{
		if (DataNum > 1 && Data[DataNum - 2] != '/' && Data[DataNum - 2] != '\\')
		{
			Data[DataNum - 1] = '/';
			Data.Add('\0');
		}
	}
	else
	{
		if (DataNum > 0)
		{
			if (DataNum > 1 && Data[DataNum - 2] != '/' && Data[DataNum - 2] != '\\' && *Str != '/')
			{
				Data[DataNum - 1] = '/';
			}
			else
			{
				Data.Pop(false);
				--DataNum;
			}
		}

		Reserve(DataNum + StrLength);
		Data.Append(Str, StrLength);
		Data.Add('\0');
	}
}

bool FString::StartsWith(const char* InPrefix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/) const
{
	if (SearchCase == ESearchCase::IgnoreCase)
	{
		return InPrefix && *InPrefix && !FCString::Strnicmp(**this, InPrefix, FCString::Strlen(InPrefix));
	}
	else
	{
		return InPrefix && *InPrefix && !FCString::Strncmp(**this, InPrefix, FCString::Strlen(InPrefix));
	}
}

bool FString::StartsWith(const FString& InPrefix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/) const
{
	if (SearchCase == ESearchCase::IgnoreCase)
	{
		return InPrefix.Len() > 0 && !FCString::Strnicmp(**this, *InPrefix, InPrefix.Len());
	}
	else
	{
		return InPrefix.Len() > 0 && !FCString::Strncmp(**this, *InPrefix, InPrefix.Len());
	}
}

bool FString::EndsWith(const char* InSuffix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/) const
{
	if (!InSuffix || *InSuffix == '\0')
	{
		return false;
	}

	int32 ThisLen = this->Len();
	int32 SuffixLen = FCString::Strlen(InSuffix);
	if (SuffixLen > ThisLen)
	{
		return false;
	}

	const char* StrPtr = Data.GetData() + ThisLen - SuffixLen;
	if (SearchCase == ESearchCase::IgnoreCase)
	{
		return !FCString::Stricmp(StrPtr, InSuffix);
	}
	else
	{
		return !FCString::Strcmp(StrPtr, InSuffix);
	}
}

bool FString::EndsWith(const FString& InSuffix, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/) const
{
	if (SearchCase == ESearchCase::IgnoreCase)
	{
		return InSuffix.Len() > 0 &&
			Len() >= InSuffix.Len() &&
			!FCString::Stricmp(&(*this)[Len() - InSuffix.Len()], *InSuffix);
	}
	else
	{
		return InSuffix.Len() > 0 &&
			Len() >= InSuffix.Len() &&
			!FCString::Strcmp(&(*this)[Len() - InSuffix.Len()], *InSuffix);
	}
}

int32 FString::Find(const char* SubStr, ESearchCase::Type SearchCase /*= ESearchCase::IgnoreCase*/, ESearchDir::Type SearchDir /*= ESearchDir::FromStart*/, int32 StartPosition /*= INDEX_NONE*/) const
{
	if (SubStr == nullptr)
	{
		return INDEX_NONE;
	}

	if (SearchDir == ESearchDir::FromStart)
	{
		const char* Start = **this;
		if (StartPosition != INDEX_NONE)
		{
			Start += FMath::Clamp(StartPosition, 0, Len() - 1);
		}
		const char* Tmp = SearchCase == ESearchCase::IgnoreCase
			? FCString::Stristr(Start, SubStr)
			: FCString::Strstr(Start, SubStr);

		return Tmp ? int32(Tmp - **this) : INDEX_NONE;
	}
	else
	{
		// if ignoring, do a onetime ToUpper on both strings, to avoid ToUppering multiple
		// times in the loop below
		if (SearchCase == ESearchCase::IgnoreCase)
		{
			return ToUpper().Find(FString(SubStr).ToUpper(), ESearchCase::CaseSensitive, SearchDir, StartPosition);
		}
		else
		{
			const int32 SearchStringLength = FMath::Max(1, FCString::Strlen(SubStr));

			if (StartPosition == INDEX_NONE || StartPosition >= Len())
			{
				StartPosition = Len();
			}

			for (int32 i = StartPosition - SearchStringLength; i >= 0; i--)
			{
				int32 j;
				for (j = 0; SubStr[j]; j++)
				{
					if ((*this)[i + j] != SubStr[j])
					{
						break;
					}
				}

				if (!SubStr[j])
				{
					return i;
				}
			}
			return INDEX_NONE;
		}
	}
}

FString FString::ToUpper() const &
{
	FString New = *this;
	New.ToUpperInline();
	return New;
}

FString FString::ToUpper() &&
{
	this->ToUpperInline();
	return MoveTemp(*this);
}

void FString::ToUpperInline()
{
	const int32 StringLength = Len();
	char* RawData = Data.GetData();
	for (int32 i = 0; i < StringLength; ++i)
	{
		RawData[i] = FCString::ToUpper(RawData[i]);
	}
}

FString FString::ToLower() const &
{
	FString New = *this;
	New.ToLowerInline();
	return New;
}

FString FString::ToLower() &&
{
	this->ToLowerInline();
	return MoveTemp(*this);
}

void FString::ToLowerInline()
{
	const int32 StringLength = Len();
	char* RawData = Data.GetData();
	for (int32 i = 0; i < StringLength; ++i)
	{
		RawData[i] = FCString::ToLower(RawData[i]);
	}
}

void FString::RemoveSpacesInline()
{
	const int32 StringLength = Len();
	if (StringLength == 0)
	{
		return;
	}

	char* RawData = Data.GetData();
	int32 CopyToIndex = 0;
	for (int32 CopyFromIndex = 0; CopyFromIndex < StringLength; ++CopyFromIndex)
	{
		if (RawData[CopyFromIndex] != ' ')
		{	// Copy any character OTHER than space.
			RawData[CopyToIndex] = RawData[CopyFromIndex];
			++CopyToIndex;
		}
	}

	// Copy null-terminating character.
	if (CopyToIndex <= StringLength)
	{
		RawData[CopyToIndex] = '\0';
		Data.SetNum(CopyToIndex + 1, false);
	}
}

// This starting size catches 99.97% of printf calls - there are about 700k printf calls per level
#define STARTING_BUFFER_SIZE		512

FString FString::Printf(const char* Fmt, ...)
{
	int32	BufferSize = STARTING_BUFFER_SIZE;
	char	StartingBuffer[STARTING_BUFFER_SIZE];
	char*	Buffer = StartingBuffer;
	int32	Result = -1;

	// First try to print to a stack allocated location 
	GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);

	// If that fails, start allocating regular memory
	if (Result == -1)
	{
		Buffer = nullptr;
		while (Result == -1)
		{
			BufferSize *= 2;
			Buffer = (char*)FMemory::Realloc(Buffer, BufferSize );
			GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);
		};
	}

	Buffer[Result] = 0;

	FString ResultString(Buffer);

	if (BufferSize != STARTING_BUFFER_SIZE)
	{
		FMemory::Free(Buffer);
	}

	return ResultString;
}

void FString::Appendf(const char* Fmt, ...)
{
	int32	BufferSize = STARTING_BUFFER_SIZE;
	char	StartingBuffer[STARTING_BUFFER_SIZE];
	char*	Buffer = StartingBuffer;
	int32	Result = -1;

	// First try to print to a stack allocated location 
	GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);

	// If that fails, start allocating regular memory
	if (Result == -1)
	{
		Buffer = nullptr;
		while (Result == -1)
		{
			BufferSize *= 2;
			Buffer = (char*)FMemory::Realloc(Buffer, BufferSize);
			GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize - 1, Fmt, Fmt, Result);
		};
	}

	Buffer[Result] = 0;

	*this += Buffer;

	if (BufferSize != STARTING_BUFFER_SIZE)
	{
		FMemory::Free(Buffer);
	}
}

int32 FString::ParseIntoArrayLines(TArray<FString>& OutArray, bool InCullEmpty /*= true*/) const
{
	// default array of LineEndings
	static const char* LineEndings[] =
	{
		"\r\n",
		"\r",
		"\n",
	};

	int32 NumLineEndings = NE_ARRAY_COUNT(LineEndings);
	return ParseIntoArray(OutArray, LineEndings, NumLineEndings, InCullEmpty);
}

int32 FString::ParseIntoArray(TArray<FString>& OutArray, const char*const* DelimArray, int32 NumDelims, bool InCullEmpty /*= true*/) const
{
	// Make sure the delimit string is not null or empty
	NE_ASSERT(DelimArray);
	OutArray.Reset();
	const char *Start = Data.GetData();
	const int32 Length = Len();
	if (Start)
	{
		int32 SubstringBeginIndex = 0;

		// Iterate through string.
		for (int32 i = 0; i < Len();)
		{
			int32 SubstringEndIndex = INDEX_NONE;
			int32 DelimiterLength = 0;

			// Attempt each delimiter.
			for (int32 DelimIndex = 0; DelimIndex < NumDelims; ++DelimIndex)
			{
				DelimiterLength = FCString::Strlen(DelimArray[DelimIndex]);

				// If we found a delimiter...
				if (FCString::Strncmp(Start + i, DelimArray[DelimIndex], DelimiterLength) == 0)
				{
					// Mark the end of the substring.
					SubstringEndIndex = i;
					break;
				}
			}

			if (SubstringEndIndex != INDEX_NONE)
			{
				const int32 SubstringLength = SubstringEndIndex - SubstringBeginIndex;
				// If we're not culling empty strings or if we are but the string isn't empty anyways...
				if (!InCullEmpty || SubstringLength != 0)
				{
					// ... add new string from substring beginning up to the beginning of this delimiter.
					OutArray.Add(FString(SubstringEndIndex - SubstringBeginIndex, Start + SubstringBeginIndex));
				}
				// Next substring begins at the end of the discovered delimiter.
				SubstringBeginIndex = SubstringEndIndex + DelimiterLength;
				i = SubstringBeginIndex;
			}
			else
			{
				++i;
			}
		}

		// Add any remaining characters after the last delimiter.
		const int32 SubstringLength = Length - SubstringBeginIndex;
		// If we're not culling empty strings or if we are but the string isn't empty anyways...
		if (!InCullEmpty || SubstringLength != 0)
		{
			// ... add new string from substring beginning up to the beginning of this delimiter.
			OutArray.Add(FString(Start + SubstringBeginIndex));
		}
	}

	return OutArray.Num();
}

