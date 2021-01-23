#pragma once
#include "Core/CoreTypes.h"
#include "Core/Memory/NullMemory.h"
#include "EnableIf.h"
#include "TypeTraits.h"
#include "IsTriviallyDestructible.h"
#include "AreTypesEqual.h"
#include "AndOrNot.h"
#include <new>

/**
 * Default constructs a range of items in memory.
 *
 * @param	Elements	The address of the first memory location to construct at.
 * @param	Count		The number of elements to destruct.
 */
template <typename ElementType, typename SizeType>
FORCEINLINE typename TEnableIf<!TIsZeroConstructType<ElementType>::Value>::Type DefaultConstructItems(void* Address, SizeType Count)
{
	ElementType* Element = (ElementType*)Address;
	while (Count)
	{
		new (Element) ElementType;
		++Element;
		--Count;
	}
}


template <typename ElementType, typename SizeType>
FORCEINLINE typename TEnableIf<TIsZeroConstructType<ElementType>::Value>::Type DefaultConstructItems(void* Elements, SizeType Count)
{
	FMemory::Memset(Elements, 0, sizeof(ElementType) * Count);
}


/**
 * Destructs a single item in memory.
 *
 * @param	Elements	A pointer to the item to destruct.
 *
 * @note: This function is optimized for values of T, and so will not dynamically dispatch destructor calls if T's destructor is virtual.
 */
template <typename ElementType>
FORCEINLINE typename TEnableIf<!TIsTriviallyDestructible<ElementType>::Value>::Type DestructItem(ElementType* Element)
{
	// We need a typedef here because VC won't compile the destructor call below if ElementType itself has a member called ElementType
	typedef ElementType DestructItemsElementTypeTypedef;

	Element->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
}


template <typename ElementType>
FORCEINLINE typename TEnableIf<TIsTriviallyDestructible<ElementType>::Value>::Type DestructItem(ElementType* Element)
{
}


/**
 * Destructs a range of items in memory.
 *
 * @param	Elements	A pointer to the first item to destruct.
 * @param	Count		The number of elements to destruct.
 *
 * @note: This function is optimized for values of T, and so will not dynamically dispatch destructor calls if T's destructor is virtual.
 */
template <typename ElementType, typename SizeType>
FORCEINLINE typename TEnableIf<!TIsTriviallyDestructible<ElementType>::Value>::Type DestructItems(ElementType* Element, SizeType Count)
{
	while (Count)
	{
		// We need a typedef here because VC won't compile the destructor call below if ElementType itself has a member called ElementType
		typedef ElementType DestructItemsElementTypeTypedef;

		Element->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
		++Element;
		--Count;
	}
}


template <typename ElementType, typename SizeType>
FORCEINLINE typename TEnableIf<TIsTriviallyDestructible<ElementType>::Value>::Type DestructItems(ElementType* Elements, SizeType Count)
{
}

/**
 * Constructs a range of items into memory from a set of arguments.  The arguments come from an another array.
 *
 * @param	Dest		The memory location to start copying into.
 * @param	Source		A pointer to the first argument to pass to the constructor.
 * @param	Count		The number of elements to copy.
 */
template <typename DestinationElementType, typename SourceElementType, typename SizeType>
FORCEINLINE typename TEnableIf<!TIsBitwiseConstructible<DestinationElementType, SourceElementType>::Value>::Type ConstructItems(void* Dest, const SourceElementType* Source, SizeType Count)
{
	while (Count)
	{
		new (Dest) DestinationElementType(*Source);
		++(DestinationElementType*&)Dest;
		++Source;
		--Count;
	}
}


template <typename DestinationElementType, typename SourceElementType, typename SizeType>
FORCEINLINE typename TEnableIf<TIsBitwiseConstructible<DestinationElementType, SourceElementType>::Value>::Type ConstructItems(void* Dest, const SourceElementType* Source, SizeType Count)
{
	FMemory::Memcpy(Dest, Source, sizeof(SourceElementType) * Count);
}

namespace MemoryOps_Private
{
	template <typename DestinationElementType, typename SourceElementType>
	struct TCanBitwiseRelocate
	{
		enum
		{
			Value =
			TOr<
			TAreTypesEqual<DestinationElementType, SourceElementType>,
			TAnd<
			TIsBitwiseConstructible<DestinationElementType, SourceElementType>,
			TIsTriviallyDestructible<SourceElementType>
			>
			>::Value
		};
	};
}


/**
 * Relocates a range of items to a new memory location as a new type. This is a so-called 'destructive move' for which
 * there is no single operation in C++ but which can be implemented very efficiently in general.
 *
 * @param	Dest		The memory location to relocate to.
 * @param	Source		A pointer to the first item to relocate.
 * @param	Count		The number of elements to relocate.
 */
template <typename DestinationElementType, typename SourceElementType, typename SizeType>
FORCEINLINE typename TEnableIf<!MemoryOps_Private::TCanBitwiseRelocate<DestinationElementType, SourceElementType>::Value>::Type RelocateConstructItems(void* Dest, const SourceElementType* Source, SizeType Count)
{
	while (Count)
	{
		// We need a typedef here because VC won't compile the destructor call below if SourceElementType itself has a member called SourceElementType
		typedef SourceElementType RelocateConstructItemsElementTypeTypedef;

		new (Dest) DestinationElementType(*Source);
		++(DestinationElementType*&)Dest;
		(Source++)->RelocateConstructItemsElementTypeTypedef::~RelocateConstructItemsElementTypeTypedef();
		--Count;
	}
}

template <typename DestinationElementType, typename SourceElementType, typename SizeType>
FORCEINLINE typename TEnableIf<MemoryOps_Private::TCanBitwiseRelocate<DestinationElementType, SourceElementType>::Value>::Type RelocateConstructItems(void* Dest, const SourceElementType* Source, SizeType Count)
{
	/* All existing UE containers seem to assume trivial relocatability (i.e. memcpy'able) of their members,
	 * so we're going to assume that this is safe here.  However, it's not generally possible to assume this
	 * in general as objects which contain pointers/references to themselves are not safe to be trivially
	 * relocated.
	 *
	 * However, it is not yet possible to automatically infer this at compile time, so we can't enable
	 * different (i.e. safer) implementations anyway. */

	FMemory::Memmove(Dest, Source, sizeof(SourceElementType) * Count);
}