#pragma once
#include "Core/CoreTypes.h"
#include "Core/Containers/Array.h"
#include "Core/Misc/CString.h"
#include "Core/Misc/Crc.h"
#include "Core/Math/NumericLimits.h"
#include "Core/Math/MathUtility.h"
#include "Core/Templates/NullTemplate.h"
#include "Core/Memory/NullMemory.h"
#include "Core/Debug/Assert.h"

class FString
{
public:
	FString() = default;
	FString(FString&&) = default;
	FString(const FString&) = default;
	FString& operator=(FString&&) = default;
	FString& operator=(const FString&) = default;

	FORCEINLINE FString(const FString& Other, int32 ExtraSlack)
		: Data(Other.Data, ExtraSlack + ((Other.Data.Num() || !ExtraSlack) ? 0 : 1)) // Add 1 if the source string array is empty and we want some slack, because we'll need to include a null terminator which is currently missing
	{
	}

	FORCEINLINE FString(FString&& Other, int32 ExtraSlack)
		: Data(MoveTemp(Other.Data), ExtraSlack + ((Other.Data.Num() || !ExtraSlack) ? 0 : 1)) // Add 1 if the source string array is empty and we want some slack, because we'll need to include a null terminator which is currently missing
	{
	}

	FORCEINLINE FString(const char* Src)
	{
		if (Src && *Src)
		{
			int32 SrcLen = FCString::Strlen(Src) + 1;
			Data.Reserve(SrcLen);
			Data.AddUninitialized(SrcLen);
			FMemory::Memcpy(Data.GetData(), Src, SrcLen);
		}
	}

	FORCEINLINE FString(const char* Src, int32 ExtraSlack)
	{
		if (Src && *Src)
		{
			int32 SrcLen = FCString::Strlen(Src) + 1;
			Data.Reserve(SrcLen + ExtraSlack);
			Data.AddUninitialized(SrcLen);

			FMemory::Memcpy(Data.GetData(), Src, SrcLen);
		}
		else
		{
			Reserve(ExtraSlack);
		}
	}

	FORCEINLINE explicit FString(int32 InCount, const char* InSrc)
	{
		if (InSrc)
		{
			if (InCount > 0 && *InSrc)
			{
				Data.Reserve(InCount + 1);
				Data.AddUninitialized(InCount + 1);

				FMemory::Memcpy(Data.GetData(), InSrc, InCount);

				*(Data.GetData() + Data.Num() - 1) = '\0';
			}
		}
	}

	FORCEINLINE FString& operator=(TArray<char>&& Other)
	{
		Data = MoveTemp(Other);
		return *this;
	}

	FORCEINLINE FString& operator=(const char* Other)
	{
		if (Data.GetData() != Other)
		{
			int32 Len = (Other && *Other) ? FCString::Strlen(Other) + 1 : 0;
			Data.Empty(Len);
			Data.AddUninitialized(Len);

			if (Len)
			{
				FMemory::Memcpy(Data.GetData(), Other, Len);
			}
		}
		return *this;
	}

	FORCEINLINE char& operator[](int32 Index)
	{
		return Data[Index];
	}

	FORCEINLINE const char& operator[](int32 Index) const
	{
		return Data[Index];
	}

	FORCEINLINE size_t GetAllocatedSize() const
	{
		return Data.GetAllocatedSize();
	}

	FORCEINLINE void Empty(int32 Slack = 0)
	{
		Data.Empty(Slack);
	}

	FORCEINLINE bool IsEmpty() const
	{
		return Data.Num() <= 1;
	}

	FORCEINLINE void Reset(int32 NewReservedSize = 0)
	{
		const int32 NewSizeIncludingTerminator = (NewReservedSize > 0) ? (NewReservedSize + 1) : 0;
		Data.Reset(NewSizeIncludingTerminator);
		if (char* DataPtr = Data.GetData())
		{
			*DataPtr = '\0';
		}
	}

