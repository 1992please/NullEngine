#pragma once
#include "Engine/Scene/Scene.h"

class FEntity
{
public:
	FEntity()
		: Scene(nullptr), EntityID(0)
	{}

	FEntity(uint32 InEntityID, FScene* InScene)
		: EntityID(InEntityID), Scene(InScene)
	{ }

	template<typename T>
	FORCEINLINE T* AddComponent() const
	{
		NE_ASSERT_F(!HasComponent<T>(), "Entity already not have component!");
		return Scene->AddComponent<T>(EntityID);
	}

	template<typename T>
	FORCEINLINE T* AddScript() const;

	template<typename T>
	FORCEINLINE void RemoveComponent() const
	{
		NE_ASSERT_F(HasComponent<T>(), "Entity does not have component!");
		Scene->RemoveComponent<T>(EntityID);
	}

	template<typename T>
	FORCEINLINE bool HasComponent() const
	{
		return Scene->HasComponent<T>(EntityID);
	}

	template<typename T>
	FORCEINLINE T* GetComponent() const
	{
		NE_ASSERT_F(HasComponent<T>(), "Entity does not have component!");
		return Scene->GetComponent<T>(EntityID);
	}

	FORCEINLINE uint32 GetID() const { return EntityID; }

private:

    uint32 EntityID;
	FScene* Scene;
};
