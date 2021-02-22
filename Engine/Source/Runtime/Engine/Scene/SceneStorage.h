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

template<typename ComponentType>
class TComponentPool
{
	FORCEINLINE void Reserve(int32 ExpectedNumElements);
	FORCEINLINE void Reset();
	FORCEINLINE int32 Num() const;
	FORCEINLINE uint32 GetAllocatedSize() const;
	FORCEINLINE int32 GetMaxIndex() const;

	TSparseArray<ComponentType> Pool;
	TArray<uint32> EntityIndeces;
};

class FSceneStorage
{
	uint32 CreateEntity();
	void DestroyEntity(uint32 InEntity);

	uint32 AvailableEntity;
	TArray<uint32> Entities;
};