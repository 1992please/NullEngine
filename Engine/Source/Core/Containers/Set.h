#pragma once
#include "SparseArray.h"
#include "Templates/NullTemplate.h"
#include "Templates/MemoryOps.h"

#define NUMBER_OF_ELEMENTS_PER_HASH_BUCKET	2
#define BASE_NUMBER_OF_HASH_BUCKETS			8
#define MIN_NUMBER_OF_HASHED_ELEMENTS		4

/** Either NULL or an identifier for an element of a set. */
class FSetElementId
{
public:

	template<typename>
	friend class TSet;

	/** Default constructor. */
	FORCEINLINE FSetElementId() :
		Index(INDEX_NONE)
	{}

	/** @return a boolean value representing whether the id is NULL. */
	FORCEINLINE bool IsValidId() const
	{
		return Index != INDEX_NONE;
	}

	/** Comparison operator. */
	FORCEINLINE friend bool operator==(const FSetElementId& A, const FSetElementId& B)
	{
		return A.Index == B.Index;
	}

	FORCEINLINE int32 AsInteger() const
	{
		return Index;
	}

	FORCEINLINE static FSetElementId FromInteger(int32 Integer)
	{
		return FSetElementId(Integer);
	}

private:
	/** Initialization constructor. */
	FORCEINLINE FSetElementId(int32 InIndex) :
		Index(InIndex)
	{}

	/** Implicit conversion to the element index. */
	FORCEINLINE operator int32() const
	{
		return Index;
	}

	/** The index of the element in the set's element array. */
	int32 Index;
};

/** An element in the set. */
template <typename InElementType>
class TSetElement
{
public:
	typedef InElementType ElementType;

	/** Default constructor. */
	FORCEINLINE TSetElement()
	{}

	/** Initialization constructor. */
	explicit FORCEINLINE TSetElement(ElementType&& InValue) : Value(Forward<ElementType>(InValue)) {}

	TSetElement(TSetElement&&) = default;
	TSetElement(const TSetElement&) = default;
	TSetElement& operator=(TSetElement&&) = default;
	TSetElement& operator=(const TSetElement&) = default;

	// Comparison operators
	FORCEINLINE bool operator==(const TSetElement& Other) const
	{
		return Value == Other.Value;
	}
	FORCEINLINE bool operator!=(const TSetElement& Other) const
	{
		return Value != Other.Value;
	}

	/** The element's value. */
	ElementType Value;

	/** The id of the next element in the same hash bucket. */
	mutable FSetElementId HashNextId;

	/** The hash bucket that the element is currently linked to. */
	mutable int32 HashIndex;
};


template<typename InElementType>
class TSet
{
private:
	typedef TSetElement<InElementType> SetElementType;
public:
	typedef InElementType ElementType;

	/** Initialization constructor. */
	FORCEINLINE TSet()
		: HashSize(0)
		, Hash(nullptr)
	{}

	/** Copy constructor. */
	FORCEINLINE TSet(const TSet& Copy)
		: HashSize(0)
		, Hash(nullptr)
	{
		*this = Copy;
	}

	FORCEINLINE explicit TSet(const TArray<ElementType>& InArray)
		: HashSize(0)
		, Hash(nullptr)
	{
		Append(InArray);
	}

	FORCEINLINE explicit TSet(TArray<ElementType>&& InArray)
		: HashSize(0)
		, Hash(nullptr)
	{
		Append(MoveTemp(InArray));
	}

	/** Destructor. */
	FORCEINLINE ~TSet()
	{
		HashSize = 0;
	}

	/** Assignment operator. */
	TSet& operator=(const TSet& Copy)
	{
		if (this != &Copy)
		{
			int32 CopyHashSize = Copy.HashSize;

			DestructItems(Hash, HashSize);
			ResizeHashAllocation(CopyHashSize, sizeof(FSetElementId));
			ConstructItems(Hash, Copy.Hash, CopyHashSize);
			HashSize = CopyHashSize;

			Elements = Copy.Elements;
		}
		return *this;
	}

	void Append(const TArray<ElementType>& InElements)
	{
		Reserve(Elements.Num() + InElements.Num());
		for (const ElementType& Element : InElements)
		{
			Add(Element);
		}
	}

	void Append(TArray<ElementType>&& InElements)
	{
		Reserve(Elements.Num() + InElements.Num());
		for (ElementType& Element : InElements)
		{
			Add(MoveTempIfPossible(Element));
		}
		InElements.Reset();
	}

	/** Preallocates enough memory to contain Number elements */
	FORCEINLINE void Reserve(int32 Number)
	{
		// makes sense only when Number > Elements.Num() since TSparseArray::Reserve 
		// does any work only if that's the case
		if (Number > Elements.Num())
		{
			// Preallocates memory for array of elements
			Elements.Reserve(Number);

			// Calculate the corresponding hash size for the specified number of elements.
			const int32 NewHashSize = GetNumberOfHashBuckets(Number);

			// If the hash hasn't been created yet, or is smaller than the corresponding hash size, rehash
			// to force a preallocation of the hash table
			if (!HashSize || HashSize < NewHashSize)
			{
				HashSize = NewHashSize;
				Rehash();
			}
		}
	}

private:
	FORCEINLINE void ResizeHashAllocation(int32 NumElements, size_t NumBytesPerElement)
	{
		// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
		if (Data || NumElements)
		{
			//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
			Data = (ElementType*)FMemory::Realloc(Data, NumElements*NumBytesPerElement);
		}
	}

	/** Resizes the hash. */
	void Rehash() const
	{
		// Free the old hash.
		ResizeHashAllocation(0, sizeof(FSetElementId));

		if (HashSize)
		{
			// Allocate the new hash.
			ASSERT(FMath::IsPowerOfTwo(HashSize));
			ResizeHashAllocation(HashSize, sizeof(FSetElementId));
			for (int32 HashIndex = 0; HashIndex < HashSize; ++HashIndex)
			{
				GetTypedHash(HashIndex) = FSetElementId();
			}

			//// Add the existing elements to the new hash.
			//for (typename ElementArrayType::TConstIterator ElementIt(Elements); ElementIt; ++ElementIt)
			//{
			//	HashElement(FSetElementId(ElementIt.GetIndex()), *ElementIt);
			//}
		}
	}

	/** Computes the number of hash buckets to use for a given number of elements. */
	static FORCEINLINE uint32 GetNumberOfHashBuckets(uint32 NumHashedElements)
	{
		if (NumHashedElements >= MIN_NUMBER_OF_HASHED_ELEMENTS)
		{
			return FMath::RoundUpToPowerOfTwo(NumHashedElements / NUMBER_OF_ELEMENTS_PER_HASH_BUCKET + BASE_NUMBER_OF_HASH_BUCKETS);
		}

		return 1;
	}


	FORCEINLINE FSetElementId& GetTypedHash(int32 HashIndex) const
	{
		return Hash[HashIndex & (HashSize - 1)];
	}

	TSparseArray<SetElementType> Elements;
	mutable FSetElementId* Hash;
	mutable int32	 HashSize;
};