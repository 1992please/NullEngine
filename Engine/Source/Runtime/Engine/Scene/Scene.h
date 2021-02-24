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
	void DestructEntity(uint32 EntityID);

	template<typename T>
	void OnComponentAdded(T& InComponent, int32 EntityID);

	template<typename T>
	void OnRemoveComponent(T& InComponent, int32 EntityID);

	const FSceneStorage& GetSceneStorage();
private:
	FSceneStorage SceneStorage;

	void* SceneData;

	uint32 ViewportWidth;
	uint32 ViewportHeight;

	friend class FEntity;
};
