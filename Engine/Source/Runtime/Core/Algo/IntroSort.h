#pragma once
#include "CoreTypes.h"
#include "Templates/Less.h"
#include "Templates/NullTemplate.h"

namespace AlgoImpl
{
	/**
	 * Implementation of an introspective sort. Starts with quick sort and switches to heap sort when the iteration depth is too big.
	 * The sort is unstable, meaning that the ordering of equal items is not necessarily preserved.
	 * This is the internal sorting function used by IntroSort overrides.
	 *
	 * @param First			pointer to the first element to sort
	 * @param Num			the number of items to sort
	 * @param Projection	The projection to sort by when applied to the element.
	 * @param Predicate		predicate class
	 */
	template <typename T, typename PredicateType>
	void IntroSortInternal(T* First, int32 Num, PredicateType Predicate)
	{
		struct FStack
		{
			T* Min;
			T* Max;
			uint32 MaxDepth;
		};

		if (Num < 2)
		{
			return;
		}

		FStack RecursionStack[32] = { {First, First + Num - 1, (uint32)(FMath::Loge((float)Num) * 2.f)} }, Current, Inner;
		for (FStack* StackTop = RecursionStack; StackTop >= RecursionStack; --StackTop) //-V625
		{
			Current = *StackTop;

		Loop:
			int32 Count = (int32)(Current.Max - Current.Min + 1);

			if (Current.MaxDepth == 0)
			{
				// We're too deep into quick sort, switch to heap sort
				HeapSortInternal(Current.Min, Count, Predicate);
				continue;
			}

			if (Count <= 8)
			{
				// Use simple bubble-sort.
				while (Current.Max > Current.Min)
				{
					T *Max, *Item;
					for (Max = Current.Min, Item = Current.Min + 1; Item <= Current.Max; Item++)
					{
						if (Predicate(*Max, *Item))
						{
							Max = Item;
						}
					}
					Swap(*Max, *Current.Max--);
				}
			}
			else
			{
				// Grab middle element so sort doesn't exhibit worst-cast behavior with presorted lists.
				Swap(Current.Min[Count / 2], Current.Min[0]);

				// Divide list into two halves, one with items <=Current.Min, the other with items >Current.Max.
				Inner.Min = Current.Min;
				Inner.Max = Current.Max + 1;
				for (; ; )
				{
					while (++Inner.Min <= Current.Max && !Predicate(*Current.Min, *Inner.Min));
					while (--Inner.Max > Current.Min && !Predicate(*Inner.Max, *Current.Min));
					if (Inner.Min > Inner.Max)
					{
						break;
					}
					Swap(*Inner.Min, *Inner.Max);
				}
				Swap(*Current.Min, *Inner.Max);

				--Current.MaxDepth;

				// Save big half and recurse with small half.
				if (Inner.Max - 1 - Current.Min >= Current.Max - Inner.Min)
				{
					if (Current.Min + 1 < Inner.Max)
					{
						StackTop->Min = Current.Min;
						StackTop->Max = Inner.Max - 1;
						StackTop->MaxDepth = Current.MaxDepth;
						StackTop++;
					}
					if (Current.Max > Inner.Min)
					{
						Current.Min = Inner.Min;
						goto Loop;
					}
				}
				else
				{
					if (Current.Max > Inner.Min)
					{
						StackTop->Min = Inner.Min;
						StackTop->Max = Current.Max;
						StackTop->MaxDepth = Current.MaxDepth;
						StackTop++;
					}
					if (Current.Min + 1 < Inner.Max)
					{
						Current.Max = Inner.Max - 1;
						goto Loop;
					}
				}
			}
		}
	}
}

namespace Algo
{
	/**
	 * Performs heap sort on the elements. Assumes < operator is defined for the element type.
	 *
	 * @param Range		The range to sort.
	 */
	template <typename RangeType>
	FORCEINLINE void IntroSort(RangeType&& Range)
	{
		AlgoImpl::IntroSortInternal(GetData(Range), GetNum(Range), TLess<>());
	}

	/**
	 * Performs heap sort on the elements.
	 *
	 * @param Range		The range to sort.
	 * @param Predicate	A binary predicate object used to specify if one element should precede another.
	 */
	template <typename ElementType, typename PredicateType>
	FORCEINLINE void IntroSort(ElementType* Array, int32 Num, PredicateType Predicate)
	{
		AlgoImpl::IntroSortInternal(Array, Num, MoveTemp(Predicate));
	}

}