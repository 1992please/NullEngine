#pragma once
#include "SparseArray.h"
#include "Templates/NullTemplate.h"
#include "Templates/MemoryOps.h"
#include "Templates/ChooseClass.h"
#include "Templates/TypeTraits.h"
#include "Templates/TypeHash.h"
#include <initializer_list>

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
	template <typename InitType> explicit FORCEINLINE TSetElement(InitType&& InValue) : Value(Forward<InitType>(InValue)) {}

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

/**
 * A default implementation of the KeyFuncs used by TSet which uses the element as a key.
 */
template<typename ElementType>
struct TKeyFuncs
{
	typedef typename TTypeTraits<ElementType>::ConstPointerType KeyInitType;
	typedef typename TCallTraits<ElementType>::ParamType ElementInitType;

	/**
	 * @return The key used to index the given element.
	 */
	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element;
	}

	/**
	 * @return True if the keys match.
	 */
	static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
	{
		return A == B;
	}

	/**
	 * @return True if the keys match.
	 */
	template<typename ComparableKey>
	static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
	{
		return A == B;
	}

	/** Calculates a hash index for a key. */
	static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
	{
		return GetTypeHash(Key);
	}

	/** Calculates a hash index for a key. */
	template<typename ComparableKey>
	static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
	{
		return GetTypeHash(Key);
	}
};



template<typename InElementType>
class TSet
{
private:
	typedef TSetElement<InElementType>          SetElementType;
	typedef typename TKeyFuncs<InElementType>   KeyFuncs;
	typedef typename KeyFuncs::KeyInitType      KeyInitType;
	typedef TSparseArray<SetElementType>        ElementArrayType;
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


	/**
	 * Adds an element to the set.
	 *
	 * @param	InElement					Element to add to set
	 * @param	bIsAlreadyInSetPtr	[out]	Optional pointer to bool that will be set depending on whether element is already in set
	 * @return	A pointer to the element stored in the set.
	 */
	FORCEINLINE FSetElementId Add(const InElementType&  InElement, bool* bIsAlreadyInSetPtr = nullptr) { return Emplace(InElement, bIsAlreadyInSetPtr); }
	FORCEINLINE FSetElementId Add(InElementType&& InElement, bool* bIsAlreadyInSetPtr = nullptr) { return Emplace(MoveTempIfPossible(InElement), bIsAlreadyInSetPtr); }
	/**
	 * Adds an element to the set.
	 *
	 * @param	Args						The argument(s) to be forwarded to the set element's constructor.
	 * @param	bIsAlreadyInSetPtr	[out]	Optional pointer to bool that will be set depending on whether element is already in set
	 * @return	A handle to the element stored in the set.
	 */
	template <typename ArgsType>
	FSetElementId Emplace(ArgsType&& Args, bool* bIsAlreadyInSetPtr = nullptr)
	{
		// Create a new element.
		FSparseArrayAllocationInfo ElementAllocation = Elements.AddUninitialized();
		SetElementType& Element = *new (ElementAllocation) SetElementType(Forward<ArgsType>(Args));

		uint32 KeyHash = KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element.Value));
		return EmplaceImpl(KeyHash, Element, ElementAllocation.Index, bIsAlreadyInSetPtr);
	}

private:
	FSetElementId EmplaceImpl(uint32 KeyHash, SetElementType& Element, FSetElementId ElementId, bool* bIsAlreadyInSetPtr)
	{
		//bool bIsAlreadyInSet = false;
		//// check for an existing element with the same key as the element being added.

		//// Don't bother searching for a duplicate if this is the first element we're adding
		//if (Elements.Num() != 1)
		//{
		//	FSetElementId ExistingId = FindIdByHash(KeyHash, KeyFuncs::GetSetKey(Element.Value));
		//	bIsAlreadyInSet = ExistingId.IsValidId();
		//	if (bIsAlreadyInSet)
		//	{
		//		// If there's an existing element with the same key as the new element, replace the existing element with the new element.
		//		MoveByRelocate(Elements[ExistingId].Value, Element.Value);

		//		// Then remove the new element.
		//		Elements.RemoveAtUninitialized(ElementId);

		//		// Then point the return value at the replaced element.
		//		ElementId = ExistingId;
		//	}
		//}

		//if (!bIsAlreadyInSet)
		//{
		//	// Check if the hash needs to be resized.
		//	if (!ConditionalRehash(Elements.Num()))
		//	{
		//		// If the rehash didn't add the new element to the hash, add it.
		//		LinkElement(ElementId, Element, KeyHash);
		//	}
		//}

		//if (bIsAlreadyInSetPtr)
		//{
		//	*bIsAlreadyInSetPtr = bIsAlreadyInSet;
		//}

		return ElementId;
	}

	FORCEINLINE void ResizeHashAllocation(int32 NumElements, size_t NumBytesPerElement) const
	{
		// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
		if (Hash || NumElements)
		{
			//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
			Hash = (FSetElementId*)FMemory::Realloc(Hash, NumElements*NumBytesPerElement);
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

			// Add the existing elements to the new hash.
			for (typename ElementArrayType::TConstIterator ElementIt(Elements); ElementIt; ++ElementIt)
			{
				HashElement(FSetElementId(ElementIt.GetIndex()), *ElementIt);
			}
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

	/** Links an added element to the hash chain. */
	FORCEINLINE void LinkElement(FSetElementId ElementId, const SetElementType& Element, uint32 KeyHash) const
	{
		// Compute the hash bucket the element goes in.
		Element.HashIndex = KeyHash & (HashSize - 1);

		// Link the element into the hash bucket.
		Element.HashNextId = GetTypedHash(Element.HashIndex);
		GetTypedHash(Element.HashIndex) = ElementId;
	}

	/** Hashes and links an added element to the hash chain. */
	FORCEINLINE void HashElement(FSetElementId ElementId, const SetElementType& Element) const
	{
		LinkElement(ElementId, Element, KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element.Value)));
	}

