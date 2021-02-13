#pragma once
#include "Core/CoreTypes.h"

class FRandomStream
{
public:
	FRandomStream()
		: InitialSeed(0)
		, Seed(0)
	{ }

	FRandomStream(int32 InSeed)
	{
		Initialize(InSeed);
	}

	void Initialize(int32 InSeed)
	{
		InitialSeed = InSeed;
		Seed = uint32(InSeed);
	}

	FORCEINLINE void Reset() const
	{
		Seed = uint32(InitialSeed);
	}

	FORCEINLINE int32 GetInitialSeed() const
	{
		return InitialSeed;
	}

	FORCEINLINE int32 GetCurrentSeed() const
	{
		return int32(Seed);
	}

	void GenerateNewSeed()
	{
		Initialize(FMath::Rand());
	}

	float FRand() const
	{
		MutateSeed();

		float Result;

		*(uint32*)&Result = 0x3F800000U | (Seed >> 9);

		return Result - 1.0f;
	}


	// returns a number from 0 to A-1
	FORCEINLINE int32 RandHelper(int32 A) const
	{
		// GetFraction guarantees a result in the [0,1) range.
		return ((A > 0) ? FMath::TruncToInt(FRand() * float(A)) : 0);
	}

	// return a random number from min to max
	FORCEINLINE int32 RandRange(int32 Min, int32 Max) const
	{
		const int32 Range = (Max - Min) + 1;

		return Min + RandHelper(Range);
	}

	FORCEINLINE float FRandRange(float InMin, float InMax) const
	{
		return InMin + (InMax - InMin) * FRand();
	}

	uint32 GetUnsignedInt() const
	{
		MutateSeed();

		return Seed;
	}

	FString ToString() const
	{
		return FString::Printf("FRandomStream(InitialSeed=%i, Seed=%u)", InitialSeed, Seed);
	}

private:

	void MutateSeed() const
	{
		Seed = (Seed * 196314165U) + 907633515U;
	}

	int32 InitialSeed;
	mutable uint32 Seed;
};