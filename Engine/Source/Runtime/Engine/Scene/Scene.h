#pragma once
class FEntity;

class FScene
{
public:
	FScene();
	~FScene();

	void OnUpdate(float DeltaTime);

	FEntity CreateEntity(const FString& InName = "");

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
};