	FORCEINLINE void Shrink()
	{
		Data.Shrink();
	}

	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < Len();
	}

	/** Get the length of the string, excluding terminating character */
	FORCEINLINE int32 Len() const
	{
		return Data.Num() ? Data.Num() - 1 : 0;
	}

	// To allow more efficient memory handling, automatically adds one for the string termination.
	FORCEINLINE void Reserve(int32 CharacterCount)
	{
		NE_CHECK(CharacterCount >= 0 && CharacterCount < MAX_int32);
		if (CharacterCount > 0)
		{
			Data.Reserve(CharacterCount + 1);
		}
	}

	FORCEINLINE const char* operator*() const
	{
		return Data.Num() ? Data.GetData() : "";
	}

	FORCEINLINE TArray<char>& GetCharArray()
	{
		return Data;
	}

	FORCEINLINE const TArray<char>& GetCharArray() const
	{
		return Data;
	}

	// count string length not including the null pointer
	FORCEINLINE FString& Append(const char* Str, int32 Count)
	{
		NE_CHECK(Str);

		if (Count)
		{
			const int32 OldNum = Data.Num();

			// Reserve enough space - including an extra gap for a null terminator if we don't already have a string allocated
			Data.AddUninitialized(Count + (OldNum ? 0 : 1));

			char* Dest = Data.GetData() + OldNum - (OldNum ? 1 : 0);

			// Copy characters to end of string, overwriting null terminator if we already have one
			FMemory::Memcpy(Dest, Str, Count);

			// (Re-)establish the null terminator
			Dest[Count] = '\0';
		}
		return *this;
	}

	FORCEINLINE FString& Append(const char* Str)
	{
		NE_CHECK(Str);

		int32 Count = FCString::Strlen(Str);
		Append(Str, Count);

		return *this;
	}

	FORCEINLINE FString& AppendChar(char InChar)
	{
		if (InChar != 0)
		{
			// position to insert the character.  
			// At the end of the string if we have existing characters, otherwise at the 0 position
			int32 InsertIndex = (Data.Num() > 0) ? Data.Num() - 1 : 0;

			// number of characters to add.  If we don't have any existing characters, 
			// we'll need to append the terminating zero as well.
			int32 InsertCount = (Data.Num() > 0) ? 1 : 2;

			Data.AddUninitialized(InsertCount);
			Data[InsertIndex] = InChar;
			Data[InsertIndex + 1] = 0;
		}
		return *this;
	}

	FORCEINLINE FString& Append(const FString& InString)
	{
		Append(InString.GetCharArray().GetData(), InString.Len());
		return *this;
	}

	/** appends the integer InNum to this string */
	void AppendInt(int32 InNum);

	static FORCEINLINE FString FromInt(int32 Num)
	{
		FString Ret;
		Ret.AppendInt(Num);
		return Ret;
	}

	template <typename StrType>
	FORCEINLINE FString& operator+=(StrType&& Str) { return Append(Forward<StrType>(Str)); }
	FORCEINLINE FString& operator+=(char Char) { return AppendChar(Char); }

	FORCEINLINE void RemoveAt(int32 Index, int32 Count = 1, bool bAllowShrinking = true)
	{
		Data.RemoveAt(Index, FMath::Clamp(Count, 0, Len() - Index), bAllowShrinking);
	}

	FORCEINLINE void InsertAt(int32 Index, char Character)
	{
		if (Character != 0)
		{
			if (Data.Num() == 0)
			{
				*this += Character;
			}
			else
			{
				Data.Insert(Character, Index);
			}
		}
	}

	FORCEINLINE void InsertAt(int32 Index, const FString& Characters)
	{
		if (Characters.Len())
		{
			if (Data.Num() == 0)
			{
				*this += Characters;
			}
			else
			{
				Data.Insert(Characters.Data.GetData(), Characters.Len(), Index);
			}
		}
	}

	bool RemoveFromStart(const char* InPrefix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase);

	bool RemoveFromStart(const FString& InPrefix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase);

	bool RemoveFromEnd(const char* InSuffix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase);

	bool RemoveFromEnd(const FString& InSuffix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase);

	void PathAppend(const char* Str, int32 StrLength);

	bool StartsWith(const char* InSuffix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	bool StartsWith(const FString& InPrefix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	bool EndsWith(const char* InSuffix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	bool EndsWith(const FString& InSuffix, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase) const;

	int32 Find(const char* SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart, int32 StartPosition = INDEX_NONE) const;

	FORCEINLINE int32 Find(const FString& SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart, int32 StartPosition = INDEX_NONE) const
	{
		return Find(*SubStr, SearchCase, SearchDir, StartPosition);
	}

	FORCEINLINE bool FindChar(char InChar, int32& Index) const
	{
		return Data.Find(InChar, Index);
	}

	template <typename Predicate>
	FORCEINLINE int32 FindLastCharByPredicate(Predicate Pred, int32 Count) const
	{
		check(Count >= 0 && Count <= this->Len());
		return Data.FindLastByPredicate(Pred, Count);
	}

	template <typename Predicate>
	FORCEINLINE int32 FindLastCharByPredicate(Predicate Pred) const
	{
		return Data.FindLastByPredicate(Pred, this->Len());
	}


	/** Returns the substring from Start position for Count characters. */
	FORCEINLINE FString Mid(int32 Start, int32 Count = MAX_int32) const
	{
		FString Result;
		if (Count >= 0)
		{
			const int32 Length = Len();
			const int32 RequestedStart = Start;
			Start = FMath::Clamp(Start, 0, Length);
			const int32 End = (int32)FMath::Clamp((int64)Count + RequestedStart, (int64)Start, (int64)Length);
			Result = FString(End - Start, **this + Start);
		}
		return Result;
	}

	/** Returns the left most given number of characters */
	FORCEINLINE FString Left(int32 Count) const &
	{
		return FString(FMath::Clamp(Count, 0, Len()), **this);
	}

	FORCEINLINE FString Left(int32 Count) &&
	{
		LeftInline(Count, false);
		return MoveTemp(*this);
	}

	/** Modifies the string such that it is now the left most given number of characters */
	FORCEINLINE void LeftInline(int32 Count, bool bAllowShrinking = true)
	{
		const int32 Length = Len();
		Count = FMath::Clamp(Count, 0, Length);
		RemoveAt(Count, Length - Count, bAllowShrinking);
	}

	FORCEINLINE bool Contains(const char* SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart) const
	{
		return Find(SubStr, SearchCase, SearchDir) != INDEX_NONE;
	}

	FORCEINLINE bool Contains(const FString& SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart) const
	{
		return Find(*SubStr, SearchCase, SearchDir) != INDEX_NONE;
	}

	/** Returns a new string with the characters of this converted to uppercase */
	FString ToUpper() const &;

	/**
	 * Converts all characters in this rvalue string to uppercase and moves it into the returned string.
	 * @return a new string with the characters of this converted to uppercase
	 */
	FString ToUpper() && ;

	/** Converts all characters in this string to uppercase */
	void ToUpperInline();

	/** Returns a new string with the characters of this converted to lowercase */
	FString ToLower() const &;

	/**
	 * Converts all characters in this rvalue string to lowercase and moves it into the returned string.
	 * @return a new string with the characters of this converted to lowercase
	 */
	FString ToLower() && ;

	/** Converts all characters in this string to lowercase */
	void ToLowerInline();

	FORCEINLINE bool FString::IsNumeric() const
	{
		if (IsEmpty())
		{
			return 0;
		}

		return FCString::IsNumeric(Data.GetData());
	}

	/** Removes spaces from the string.  I.E. "Spaces Are Cool" --> "SpacesAreCool". */
	void RemoveSpacesInline();

	FORCEINLINE friend bool operator==(const FString& Lhs, const FString& Rhs)
	{
		return Lhs.Equals(Rhs, ESearchCase::IgnoreCase);
	}

	FORCEINLINE friend bool operator!=(const FString& Lhs, const FString& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCEINLINE bool Equals(const FString& Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const
	{
		int32 Num = Data.Num();
		int32 OtherNum = Other.Data.Num();

		if (Num != OtherNum)
		{
			// Handle special case where FString() == FString("")
			return Num + OtherNum == 1;
		}
		else if (Num > 1)
		{
			if (SearchCase == ESearchCase::CaseSensitive)
			{
				return FCString::Strcmp(Data.GetData(), Other.Data.GetData()) == 0;
			}
			else
			{
				return FCString::Stricmp(Data.GetData(), Other.Data.GetData()) == 0;
			}
		}

		return true;
	}

	static FString Printf(const char* Fmt, ...);
	void Appendf(const char* Fmt, ...);

	int32 ParseIntoArrayLines(TArray<FString>& OutArray, bool InCullEmpty = true) const;
	int32 ParseIntoArray(TArray<FString>& OutArray, const char*const* DelimArray, int32 NumDelims, bool InCullEmpty = true) const;

public:
	/**
	 * DO NOT USE DIRECTLY
	 * STL-like iterators to enable range-based for loop support.
	 */
	FORCEINLINE char*       begin()                { return Data.begin(); }
	FORCEINLINE const char* begin() const          { return Data.begin(); }
	FORCEINLINE char*       end  ()                { char* Result = Data.end();   if (Data.Num()) { --Result; }     return Result; }
	FORCEINLINE const char* end  () const          { const char* Result = Data.end();   if (Data.Num()) { --Result; }     return Result; }
private:
	TArray<char> Data;
};

/** Case insensitive string hash function. */
FORCEINLINE uint32 GetTypeHash(const FString& S)
{
	// This must match the GetTypeHash behavior of FStringView
	return FCrc::StrCrc32(*S);
}

