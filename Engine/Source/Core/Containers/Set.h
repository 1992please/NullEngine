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

	template<typename, typename>
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
	/** Reset a range of FSetElementIds to invalid */
	FORCEINLINE static void ResetRange(FSetElementId* Range, int32 Count)
	{
		for (int32 I = 0; I < Count; ++I)
		{
			Range[I] = FSetElementId();
		}
	}

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
	template <typename InitType> explicit FORCEINLINE TSetElement(InitType&& InValue) 
		: Value(Forward<InitType>(InValue)) {}

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
struct DefaultKeyFuncs
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


/**
 * A set with an optional KeyFuncs parameters for customizing how the elements are compared and searched.
 * E.g. You can specify a mapping from elements to keys if you want to find elements by specifying a subset of
 * the element type.  It uses a TSparseArray of the elements, and also links the elements into a hash with a
 * number of buckets proportional to the number of elements.  Addition, removal, and finding are O(1).
 *
 * The ByHash() functions are somewhat dangerous but particularly useful in two scenarios:
 * -- Heterogeneous lookup to avoid creating expensive keys like FString when looking up by const TCHAR*.
 *	  You must ensure the hash is calculated in the same way as ElementType is hashed.
 *    If possible put both ComparableKey and ElementType hash functions next to each other in the same header
 *    to avoid bugs when the ElementType hash function is changed.
 * -- Reducing contention around hash tables protected by a lock. It is often important to incur
 *    the cache misses of reading key data and doing the hashing *before* acquiring the lock.
 *
 **/
template<typename InElementType, typename KeyFuncs = DefaultKeyFuncs<InElementType>>
class TSet
{
private:
	typedef TSetElement<InElementType>          SetElementType;
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

	/**
	 * Checks whether an element id is valid.
	 * @param Id - The element id to check.
	 * @return true if the element identifier refers to a valid element in this set.
	 */
	FORCEINLINE bool IsValidId(FSetElementId Id) const
	{
		return	Id.IsValidId() &&
			Id >= 0 &&
			Id < Elements.GetMaxIndex() &&
			Elements.IsAllocated(Id);
	}

	/** Accesses the identified element's value. */
	FORCEINLINE ElementType& operator[](FSetElementId Id)
	{
		return Elements[Id].Value;
	}

	/** Accesses the identified element's value. */
	FORCEINLINE const ElementType& operator[](FSetElementId Id) const
	{
		return Elements[Id].Value;
	}


