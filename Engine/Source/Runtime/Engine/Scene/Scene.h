#pragma once
#include "Engine/Scene/SceneView.h"
#include "Engine/Scene/SceneStorage.h"

class FEntity;

class FScene
{
public:
	FScene();
	~FScene();

	void OnUpdateRuntime(float DeltaTime);
	void OnUpdateEditor(float DeltaTime, const FSceneView& InSceneView);
	void OnViewportResize(uint32 InWidth, uint32 InHeight);

	FEntity CreateEntity(const FString& InName = "Entity");
	void DestroyEntity(uint32 EntityID);

	template<typename T>
	void OnComponentAdded(T& InComponent, int32 EntityID);

	template<typename T>
	void OnRemoveComponent(T& InComponent, int32 EntityID);

	const FSceneStorage& GetSceneStorage() { return SceneStorage; }

	friend FMemoryArchive& operator<<(FMemoryArchive& Ar, FScene& S);
private:
	FSceneStorage SceneStorage;

	uint32 ViewportWidth;
	uint32 ViewportHeight;

	friend class FEntity;
};
