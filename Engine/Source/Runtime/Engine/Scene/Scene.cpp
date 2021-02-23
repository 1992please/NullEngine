#include "NullPCH.h"
#include "Scene.h"

#include "Engine/Scene/Entity.h"
#include "Renderer/Components/Renderer2D.h"

#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/TagComponent.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Components/NativeScriptComponent.h"



#pragma warning(push)
#pragma warning(disable: 4307) //4307 integral constant overflow

#define ENTT_ASSERT NE_CHECK
#include "entt.hpp"

#pragma warning(pop)

#define REGISTRY(data) ((entt::registry*)data)
#define ENTITY(data) (entt::entity)data

FScene::FScene()
{
	SceneData = new entt::registry;

}

FScene::~FScene()
{
	
}

void FScene::OnUpdateRuntime(float DeltaTime)
{

	// Update scripts
	{
		REGISTRY(SceneData)->view<FNativeScriptComponent>().each([=](FNativeScriptComponent& NativeScriptComponent)
		{
			NativeScriptComponent.Script->Update(DeltaTime);
		});
	}

	// Renderer 2D
	FCameraComponent* MainCamera = nullptr;
	FTransformComponent* CameraTransform = nullptr;
	{
		auto view = REGISTRY(SceneData)->view<FCameraComponent>();
		for (entt::entity entity : view)
		{
			MainCamera = &view.get<FCameraComponent>(entity);
			CameraTransform = &REGISTRY(SceneData)->get<FTransformComponent>(entity);
			break;
		}
	}
	if (MainCamera)
	{
		const FSceneView SceneView = { MainCamera->GetProjectionMatrix(), CameraTransform->Transform.ToInvMatrixNoScale() };
		FRenderer2D::BeginScene(SceneView);
		REGISTRY(SceneData)->group<FTransformComponent>(entt::get<FSpriteComponent>).each([](FTransformComponent& TransComp, FSpriteComponent& SpriteComp) {
			FRenderer2D::DrawQuad(TransComp.Transform.ToMatrixWithScale(), SpriteComp.Color);
		});
		FRenderer2D::EndScene();
	}
}

void FScene::OnUpdateEditor(float DeltaTime, const FSceneView& InSceneView)
{
	FRenderer2D::BeginScene(InSceneView);
	REGISTRY(SceneData)->group<FTransformComponent>(entt::get<FSpriteComponent>).each([](FTransformComponent& TransComp, FSpriteComponent& SpriteComp) {
		FRenderer2D::DrawQuad(TransComp.Transform.ToMatrixWithScale(), SpriteComp.Color);
	});
	FRenderer2D::EndScene();
}

void FScene::OnViewportResize(uint32 InWidth, uint32 InHeight)
{
	ViewportWidth = InWidth;
	ViewportHeight = InHeight;

	REGISTRY(SceneData)->view<FCameraComponent>().each([InWidth, InHeight](FCameraComponent& CameraComp){
		if (!CameraComp.bFixedAspectRatio)
		{
			CameraComp.SetViewportSize(InWidth, InHeight);
		}
	});
}

FEntity FScene::CreateEntity(const FString& InName)
{
	FEntity OutEntity = { (uint32)REGISTRY(SceneData)->create(), this };
	OutEntity.AddComponent<FTransformComponent>();
	FTagComponent* TagComp = OutEntity.AddComponent<FTagComponent>();
	FCString::Strcpy(TagComp->Name, InName.GetCharArray().GetData());
	TagComp->Tag = 0;
	return OutEntity;
}

void FScene::DestructEntity(uint32 EntityID)
{
	REGISTRY(SceneData)->destroy(ENTITY(EntityID));
}

//*******************************HasComponent*************************************************************************************************************/


template<typename T>
bool FScene::HasComponent(uint32 EntityID) const
{
	NE_CHECK(false);
	return false;
}

template<>
bool FScene::HasComponent<FTagComponent>(uint32 EntityID) const
{
	return REGISTRY(SceneData)->has<FTagComponent>(ENTITY(EntityID));
}

template<>
bool FScene::HasComponent<FTransformComponent>(uint32 EntityID) const
{
	return REGISTRY(SceneData)->has<FTransformComponent>(ENTITY(EntityID));
}

template<>
bool FScene::HasComponent<FSpriteComponent>(uint32 EntityID) const
{
	return REGISTRY(SceneData)->has<FSpriteComponent>(ENTITY(EntityID));
}

template<>
bool FScene::HasComponent<FCameraComponent>(uint32 EntityID) const
{
	return REGISTRY(SceneData)->has<FCameraComponent>(ENTITY(EntityID));
}

