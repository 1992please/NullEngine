#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

class FScript
{
public:
	template<typename T>
	T* GetComponent() { return Entity.GetComponent<T>(); }
protected:
    virtual void OnInitialize() {}
    virtual void Start() {}
    virtual void Update(float DeltaTime) {}
    virtual void OnDestroy() {}
private:
	FEntity Entity;

	friend class FNativeScriptComponent;
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
		NE_ASSERT(!Script);
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

	template<typename T>
	FORCEINLINE static T* CreateScript(const FEntity* Entity)
	{
		FNativeScriptComponent* ScriptComponent = Entity->AddComponent<FNativeScriptComponent>();
		ScriptComponent->Entity = *Entity;
		return ScriptComponent->Create<T>():
	}

};
