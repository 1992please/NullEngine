#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

class FScript
{
public:

	template<typename T>
	FORCEINLINE T& GetComponent() { return Entity.GetComponent<T>(); }
protected:
    virtual void OnInitialize() {}
    virtual void Start() {}
    virtual void Update(float DeltaTime) {}
    virtual void OnDestroy() {}
private:

	FEntity Entity;

	friend class FEntity;
	friend class FScene;
};

class FNativeScriptComponent
{
public:
	FScript* Script;

	FNativeScriptComponent()
		: Script(nullptr)
	{}

	~FNativeScriptComponent()
	{
		NE_CHECK(!Script);
	}

	template<typename T>
	FORCEINLINE T* Create()
	{
		if (!Script)
		{
			Script = new T();
			return (T*)Script;
		}
		return nullptr;
	}

	FORCEINLINE void Destroy()
    {
		if (Script)
		{
			delete Script;
			Script = nullptr;
		}
    }
};

template<typename T>
FORCEINLINE T* FEntity::AddScript()
{
	FNativeScriptComponent& ScriptComponent = Scene->SceneStorage.AddComponent<FNativeScriptComponent>(EntityID);
	T* Script = ScriptComponent.Create<T>();
	Scene->OnComponentAdded(ScriptComponent, EntityID);
	return Script;
}