private:
	/** The base type of whole set iterators. */
	template<bool bConst>
	class TBaseIterator
	{
	private:
		friend class TSet;

		typedef typename TChooseClass<bConst, const ElementType, ElementType>::Result ItElementType;

	public:
		typedef typename TChooseClass<bConst, typename ElementArrayType::TConstIterator, typename ElementArrayType::TIterator>::Result ElementItType;

		FORCEINLINE TBaseIterator(const ElementItType& InElementIt)
			: ElementIt(InElementIt)
		{
		}

		/** Advances the iterator to the next element. */
		FORCEINLINE TBaseIterator& operator++()
		{
			++ElementIt;
			return *this;
		}

		/** conversion to "bool" returning true if the iterator is valid. */
		FORCEINLINE explicit operator bool() const
		{
			return !!ElementIt;
		}
		/** inverse of the "bool" operator */
		FORCEINLINE bool operator !() const
		{
			return !(bool)*this;
		}

		// Accessors.
		FORCEINLINE FSetElementId GetId() const
		{
			return TSet::IndexToId(ElementIt.GetIndex());
		}
		FORCEINLINE ItElementType* operator->() const
		{
			return &ElementIt->Value;
		}
		FORCEINLINE ItElementType& operator*() const
		{
			return ElementIt->Value;
		}

		FORCEINLINE friend bool operator==(const TBaseIterator& Lhs, const TBaseIterator& Rhs) { return Lhs.ElementIt == Rhs.ElementIt; }
		FORCEINLINE friend bool operator!=(const TBaseIterator& Lhs, const TBaseIterator& Rhs) { return Lhs.ElementIt != Rhs.ElementIt; }

		ElementItType ElementIt;
	};

	/** The base type of whole set iterators. */
	template<bool bConst>
	class TBaseKeyIterator
	{
	private:
		typedef typename TChooseClass<bConst, const TSet, TSet>::Result SetType;
		typedef typename TChooseClass<bConst, const ElementType, ElementType>::Result ItElementType;
	public:
		/** Initialization constructor. */
		FORCEINLINE TBaseKeyIterator(SetType& InSet, KeyInitType InKey)
			: Set(InSet)
			, Key(InKey)
			, Id()
		{
			// The set's hash needs to be initialized to find the elements with the specified key.
			Set.ConditionalRehash(Set.Elements.Num());
			if (Set.HashSize)
			{
				NextId = Set.GetTypedHash(KeyFuncs::GetKeyHash(Key));
				++(*this);
			}
		}

		/** Advances the iterator to the next element. */
		FORCEINLINE TBaseKeyIterator& operator++()
		{
			Id = NextId;

			while (Id.IsValidId())
			{
				NextId = Set.GetInternalElement(Id).HashNextId;
				ASSERT(Id != NextId);

				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Set[Id]), Key))
				{
					break;
				}

				Id = NextId;
			}
			return *this;
		}

		/** conversion to "bool" returning true if the iterator is valid. */
		FORCEINLINE explicit operator bool() const
		{
			return Id.IsValidId();
		}
		/** inverse of the "bool" operator */
		FORCEINLINE bool operator !() const
		{
			return !(bool)*this;
		}

		// Accessors.
		FORCEINLINE ItElementType* operator->() const
		{
			return &Set[Id];
		}
		FORCEINLINE ItElementType& operator*() const
		{
			return Set[Id];
		}

	protected:
		SetType& Set;
		typename TTypeTraits<typename KeyFuncs::KeyType>::ConstPointerType Key;
		FSetElementId Id;
		FSetElementId NextId;
	};


	ElementArrayType Elements;
	mutable FSetElementId* Hash;
	mutable int32	 HashSize;
};