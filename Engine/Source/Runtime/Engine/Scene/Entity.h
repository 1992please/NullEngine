#pragma once
#include "Engine/Scene/Scene.h"
class FEntity
{
public:
	FEntity(){}

	FEntity(uint32 InEntityID, FScene* InScene)
		: EntityID(InEntityID), Scene(InScene)
	{ }

	template<typename T>
	FORCEINLINE T* AddComponent()
	{
		NE_ASSERT_F(!HasComponent<T>(), "Entity already not have component!");
		return Scene->AddComponent<T>(this);
	}

	template<typename T>
	FORCEINLINE void RemoveComponent()
	{
		NE_ASSERT_F(HasComponent<T>(), "Entity does not have component!");
		return Scene->AddComponent<T>(this);
	}

	template<typename T>
	FORCEINLINE bool HasComponent() const
	{
		return Scene->HasComponent<T>(this);
	}

	template<typename T>
	FORCEINLINE T* GetComponent()
	{
		NE_ASSERT_F(HasComponent<T>(), "Entity does not have component!");
		return Scene->GetComponent<T>(this);
	}

	FORCEINLINE uint32 GetID() const { return EntityID; }

private:
    uint32 EntityID;
	FScene* Scene;
};