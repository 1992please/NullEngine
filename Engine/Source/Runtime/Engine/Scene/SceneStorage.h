#pragma once

namespace internal
{
	static uint32 GetNextComponentIDType()
	{
		static uint32 ComponentTypeID = 0;
		return ComponentTypeID++;
	}
}

template<typename ComponentType>
struct TComponentTypeSequence
{
	static uint32 Value()
	{
		static const uint32 TypeValue = internal::GetNextComponentIDType();
		return TypeValue;
	}
};

class FComponentPoolBase
{
public:
	virtual void Remove(uint32 Entity) = 0;

	TArray<uint32> EntityIndeces;
};

template<typename ComponentType>
class TComponentPool : public FComponentPoolBase
{
public:
	typedef TSparseArray<ComponentType> ComponentSparseArray;

	template <typename... ArgsType>
	FORCEINLINE ComponentType& Emplace(uint32 Entity, ArgsType&&... Args)
	{
		NE_CHECK(!Has(Entity));
		const int32 Index = Pool.Emplace(Forward<ArgsType>(Args)...);
		EntityIndeces[Entity] = Index;
	}

	virtual void Remove(uint32 Entity) override
	{
		NE_CHECK(Has(Entity));
		const int32 Index = EntityIndeces[Entity];
		Pool.RemoveAt(Index);
		EntityIndeces[Entity] = MAX_uint32;
	}

	FORCEINLINE ComponentType& Get(uint32 Entity)
	{
		NE_CHECK(Has(Entity));
		const int32 Index = EntityIndeces[Entity];
		return Pool[Index];
	}

	FORCEINLINE bool Has(uint32 Entity) const
	{
		return EntityIndeces[Entity] != MAX_uint32;
	}

	FORCEINLINE void Reserve(int32 ExpectedNumElements)
	{
		Pool.Reserve(ExpectedNumElements);
	}

	FORCEINLINE void Reset()
	{
		Pool.Reset();
		FMemory::Memset(EntityIndeces.GetData(), MAX_uint8, EntityIndeces.GetAllocatedSize());
	}

	FORCEINLINE int32 Num() const
	{
		return Pool.Num();
	}

	FORCEINLINE uint32 GetAllocatedSize() const
	{
		return Pool.GetAllocatedSize() + EntityIndeces.GetAllocatedSize();
	}

	FORCEINLINE int32 Max() const
	{
		return Pool.GetMaxIndex();
	}

	FORCEINLINE const uint32* GetEntitiesData() const
	{
		return EntityIndeces.GetData();
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
		: AvailableEntity(MAX_uint32)
	{
		ReserveComponentPools(InExpectedNumComponentTypes);
		ReserveEntities(InExpectedNumOfEntities);
	}

	FORCEINLINE void ReserveComponentPools(int32 InExpectedNumComponentTypes)
	{
		Pools.Reserve(InExpectedNumComponentTypes);
	}

	template<typename ComponentType>
	FORCEINLINE void ReservePool(uint32 InNum);

	FORCEINLINE void ReserveEntities(uint32 InExpectedNumOfEntities)
	{
		Entities.Reserve(InExpectedNumOfEntities);
	}

	FORCEINLINE uint32 CreateEntity()
	{
		if (AvailableEntity != MAX_uint32)
		{
			const uint32 Curr = AvailableEntity;
			AvailableEntity = Entities[Curr];
			return Entities[Curr] = Curr;
		}
		else
		{
			NE_CHECK(Entities.Num() < MAX_uint32);
			Entities.Add(Entities.Num());
			for (FComponentPoolBase* Pool : Pools)
			{
				Pool->EntityIndeces.Add(MAX_uint32);
			}
			return Entities.Num();
		}
	}

	FORCEINLINE void DestroyEntity(uint32 InEntity)
	{
		for (FComponentPoolBase* Pool : Pools)
		{
			Pool->Remove(InEntity);
		}
		Entities[InEntity] = MAX_uint32;
		// todo continue from here tomorrrow
	}

	template<typename ComponentType>
	FORCEINLINE ComponentType& AddComponent(uint32 InEntity)
	{

	}

	template<typename ComponentType>
	FORCEINLINE void RemoveComponent(uint32 InEntity);

	template<typename ComponentType>
	FORCEINLINE bool HasComponent(uint32 InEntity) const;

	template<typename ComponentType>
	FORCEINLINE ComponentType& GetComponent(uint32 InEntity);

	FORCEINLINE bool IsValid(uint32 InEntity);

	FORCEINLINE const uint32* GetData() const;

	template<typename ComponentType>
	FORCEINLINE const uint32* GetData() const;
private:
	uint32 AvailableEntity;
	TArray<uint32> Entities;
	TArray<FComponentPoolBase*> Pools;
};