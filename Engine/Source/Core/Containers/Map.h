#pragma once
#include "Core.h"
#include "Set.h"
#include "Templates/NullTemplate.h"
#include "Templates/TypeTraits.h"

template <typename KeyType, typename ValueType>
struct TPair
{
	KeyType   Key;
	ValueType Value;
	explicit TPair(KeyType&& KeyArg, ValueType&& ValueArg)
		: Key(Forward<KeyType>(KeyArg))
		, Value(Forward<ValueType>(ValueArg))
	{
	}
};

/** An initializer type for pairs that's passed to the pair set when adding a new pair. */
template <typename KeyInitType, typename ValueInitType>
class TPairInitializer
{
public:
	typename TRValueToLValueReference<KeyInitType  >::Type Key;
	typename TRValueToLValueReference<ValueInitType>::Type Value;

	/** Initialization constructor. */
	FORCEINLINE TPairInitializer(KeyInitType InKey, ValueInitType InValue)
		: Key(InKey)
		, Value(InValue)
	{
	}

	/** Implicit conversion to pair initializer. */
	template <typename KeyType, typename ValueType>
	FORCEINLINE TPairInitializer(const TPair<KeyType, ValueType>& Pair)
		: Key(Pair.Key)
		, Value(Pair.Value)
	{
	}

	template <typename KeyType, typename ValueType>
	operator TPair<KeyType, ValueType>() const
	{
		return TPair<KeyType, ValueType>(StaticCast<KeyInitType>(Key), StaticCast<ValueInitType>(Value));
	}
};

/** An initializer type for keys that's passed to the pair set when adding a new key. */
template <typename KeyInitType>
class TKeyInitializer
{
public:
	typename TRValueToLValueReference<KeyInitType>::Type Key;

	/** Initialization constructor. */
	FORCEINLINE explicit TKeyInitializer(KeyInitType InKey)
		: Key(InKey)
	{ }

	template <typename KeyType, typename ValueType>
	operator TPair<KeyType, ValueType>() const
	{
		return TPair<KeyType, ValueType>(StaticCast<KeyInitType>(Key), ValueType());
	}
};

template<typename KeyType, typename ValueType>
struct TDefaultMapHashableKeyFuncs
{
	// Check that the key type is actually hashable
	//
	// If this line fails to compile then your key doesn't have
	// a GetTypeHash() overload.
	using HashabilityCheck = decltype(GetTypeHash(DeclVal<const KeyType>()));

	typedef typename TTypeTraits<KeyType>::ConstPointerType KeyInitType;
	typedef const TPairInitializer<typename TTypeTraits<KeyType>::ConstInitType, typename TTypeTraits<ValueType>::ConstInitType>& ElementInitType;

	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element.Key;
	}

	static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B)
	{
		return A == B;
	}

	template<typename ComparableKey>
	static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
	{
		return A == B;
	}

	static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
	{
		return GetTypeHash(Key);
	}

	template<typename ComparableKey>
	static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
	{
		return GetTypeHash(Key);
	}
};

template<typename KeyType, typename ValueType, typename KeyFuncs = TDefaultMapHashableKeyFuncs<KeyType,ValueType>>
class TMap
{
	template <typename OtherKeyType, typename OtherValueType, typename OtherKeyFuncs>
	friend class TMapBase;

	typedef typename TTypeTraits<KeyType  >::ConstPointerType KeyConstPointerType;
	typedef typename TTypeTraits<KeyType  >::ConstInitType    KeyInitType;
	typedef typename TTypeTraits<ValueType>::ConstInitType    ValueInitType;
	typedef TPair<KeyType, ValueType> ElementType;
public:

	TMap() = default;
	TMap(TMap&&) = default;
	TMap(const TMap&) = default;
	TMap& operator=(TMap&&) = default;
	TMap& operator=(const TMap&) = default;

	/**
	 * Removes all elements from the map.
	 *
	 * This method potentially leaves space allocated for an expected
	 * number of elements about to be added.
	 *
	 * @param ExpectedNumElements The number of elements about to be added to the set.
	 */
	FORCEINLINE void Empty(int32 ExpectedNumElements = 0)
	{
		Pairs.Empty(ExpectedNumElements);
	}

	/** Efficiently empties out the map but preserves all allocations and capacities */
	FORCEINLINE void Reset()
	{
		Pairs.Reset();
	}

	/** Shrinks the pair set to avoid slack. */
	FORCEINLINE void Shrink()
	{
		Pairs.Shrink();
	}

	/** Compacts the pair set to remove holes */
	FORCEINLINE void Compact()
	{
		Pairs.Compact();
	}

	/** Compacts the pair set to remove holes. Does not change the iteration order of the elements. */
	FORCEINLINE void CompactStable()
	{
		Pairs.CompactStable();
	}

	/** Preallocates enough memory to contain Number elements */
	FORCEINLINE void Reserve(int32 Number)
	{
		Pairs.Reserve(Number);
	}

	/** @return The number of elements in the map. */
	FORCEINLINE int32 Num() const
	{
		return Pairs.Num();
	}

	/**
	 * Helper function to return the amount of memory allocated by this container .
	 * Only returns the size of allocations made directly by the container, not the elements themselves.
	 *
	 * @return Number of bytes allocated by this container.
	 * @see CountBytes
	 */
	FORCEINLINE uint32 GetAllocatedSize() const
	{
		return Pairs.GetAllocatedSize();
	}