template<>
bool FScene::HasComponent<FNativeScriptComponent>(uint32 EntityID) const
{
	return REGISTRY(SceneData)->has<FNativeScriptComponent>(ENTITY(EntityID));
}

//*******************************GetComponent*************************************************************************************************************/

template<typename T>
T* FScene::GetComponent(uint32 EntityID)
{
	NE_CHECK(false);
	return nullptr;
}

template<>
FTagComponent* FScene::GetComponent<FTagComponent>(uint32 EntityID)
{
	return &REGISTRY(SceneData)->get<FTagComponent>(ENTITY(EntityID));
}

template<>
FTransformComponent* FScene::GetComponent<FTransformComponent>(uint32 EntityID)
{
	return &REGISTRY(SceneData)->get<FTransformComponent>(ENTITY(EntityID));
}

template<>
FSpriteComponent* FScene::GetComponent<FSpriteComponent>(uint32 EntityID)
{
	return &REGISTRY(SceneData)->get<FSpriteComponent>(ENTITY(EntityID));
}

template<>
FCameraComponent* FScene::GetComponent<FCameraComponent>(uint32 EntityID)
{
	return &REGISTRY(SceneData)->get<FCameraComponent>(ENTITY(EntityID));
}

template<>
FNativeScriptComponent* FScene::GetComponent<FNativeScriptComponent>(uint32 EntityID)
{
	return &REGISTRY(SceneData)->get<FNativeScriptComponent>(ENTITY(EntityID));
}

//*******************************AddComponent*************************************************************************************************************/

template<typename T>
T* FScene::AddComponent(uint32 EntityID)
{
	NE_CHECK(false);
	return nullptr;
}

template<>
FTagComponent* FScene::AddComponent<FTagComponent>(uint32 EntityID)
{
	FTagComponent* Out = &REGISTRY(SceneData)->emplace<FTagComponent>(ENTITY(EntityID));

	return Out;
}

template<>
FTransformComponent* FScene::AddComponent<FTransformComponent>(uint32 EntityID)
{
	FTransformComponent* Out = &REGISTRY(SceneData)->emplace<FTransformComponent>(ENTITY(EntityID));

	return Out;
}

template<>
FSpriteComponent* FScene::AddComponent<FSpriteComponent>(uint32 EntityID)
{
	FSpriteComponent* Out = &REGISTRY(SceneData)->emplace<FSpriteComponent>(ENTITY(EntityID));

	return Out;
}

template<>
FCameraComponent* FScene::AddComponent<FCameraComponent>(uint32 EntityID)
{
	FCameraComponent* Out = &REGISTRY(SceneData)->emplace<FCameraComponent>(ENTITY(EntityID));
	Out->SetViewportSize(ViewportWidth, ViewportHeight);
	return Out;
}

template<>
FNativeScriptComponent* FScene::AddComponent<FNativeScriptComponent>(uint32 EntityID)
{
	FNativeScriptComponent* Out = &REGISTRY(SceneData)->emplace<FNativeScriptComponent>(ENTITY(EntityID));

	return Out;
}
//*******************************RemoveComponent*************************************************************************************************************/

template<typename T>
void FScene::RemoveComponent(uint32 EntityID)
{
	NE_CHECK(false);
}

template<>
void FScene::RemoveComponent<FTagComponent>(uint32 EntityID)
{
	REGISTRY(SceneData)->remove<FTagComponent>(ENTITY(EntityID));
}

template<>
void FScene::RemoveComponent<FTransformComponent>(uint32 EntityID)
{
	REGISTRY(SceneData)->remove<FTransformComponent>(ENTITY(EntityID));
}

template<>
void FScene::RemoveComponent<FSpriteComponent>(uint32 EntityID)
{
	REGISTRY(SceneData)->remove<FSpriteComponent>(ENTITY(EntityID));
}

template<>
void FScene::RemoveComponent<FCameraComponent>(uint32 EntityID)
{
	REGISTRY(SceneData)->remove<FCameraComponent>(ENTITY(EntityID));
}

template<>
void FScene::RemoveComponent<FNativeScriptComponent>(uint32 EntityID)
{
	FNativeScriptComponent* ScriptComponent =  &REGISTRY(SceneData)->get<FNativeScriptComponent>(ENTITY(EntityID));
	if (ScriptComponent->Script)
	{
		ScriptComponent->Script->OnDestroy();
		ScriptComponent->Destroy();
	}

	REGISTRY(SceneData)->remove<FNativeScriptComponent>(ENTITY(EntityID));
}