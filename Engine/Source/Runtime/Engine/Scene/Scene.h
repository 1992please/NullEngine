#pragma once
#include "Engine/Scene/SceneView.h"
#include "SceneStorage.h"

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
	bool HasComponent(uint32 EntityID) const;

	template<typename T>
	T* GetComponent(uint32 EntityID);

	template<typename T>
	T* AddComponent(uint32 EntityID);

	template<typename T>
	void RemoveComponent(uint32 EntityID);


private:
	void* SceneData;

	uint32 ViewportWidth;
	uint32 ViewportHeight;
};
