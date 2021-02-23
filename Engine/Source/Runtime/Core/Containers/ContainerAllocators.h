// TODO implement someday

//
//#pragma once
//#include "Core/CoreTypes.h"
//#include "Core/Debug/Assert.h"
//
///** The indirect allocation policy always allocates the elements indirectly. */
//class THeapAllocator
//{
//public:
//	enum { NeedsElementType = false };
//	enum { RequireRangeCheck = true };
//
//	class ForAnyElementType
//	{
//		template <int>
//		friend class TSizedHeapAllocator;
//
//	public:
//		/** Default constructor. */
//		ForAnyElementType()
//			: Data(nullptr)
//		{}
//
//		/**
//		 * Moves the state of another allocator into this one.  The allocator can be different.
//		 *
//		 * Assumes that the allocator is currently empty, i.e. memory may be allocated but any existing elements have already been destructed (if necessary).
//		 * @param Other - The allocator to move the state from.  This allocator should be left in a valid empty state.
//		 */
//		template <typename OtherAllocator>
//		FORCEINLINE void MoveToEmptyFromOtherAllocator(typename OtherAllocator::ForAnyElementType& Other)
//		{
//			NE_CHECK((void*)this != (void*)&Other);
//
//			if (Data)
//			{
//				FMemory::Free(Data);
//			}
//
//			Data = Other.Data;
//			Other.Data = nullptr;
//		}
//
//		/**
//		 * Moves the state of another allocator into this one.
//		 * Moves the state of another allocator into this one.  The allocator can be different.
//		 *
//		 * Assumes that the allocator is currently empty, i.e. memory may be allocated but any existing elements have already been destructed (if necessary).
//		 * @param Other - The allocator to move the state from.  This allocator should be left in a valid empty state.
//		 */
//		FORCEINLINE void MoveToEmpty(ForAnyElementType& Other)
//		{
//			this->MoveToEmptyFromOtherAllocator<TSizedHeapAllocator>(Other);
//		}
//
//		/** Destructor. */
//		FORCEINLINE ~ForAnyElementType()
//		{
//			if(Data)
//			{
//				FMemory::Free(Data);
//			}
//		}
//
//		// FContainerAllocatorInterface
//		FORCEINLINE FScriptContainerElement* GetAllocation() const
//		{
//			return Data;
//		}
//		FORCEINLINE void ResizeAllocation(int32 PreviousNumElements, int32 NumElements, SIZE_T NumBytesPerElement)
//		{
//			// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
//			if (Data || NumElements)
//			{
//				//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
//				Data = (FScriptContainerElement*)FMemory::Realloc( Data, NumElements*NumBytesPerElement );
//			}
//		}
//		FORCEINLINE int32 CalculateSlackReserve(int32 NumElements, size_t NumBytesPerElement) const
//		{
//			return DefaultCalculateSlackReserve(NumElements, NumBytesPerElement, true);
//		}
//		FORCEINLINE int32 CalculateSlackShrink(int32 NumElements, int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			return DefaultCalculateSlackShrink(NumElements, NumAllocatedElements, NumBytesPerElement, true);
//		}
//		FORCEINLINE int32 CalculateSlackGrow(int32 NumElements, int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			return DefaultCalculateSlackGrow(NumElements, NumAllocatedElements, NumBytesPerElement, true);
//		}
//
//		size_t GetAllocatedSize(int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			return NumAllocatedElements * NumBytesPerElement;
//		}
//
//		bool HasAllocation() const
//		{
//			return !!Data;
//		}
//
//		int32 GetInitialCapacity() const
//		{
//			return 0;
//		}
//
//	private:
//		ForAnyElementType(const ForAnyElementType&);
//		ForAnyElementType& operator=(const ForAnyElementType&);
//
//		/** A pointer to the container's elements. */
//		void* Data;
//	};
//	
//	template<typename ElementType>
//	class ForElementType : public ForAnyElementType
//	{
//	public:
//
//		/** Default constructor. */
//		ForElementType()
//		{}
//
//		FORCEINLINE ElementType* GetAllocation() const
//		{
//			return (ElementType*)ForAnyElementType::GetAllocation();
//		}
//	};
//};
//
//template <uint32 NumInlineElements, typename SecondaryAllocator = THeapAllocator>
//class TInlineAllocator
//{
//public:
//	using SizeType = int32;
//
//	enum { NeedsElementType = true };
//	enum { RequireRangeCheck = true };
//
//	template<typename ElementType>
//	class ForElementType
//	{
//	public:
//
//		/** Default constructor. */
//		ForElementType()
//		{
//		}
//
//		/**
//		 * Moves the state of another allocator into this one.
//		 * Assumes that the allocator is currently empty, i.e. memory may be allocated but any existing elements have already been destructed (if necessary).
//		 * @param Other - The allocator to move the state from.  This allocator should be left in a valid empty state.
//		 */
//		FORCEINLINE void MoveToEmpty(ForElementType& Other)
//		{
//			checkSlow(this != &Other);
//
//			if (!Other.SecondaryData.GetAllocation())
//			{
//				// Relocate objects from other inline storage only if it was stored inline in Other
//				RelocateConstructItems<ElementType>((void*)InlineData, Other.GetInlineElements(), NumInlineElements);
//			}
//
//			// Move secondary storage in any case.
//			// This will move secondary storage if it exists but will also handle the case where secondary storage is used in Other but not in *this.
//			SecondaryData.MoveToEmpty(Other.SecondaryData);
//		}
//
//		// FContainerAllocatorInterface
//		FORCEINLINE ElementType* GetAllocation() const
//		{
//			return IfAThenAElseB<ElementType>(SecondaryData.GetAllocation(),GetInlineElements());
//		}
//
//		void ResizeAllocation(int32 PreviousNumElements, int32 NumElements, size_t NumBytesPerElement)
//		{
//			// Check if the new allocation will fit in the inline data area.
//			if(NumElements <= NumInlineElements)
//			{
//				// If the old allocation wasn't in the inline data area, relocate it into the inline data area.
//				if(SecondaryData.GetAllocation())
//				{
//					RelocateConstructItems<ElementType>((void*)InlineData, (ElementType*)SecondaryData.GetAllocation(), PreviousNumElements);
//
//					// Free the old indirect allocation.
//					SecondaryData.ResizeAllocation(0,0,NumBytesPerElement);
//				}
//			}
//			else
//			{
//				if(!SecondaryData.GetAllocation())
//				{
//					// Allocate new indirect memory for the data.
//					SecondaryData.ResizeAllocation(0,NumElements,NumBytesPerElement);
//
//					// Move the data out of the inline data area into the new allocation.
//					RelocateConstructItems<ElementType>((void*)SecondaryData.GetAllocation(), GetInlineElements(), PreviousNumElements);
//				}
//				else
//				{
//					// Reallocate the indirect data for the new size.
//					SecondaryData.ResizeAllocation(PreviousNumElements, NumElements, NumBytesPerElement);
//				}
//			}
//		}
//
//		FORCEINLINE int32 CalculateSlackReserve(int32 NumElements, size_t NumBytesPerElement) const
//		{
//			// If the elements use less space than the inline allocation, only use the inline allocation as slack.
//			return NumElements <= NumInlineElements ?
//				NumInlineElements :
//				SecondaryData.CalculateSlackReserve(NumElements, NumBytesPerElement);
//		}
//		FORCEINLINE int32 CalculateSlackShrink(int32 NumElements, int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			// If the elements use less space than the inline allocation, only use the inline allocation as slack.
//			return NumElements <= NumInlineElements ?
//				NumInlineElements :
//				SecondaryData.CalculateSlackShrink(NumElements, NumAllocatedElements, NumBytesPerElement);
//		}
//		FORCEINLINE int32 CalculateSlackGrow(int32 NumElements, int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			// If the elements use less space than the inline allocation, only use the inline allocation as slack.
//			return NumElements <= NumInlineElements ?
//				NumInlineElements :
//				SecondaryData.CalculateSlackGrow(NumElements, NumAllocatedElements, NumBytesPerElement);
//		}
//
//		size_t GetAllocatedSize(int32 NumAllocatedElements, size_t NumBytesPerElement) const
//		{
//			if (NumAllocatedElements > NumInlineElements)
//			{
//				return SecondaryData.GetAllocatedSize(NumAllocatedElements, NumBytesPerElement);
//			}
//			return 0;
//		}
//
//		bool HasAllocation() const
//		{
//			return SecondaryData.HasAllocation();
//		}
//
//		int32 GetInitialCapacity() const
//		{
//			return NumInlineElements;
//		}
//
//	private:
//		ForElementType(const ForElementType&);
//		ForElementType& operator=(const ForElementType&);
//
//		/** The data is stored in this array if less than NumInlineElements is needed. */
//		TTypeCompatibleBytes<ElementType> InlineData[NumInlineElements];
//
//		/** The data is allocated through the indirect allocation policy if more than NumInlineElements is needed. */
//		typename SecondaryAllocator::template ForElementType<ElementType> SecondaryData;
//
//		/** @return the base of the aligned inline element data */
//		ElementType* GetInlineElements() const
//		{
//			return (ElementType*)InlineData;
//		}
//	};
//
//	typedef void ForAnyElementType;
//};