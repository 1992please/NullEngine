#include "NullPCH.h"
#include "Scene.h"

#include "Engine/Scene/Entity.h"
#include "Renderer/Components/Renderer2D.h"

#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/TagComponent.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Components/NativeScriptComponent.h"


FScene::FScene()
	:SceneStorage(20, 100)
{
	SceneStorage.ReservePool<FTagComponent>(200);
	SceneStorage.ReservePool<FTransformComponent>(200);
	SceneStorage.ReservePool<FCameraComponent>(5);
	SceneStorage.ReservePool<FSpriteComponent>(150);
	SceneStorage.ReservePool<FNativeScriptComponent>(100);
}

FScene::~FScene()
{
	
}

void FScene::OnUpdateRuntime(float DeltaTime)
{

	// Update scripts
	{
		for (FNativeScriptComponent& NativeScriptComponent : SceneStorage.GetPool<FNativeScriptComponent>())
		{
			NativeScriptComponent.Script->Update(DeltaTime);
		}
	}

	// Renderer 2D
	FCameraComponent* MainCamera = nullptr;
	FTransformComponent* CameraTransform = nullptr;
	for (int32 Entity : SceneStorage.GetEntities<FCameraComponent>())
	{
		MainCamera = &SceneStorage.GetComponent<FCameraComponent>(Entity);
		CameraTransform = &SceneStorage.GetComponent<FTransformComponent>(Entity);
	}

	if (MainCamera)
	{
		const FSceneView SceneView = { MainCamera->GetProjectionMatrix(), CameraTransform->Transform.ToInvMatrixNoScale() };
		FRenderer2D::BeginScene(SceneView);
		for (int32 Entity : SceneStorage.GetEntities<FSpriteComponent>())
		{
			const FSpriteComponent& SpriteComp = SceneStorage.GetComponent<FSpriteComponent>(Entity);
			const FTransformComponent& TransComp = SceneStorage.GetComponent<FTransformComponent>(Entity);
			FRenderer2D::DrawQuad(TransComp.Transform.ToMatrixWithScale(), SpriteComp.Color);
		}

		FRenderer2D::EndScene();
	}
}

void FScene::OnUpdateEditor(float DeltaTime, const FSceneView& InSceneView)
{
	FRenderer2D::BeginScene(InSceneView);
	for (int32 Entity : SceneStorage.GetEntities<FSpriteComponent>())
	{
		const FSpriteComponent& SpriteComp = SceneStorage.GetComponent<FSpriteComponent>(Entity);
		const FTransformComponent& TransComp = SceneStorage.GetComponent<FTransformComponent>(Entity);
		FRenderer2D::DrawQuad(TransComp.Transform.ToMatrixWithScale(), SpriteComp.Color);
	}
	FRenderer2D::EndScene();
}

void FScene::OnViewportResize(uint32 InWidth, uint32 InHeight)
{
	ViewportWidth = InWidth;
	ViewportHeight = InHeight;

	for (FCameraComponent& CameraComp : SceneStorage.GetPool<FCameraComponent>())
	{
		if (!CameraComp.bFixedAspectRatio)
		{
			CameraComp.SetViewportSize(InWidth, InHeight);
		}
	}
}

FEntity FScene::CreateEntity(const FString& InName)
{
	FEntity OutEntity = { SceneStorage.CreateEntity(), this };
	OutEntity.AddComponent<FTransformComponent>();
	FTagComponent& TagComp = OutEntity.AddComponent<FTagComponent>();
	FCString::Strcpy(TagComp.Name, InName.GetCharArray().GetData());
	TagComp.Tag = 0;
	return OutEntity;
}

void FScene::DestroyEntity(uint32 EntityID)
{
	SceneStorage.DestroyEntity(EntityID);
}

template<typename T>
void FScene::OnComponentAdded(T& InComponent, int32 EntityID)
{
	NE_CHECK(false);
}

template<>
void FScene::OnComponentAdded(FTagComponent& InComponent, int32 EntityID)
{

}

template<>
void FScene::OnComponentAdded(FTransformComponent& InComponent, int32 EntityID)
{

}

template<>
void FScene::OnComponentAdded(FSpriteComponent& InComponent, int32 EntityID)
{

}

template<>
void FScene::OnComponentAdded(FCameraComponent& InComponent, int32 EntityID)
{
	InComponent.SetViewportSize(ViewportWidth, ViewportHeight);
}

template<>
void FScene::OnComponentAdded(FNativeScriptComponent& InComponent, int32 EntityID)
{
	if (InComponent.Script)
	{
		InComponent.Script->Entity = { EntityID, this };
	}
}
//*******************************RemoveComponent*************************************************************************************************************/

template<typename T>
void FScene::OnRemoveComponent(T& InComponent, int32 EntityID)
{
	NE_CHECK(false);
}

template<>
void FScene::OnRemoveComponent(FTagComponent& InComponent, int32 EntityID)
{
}

template<>
void FScene::OnRemoveComponent(FTransformComponent& InComponent, int32 EntityID)
{
}

template<>
void FScene::OnRemoveComponent(FSpriteComponent& InComponent, int32 EntityID)
{
}

template<>
void FScene::OnRemoveComponent(FCameraComponent& InComponent, int32 EntityID)
{
}

template<>
void FScene::OnRemoveComponent(FNativeScriptComponent& InComponent, int32 EntityID)
{
	if (InComponent.Script)
	{
		InComponent.Script->OnDestroy();
		InComponent.Destroy();
	}

}