	/**
	 * Finds an element with the given key in the set.
	 * @param Key - The key to search for.
	 * @return The id of the set element matching the given key, or the NULL id if none matches.
	 */
	FSetElementId FindId(KeyInitType Key) const
	{
		if (Elements.Num())
		{
			for (FSetElementId ElementId = GetTypedHash(KeyFuncs::GetKeyHash(Key));
				ElementId.IsValidId();
				ElementId = Elements[ElementId].HashNextId)
			{
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Elements[ElementId].Value), Key))
				{
					// Return the first match, regardless of whether the set has multiple matches for the key or not.
					return ElementId;
				}
			}
		}
		return FSetElementId();
	}

	/**
	 * Finds an element with a pre-calculated hash and a key that can be compared to KeyType
	 * @see	Class documentation section on ByHash() functions
	 * @return The element id that matches the key and hash or an invalid element id
	 */
	template<typename ComparableKey>
	FSetElementId FindIdByHash(uint32 KeyHash, const ComparableKey& Key) const
	{
		if (Elements.Num())
		{
			ASSERT(KeyHash == KeyFuncs::GetKeyHash(Key));

			for (FSetElementId ElementId = GetTypedHash(KeyHash);
				ElementId.IsValidId();
				ElementId = Elements[ElementId].HashNextId)
			{
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Elements[ElementId].Value), Key))
				{
					return ElementId;
				}
			}
		}
		return FSetElementId();
	}

	/**
	 * Finds an element with the given key in the set.
	 * @param Key - The key to search for.
	 * @return A pointer to an element with the given key.  If no element in the set has the given key, this will return NULL.
	 */
	FORCEINLINE ElementType* Find(KeyInitType Key)
	{
		FSetElementId ElementId = FindId(Key);
		if (ElementId.IsValidId())
		{
			return &Elements[ElementId].Value;
		}
		else
		{
			return nullptr;
		}
	}

	/**
	 * Finds an element with a pre-calculated hash and a key that can be compared to KeyType.
	 * @see	Class documentation section on ByHash() functions
	 * @return A pointer to the contained element or nullptr.
	 */
	template<typename ComparableKey>
	ElementType* FindByHash(uint32 KeyHash, const ComparableKey& Key)
	{
		FSetElementId ElementId = FindIdByHash(KeyHash, Key);
		if (ElementId.IsValidId())
		{
			return &Elements[ElementId].Value;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename ComparableKey>
	const ElementType* FindByHash(uint32 KeyHash, const ComparableKey& Key) const
	{
		return const_cast<TSet*>(this)->FindByHash(KeyHash, Key);
	}

	/**
	 * Removes all elements from the set matching the specified key.
	 * @param Key - The key to match elements against.
	 * @return The number of elements removed.
	 */
	int32 Remove(KeyInitType Key)
	{
		if (Elements.Num())
		{
			return RemoveImpl(KeyFuncs::GetKeyHash(Key), Key);
		}

		return 0;
	}

	/**
	 * Removes an element from the set.
	 * @param Element - A pointer to the element in the set, as returned by Add or Find.
	 */
	void Remove(FSetElementId ElementId)
	{
		if (Elements.Num())
		{
			const auto& ElementBeingRemoved = Elements[ElementId];

			// Remove the element from the hash.
			for (FSetElementId* NextElementId = &GetTypedHash(ElementBeingRemoved.HashIndex);
				NextElementId->IsValidId();
				NextElementId = &Elements[*NextElementId].HashNextId)
			{
				if (*NextElementId == ElementId)
				{
					*NextElementId = ElementBeingRemoved.HashNextId;
					break;
				}
			}
		}

		// Remove the element from the elements array.
		Elements.RemoveAt(ElementId);
	}

	/**
	 * Removes all elements from the set, potentially leaving space allocated for an expected number of elements about to be added.
	 * @param ExpectedNumElements - The number of elements about to be added to the set.
	 */
	void Empty(int32 ExpectedNumElements = 0)
	{
		// Empty the elements array, and reallocate it for the expected number of elements.
		const int32 DesiredHashSize = GetNumberOfHashBuckets(ExpectedNumElements);
		const bool ShouldDoRehash = ShouldRehash(ExpectedNumElements, DesiredHashSize, true);

		if (!ShouldDoRehash)
		{
			// If the hash was already the desired size, clear the references to the elements that have now been removed.
			UnhashElements();
		}

		Elements.Empty(ExpectedNumElements);

		// Resize the hash to the desired size for the expected number of elements.
		if (ShouldDoRehash)
		{
			HashSize = DesiredHashSize;
			Rehash();
		}
	}

	/** Efficiently empties out the set but preserves all allocations and capacities */
	void Reset()
	{
		if (Num() == 0)
		{
			return;
		}

		// Reset the elements array.
		UnhashElements();
		Elements.Reset();
	}

	/** Shrinks the set's element storage to avoid slack. */
	FORCEINLINE void Shrink()
	{
		Elements.Shrink();
		Relax();
	}

	/** Compacts the allocated elements into a contiguous range. */
	FORCEINLINE void Compact()
	{
		if (Elements.Compact())
		{
			Rehash();
		}
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

	/** Relaxes the set's hash to a size strictly bounded by the number of elements in the set. */
	FORCEINLINE void Relax()
	{
		ConditionalRehash(Elements.Num(), true);
	}

	/** @return the number of elements. */
	FORCEINLINE int32 Num() const
	{
		return Elements.Num();
	}

	/**
	 * Helper function to return the amount of memory allocated by this container
	 * Only returns the size of allocations made directly by the container, not the elements themselves.
	 * @return number of bytes allocated by this container
	 */
	FORCEINLINE uint32 GetAllocatedSize(void) const
	{
		return Elements.GetAllocatedSize() + (HashSize * sizeof(FSetElementId));
	}
private:
	FSetElementId EmplaceImpl(uint32 KeyHash, SetElementType& Element, FSetElementId ElementId, bool* bIsAlreadyInSetPtr)
	{
		bool bIsAlreadyInSet = false;
		// check for an existing element with the same key as the element being added.

		// Don't bother searching for a duplicate if this is the first element we're adding
		if (Elements.Num() != 1)
		{
			FSetElementId ExistingId = FindIdByHash(KeyHash, KeyFuncs::GetSetKey(Element.Value));
			bIsAlreadyInSet = ExistingId.IsValidId();
			if (bIsAlreadyInSet)
			{
				// If there's an existing element with the same key as the new element, replace the existing element with the new element.
				MoveByRelocate(Elements[ExistingId].Value, Element.Value);

				// Then remove the new element.
				Elements.RemoveAtUninitialized(ElementId);

				// Then point the return value at the replaced element.
				ElementId = ExistingId;
			}
		}

		if (!bIsAlreadyInSet)
		{
			// Check if the hash needs to be resized.
			if (!ConditionalRehash(Elements.Num()))
			{
				// If the rehash didn't add the new element to the hash, add it.
				LinkElement(ElementId, Element, KeyHash);
			}
		}

		if (bIsAlreadyInSetPtr)
		{
			*bIsAlreadyInSetPtr = bIsAlreadyInSet;
		}

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

	/**
	 * Checks if the hash has an appropriate number of buckets, and if it should be resized.
	 * @param NumHashedElements - The number of elements to size the hash for.
	 * @param DesiredHashSize - Desired size if we should rehash.
	 * @param bAllowShrinking - true if the hash is allowed to shrink.
	 * @return true if the set should berehashed.
	 */
	FORCEINLINE bool ShouldRehash(int32 NumHashedElements, int32 DesiredHashSize, bool bAllowShrinking = false) const
	{
		// If the hash hasn't been created yet, or is smaller than the desired hash size, rehash.
		return (NumHashedElements > 0 &&
			(!HashSize ||
				HashSize < DesiredHashSize ||
				(HashSize > DesiredHashSize && bAllowShrinking)));
	}

	/**
	 * Checks if the hash has an appropriate number of buckets, and if not resizes it.
	 * @param NumHashedElements - The number of elements to size the hash for.
	 * @param bAllowShrinking - true if the hash is allowed to shrink.
	 * @return true if the set was rehashed.
	 */
	bool ConditionalRehash(int32 NumHashedElements, bool bAllowShrinking = false) const
	{
		// Calculate the desired hash size for the specified number of elements.
		const int32 DesiredHashSize = GetNumberOfHashBuckets(NumHashedElements);

		if (ShouldRehash(NumHashedElements, DesiredHashSize, bAllowShrinking))
		{
			HashSize = DesiredHashSize;
			Rehash();
			return true;
		}

		return false;
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

	/**
	 * Accesses an element in the set.
	 * This is needed because the iterator classes aren't friends of FSetElementId and so can't access the element index.
	 */
	FORCEINLINE const SetElementType& GetInternalElement(FSetElementId Id) const
	{
		return Elements[Id];
	}
	FORCEINLINE SetElementType& GetInternalElement(FSetElementId Id)
	{
		return Elements[Id];
	}

	template<typename ComparableKey>
	FORCEINLINE int32 RemoveImpl(uint32 KeyHash, const ComparableKey& Key)
	{
		int32 NumRemovedElements = 0;

		FSetElementId* NextElementId = &GetTypedHash(KeyHash);
		while (NextElementId->IsValidId())
		{
			auto& Element = Elements[*NextElementId];
			if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Element.Value), Key))
			{
				// This element matches the key, remove it from the set.  Note that Remove sets *NextElementId to point to the next
				// element after the removed element in the hash bucket.
				Remove(*NextElementId);
				NumRemovedElements++;
				// If the hash disallows duplicate keys, we're done removing after the first matched key.
				break;
			}
			else
			{
				NextElementId = &Element.HashNextId;
			}
		}

		return NumRemovedElements;
	}

	/** Returns if it should be faster to clear the hash by going through elements instead of reseting the whole bucket lists*/
	FORCEINLINE bool ShouldClearByElements()
	{
		return Num() < (HashSize / 4);
	}

	/** Reset elements buckets of FSetElementIds to invalid */
	void UnhashElements()
	{
		if (ShouldClearByElements())
		{
			// Faster path: only reset hash buckets to FSetElementId for elements in the hash
			for (const SetElementType& Element : Elements)
			{
				Hash[Element.HashIndex] = FSetElementId();
			}
		}
		else
		{
			FSetElementId::ResetRange(Hash, HashSize);
		}
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
public:

	/** Used to iterate over the elements of a const TSet. */
	class TConstIterator : public TBaseIterator<true>
	{
		friend class TSet;

	public:
		FORCEINLINE TConstIterator(const TSet& InSet)
			: TBaseIterator<true>(InSet.Elements.begin())
		{
		}
	};

	/** Used to iterate over the elements of a TSet. */
	class TIterator : public TBaseIterator<false>
	{
		friend class TSet;

	public:
		FORCEINLINE TIterator(TSet& InSet)
			: TBaseIterator<false>(InSet.Elements.begin())
			, Set(InSet)
		{
		}

		/** Removes the current element from the set. */
		FORCEINLINE void RemoveCurrent()
		{
			Set.Remove(TBaseIterator<false>::GetId());
		}

	private:
		TSet& Set;
	};

	/** Used to iterate over the elements of a const TSet. */
	class TConstKeyIterator : public TBaseKeyIterator<true>
	{
	public:
		FORCEINLINE TConstKeyIterator(const TSet& InSet, KeyInitType InKey) :
			TBaseKeyIterator<true>(InSet, InKey)
		{}
	};

	/** Used to iterate over the elements of a TSet. */
	class TKeyIterator : public TBaseKeyIterator<false>
	{
	public:
		FORCEINLINE TKeyIterator(TSet& InSet, KeyInitType InKey)
			: TBaseKeyIterator<false>(InSet, InKey)
			, Set(InSet)
		{}

		/** Removes the current element from the set. */
		FORCEINLINE void RemoveCurrent()
		{
			Set.Remove(Id);
			Id = FSetElementId();
		}
	private:
		TSet& Set;
	};

	/** Creates an iterator for the contents of this set */
	FORCEINLINE TIterator CreateIterator()
	{
		return TIterator(*this);
	}

	/** Creates a const iterator for the contents of this set */
	FORCEINLINE TConstIterator CreateConstIterator() const
	{
		return TConstIterator(*this);
	}

	/**
	* DO NOT USE DIRECTLY
	* STL-like iterators to enable range-based for loop support.
	*/
	using TRangedForConstIterator = TBaseIterator<true>;
	using TRangedForIterator = TBaseIterator<false>;

	FORCEINLINE TRangedForIterator      begin()         { return TRangedForIterator(Elements.begin()); }
	FORCEINLINE TRangedForConstIterator begin() const   { return TRangedForConstIterator(Elements.begin()); }
	FORCEINLINE TRangedForIterator      end()           { return TRangedForIterator(Elements.end()); }
	FORCEINLINE TRangedForConstIterator end()   const   { return TRangedForConstIterator(Elements.end()); }
private:
	ElementArrayType Elements;
	mutable FSetElementId* Hash;
	mutable int32	 HashSize;
};