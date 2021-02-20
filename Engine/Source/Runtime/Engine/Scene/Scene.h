#pragma once
#include "Engine/Scene/SceneView.h"
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

	template<typename T>
	bool HasComponent(const FEntity* InEntity) const;

	template<typename T>
	T* GetComponent(const FEntity* InEntity);

	template<typename T>
	T* AddComponent(const FEntity* InEntity);

	template<typename T>
	void RemoveComponent(const FEntity* InEntity);


private:
	void* SceneData;

	uint32 ViewportWidth;
	uint32 ViewportHeight;
};
