#pragma once
template<typename InElementType>
class TSet
{
	/** Initialization constructor. */
	FORCEINLINE TSet()
		: HashSize(0)
	{}



	mutable int32	 HashSize;
};