	/**
	 * Get the unique keys contained within this map.
	 *
	 * @param OutKeys Upon return, contains the set of unique keys in this map.
	 * @return The number of unique keys in the map.
	 */
	int32 GetKeys(TArray<KeyType>& OutKeys) const
	{
		OutKeys.Reset();

		TSet<KeyType> VisitedKeys;
		VisitedKeys.Reserve(Num());

		OutKeys.Reserve(Num());

		for (typename ElementSetType::TConstIterator It(Pairs); It; ++It)
		{
			// Even if bAllowDuplicateKeys is false, we still want to filter for duplicate
			// keys due to maps with keys that can be invalidated (UObjects, TWeakObj, etc.)
			if (!VisitedKeys.Contains(It->Key))
			{
				OutKeys.Add(It->Key);
				VisitedKeys.Add(It->Key);
			}
		}

		return OutKeys.Num();
	}

	/**
	 * Get the unique keys contained within this map.
	 *
	 * @param OutKeys Upon return, contains the set of unique keys in this map.
	 * @return The number of unique keys in the map.
	 */
	int32 GetKeys(TSet<KeyType>& OutKeys) const
	{
		OutKeys.Reset();

		// Presize the set if we know there are supposed to be no duplicate keys
		OutKeys.Reserve(Num());

		for (typename ElementSetType::TConstIterator It(Pairs); It; ++It)
		{
			OutKeys.Add(It->Key);
		}

		return OutKeys.Num();
	}

	/**
	 * Set the value associated with a key.
	 *
	 * @param InKey The key to associate the value with.
	 * @param InValue The value to associate with the key.
	 * @return A reference to the value as stored in the map. The reference is only valid until the next change to any key in the map.
	 */
	FORCEINLINE ValueType& Add(const KeyType&  InKey, const ValueType&  InValue) { return Emplace(InKey, InValue); }
	FORCEINLINE ValueType& Add(const KeyType&  InKey, ValueType&& InValue) { return Emplace(InKey, MoveTempIfPossible(InValue)); }
	FORCEINLINE ValueType& Add(KeyType&& InKey, const ValueType&  InValue) { return Emplace(MoveTempIfPossible(InKey), InValue); }
	FORCEINLINE ValueType& Add(KeyType&& InKey, ValueType&& InValue) { return Emplace(MoveTempIfPossible(InKey), MoveTempIfPossible(InValue)); }

	/**
	 * Set a default value associated with a key.
	 *
	 * @param InKey The key to associate the value with.
	 * @return A reference to the value as stored in the map. The reference is only valid until the next change to any key in the map.
	 */
	FORCEINLINE ValueType& Add(const KeyType&  InKey) { return Emplace(InKey); }
	FORCEINLINE ValueType& Add(KeyType&& InKey) { return Emplace(MoveTempIfPossible(InKey)); }

	/**
	 * Sets the value associated with a key.
	 *
	 * @param InKey The key to associate the value with.
	 * @param InValue The value to associate with the key.
	 * @return A reference to the value as stored in the map. The reference is only valid until the next change to any key in the map.	 */
	template <typename InitKeyType, typename InitValueType>
	ValueType& Emplace(InitKeyType&& InKey, InitValueType&& InValue)
	{
		const FSetElementId PairId = Pairs.Emplace(TPairInitializer<InitKeyType&&, InitValueType&&>(Forward<InitKeyType>(InKey), Forward<InitValueType>(InValue)));

		return Pairs[PairId].Value;
	}

	/**
	 * Set a default value associated with a key.
	 *
	 * @param InKey The key to associate the value with.
	 * @return A reference to the value as stored in the map. The reference is only valid until the next change to any key in the map.
	 */
	template <typename InitKeyType>
	ValueType& Emplace(InitKeyType&& InKey)
	{
		const FSetElementId PairId = Pairs.Emplace(TKeyInitializer<InitKeyType&&>(Forward<InitKeyType>(InKey)));

		return Pairs[PairId].Value;
	}

	/**
	 * Remove all value associations for a key.
	 *
	 * @param InKey The key to remove associated values for.
	 * @return The number of values that were associated with the key.
	 */
	FORCEINLINE int32 Remove(KeyConstPointerType InKey)
	{
		const int32 NumRemovedPairs = Pairs.Remove(InKey);
		return NumRemovedPairs;
	}

	/**
	 * Find the value associated with a specified key.
	 *
	 * @param Key The key to search for.
	 * @return A pointer to the value associated with the specified key, or nullptr if the key isn't contained in this map.  The pointer
	 *			is only valid until the next change to any key in the map.
	 */
	FORCEINLINE ValueType* Find(KeyConstPointerType Key)
	{
		if (auto* Pair = Pairs.Find(Key))
		{
			return &Pair->Value;
		}

		return nullptr;
	}
	FORCEINLINE const ValueType* Find(KeyConstPointerType Key) const
	{
		return const_cast<TMap*>(this)->Find(Key);
	}

private:

	typedef TSet<ElementType, KeyFuncs> ElementSetType;

	/** A set of the key-value pairs in the map. */
	ElementSetType Pairs;
};