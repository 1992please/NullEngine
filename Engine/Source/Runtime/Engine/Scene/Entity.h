#pragma once
#include "Engine/Scene/Scene.h"

class FEntity
{
public:
	FEntity()
		: Scene(nullptr), EntityID(INDEX_NONE)
	{}

	FEntity(int32 InEntityID, FScene* InScene)
		: EntityID(InEntityID), Scene(InScene)
	{ }

	template<typename T, typename... ArgsType>
	FORCEINLINE T& AddComponent(ArgsType&&... Args)
	{
		T& NewComponent = Scene->SceneStorage.AddComponent<T>(EntityID, Forward<ArgsType>(Args)...);
		Scene->OnComponentAdded(NewComponent, EntityID);
		return NewComponent;
	}

	template<typename T>
	FORCEINLINE T* AddScript();

	template<typename T>
	FORCEINLINE void RemoveComponent() const
	{
		Scene->OnRemoveComponent(GetComponent<T>(), EntityID);
		Scene->SceneStorage.RemoveComponent<T>(EntityID);
	}

	template<typename T>
	FORCEINLINE bool HasComponent() const
	{
		return Scene->SceneStorage.HasComponent<T>(EntityID);
	}

	template<typename T>
	FORCEINLINE T& GetComponent() const
	{
		return Scene->SceneStorage.GetComponent<T>(EntityID);
	}

	FORCEINLINE int32 GetID() const { return EntityID; }

private:

    int32 EntityID;
	FScene* Scene;
};
