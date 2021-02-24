#pragma once

namespace internal
{
	static int32 GetNextComponentIDType()
	{
		static int32 ComponentTypeID = 0;
		return ComponentTypeID++;
	}
}

template<typename ComponentType>
struct TComponentTypeSequence
{
	static int32 Value()
	{
		static const int32 TypeValue = internal::GetNextComponentIDType();
		return TypeValue;
	}
};

class FComponentPoolBase
{
public:
	virtual void Remove(int32 Entity) = 0;

	TSet<int32> OwnerEntities;
	TArray<int32> EntitiesArray;
};

template<typename ComponentType>
class TComponentPool : public FComponentPoolBase
{
public:
	typedef TSparseArray<ComponentType> ComponentSparseArray;

public:
	TComponentPool(int32 NumOfEntities)
	{
		EntitiesArray.SetNum(NumOfEntities);
		FMemory::Memset(EntitiesArray.GetData(), 0xFF, EntitiesArray.Num() * sizeof(int32));
	}

	template <typename... ArgsType>
	FORCEINLINE ComponentType& Emplace(int32 Entity, ArgsType&&... Args)
	{
		NE_CHECK(!Has(Entity));
		const int32 Index = Pool.Emplace(Forward<ArgsType>(Args)...);
		EntitiesArray[Entity] = Index;
		OwnerEntities.Add(Entity);
		return Pool[Index];
	}

	virtual void Remove(int32 Entity) override
	{
		NE_CHECK(Has(Entity));
		Pool.RemoveAt(EntitiesArray[Entity]);

		EntitiesArray[Entity] = INDEX_NONE;
		OwnerEntities.Remove(Entity);
	}

	FORCEINLINE ComponentType& Get(int32 Entity)
	{
		NE_CHECK(Has(Entity));
		return Pool[EntitiesArray[Entity]];
	}

	FORCEINLINE bool Has(int32 Entity) const
	{
		return EntitiesArray[Entity] != INDEX_NONE;
	}

	FORCEINLINE void Reserve(int32 ExpectedNumElements)
	{
		Pool.Reserve(ExpectedNumElements);
		OwnerEntities.Reserve(ExpectedNumElements);
	}

	FORCEINLINE void Reset()
	{
		Pool.Reset();
		FMemory::Memset(EntitiesArray.GetData(), MAX_uint8, EntitiesArray.Num() * sizeof(int32));
		OwnerEntities.Reset();
	}

	FORCEINLINE int32 Num() const
	{
		return Pool.Num();
	}

	FORCEINLINE int32 GetAllocatedSize() const
	{
		return Pool.GetAllocatedSize() + EntitiesArray.GetAllocatedSize() + OwnerEntities.GetAllocatedSize();
	}

	FORCEINLINE int32 Max() const
	{
		return Pool.GetMaxIndex();
	}


public:
	FORCEINLINE typename ComponentSparseArray::TIterator      begin() { return Pool.begin(); }
	FORCEINLINE typename ComponentSparseArray::TConstIterator begin() const { return Pool.begin(); }
	FORCEINLINE typename ComponentSparseArray::TIterator      end() { return Pool.end(); }
	FORCEINLINE typename ComponentSparseArray::TConstIterator end()   const { return Pool.end(); }

private:
	ComponentSparseArray Pool;
};

class FSceneStorage
{
public:
	FORCEINLINE FSceneStorage(int32 InExpectedNumComponentTypes = 20, int32 InExpectedNumOfEntities = 100)
		: AvailableEntity(INDEX_NONE)
	{
		ReserveComponentPools(InExpectedNumComponentTypes);
		ReserveEntities(InExpectedNumOfEntities);
	}

	FORCEINLINE void ReserveComponentPools(int32 InExpectedNumComponentTypes)
	{
		Pools.Reserve(InExpectedNumComponentTypes);
	}

	template<typename ComponentType>
	FORCEINLINE void ReservePool(int32 InNum)
	{
		GetOrCreateComponentPool<ComponentType>()->Reserve(InNum);
	}

	FORCEINLINE void ReserveEntities(int32 InExpectedNumOfEntities)
	{
		Entities.Reserve(InExpectedNumOfEntities);
	}

	FORCEINLINE int32 CreateEntity()
	{
		if (AvailableEntity != INDEX_NONE)
		{
			const int32 Curr = AvailableEntity;
			AvailableEntity = Entities[Curr];
			return Entities[Curr] = Curr;
		}
		else
		{
			NE_CHECK(Entities.Num() < MAX_int32);
			const int32 NewEntity = Entities.Num();
			Entities.Add(NewEntity);
			for (FComponentPoolBase* Pool : Pools)
			{
				Pool->EntitiesArray.Add(INDEX_NONE);
			}
			return NewEntity;
		}
	}

	FORCEINLINE void DestroyEntity(int32 InEntity)
	{
		NE_CHECK(IsValid(InEntity));
		for (FComponentPoolBase* Pool : Pools)
		{
			Pool->Remove(InEntity);
		}
		Entities[InEntity] = AvailableEntity;
		AvailableEntity = InEntity;
	}

	template<typename ComponentType, typename... ArgsType>
	FORCEINLINE ComponentType& AddComponent(int32 InEntity, ArgsType&&... Args)
	{
		return GetOrCreateComponentPool<ComponentType>()->Emplace(InEntity, Forward<ArgsType>(Args)...);
	}

	template<typename ComponentType>
	FORCEINLINE void RemoveComponent(int32 InEntity)
	{
		return GetOrCreateComponentPool<ComponentType>()->Remove(InEntity);
	}

	template<typename ComponentType>
	FORCEINLINE bool HasComponent(int32 InEntity) const
	{
		return GetOrCreateComponentPool<ComponentType>()->Has(InEntity);
	}

	template<typename ComponentType>
	FORCEINLINE ComponentType& GetComponent(int32 InEntity)
	{
		return GetOrCreateComponentPool<ComponentType>()->Get(InEntity);
	}

	FORCEINLINE bool IsValid(int32 InEntity)
	{
		return InEntity < Entities.Num() && InEntity == InEntity;
	}

	FORCEINLINE const int32* GetData() const
	{
		return Entities.GetData();
	}

	template<typename ComponentType>
	FORCEINLINE TComponentPool<ComponentType>& GetPool()
	{
		return *((TComponentPool<ComponentType>*) Pools[TComponentTypeSequence<ComponentType>::Value()]);
	}

	template<typename ComponentType>
	FORCEINLINE TSet<int32>& GetEntities()
	{
		return Pools[TComponentTypeSequence<ComponentType>::Value()]->OwnerEntities;
	}

	template<typename ComponentType>
	FORCEINLINE TComponentPool<ComponentType>* GetOrCreateComponentPool()
	{
		const int32 ComponentTypeID = TComponentTypeSequence<ComponentType>::Value();
		if (ComponentTypeID >= Pools.Num())
		{
			TComponentPool<ComponentType>* NewComponentPool = new TComponentPool<ComponentType>(Entities.Num());
			Pools.Add(NewComponentPool);
			return NewComponentPool;
		}
		else
		{
			return (TComponentPool<ComponentType>*) Pools[ComponentTypeID];
		}
	}
private:
	int32 AvailableEntity;
	TArray<int32> Entities;
	TArray<FComponentPoolBase*> Pools;
};