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

#define ENTT_ASSERT NE_ASSERT
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
	//{
	//	REGISTRY(SceneData)->view<FNativeScriptComponent>().each([=](auto& nsc)
	//	{
	//		// TODO: Move to Scene::OnScenePlay
	//		if (!nsc.Instance)
	//		{
	//			nsc.Instance = nsc.InstantiateScript();
	//			nsc.Instance->m_Entity = Entity{ entity, this };
	//			nsc.Instance->OnCreate();
	//		}

	//		nsc.Instance->OnUpdate(ts);
	//	});
	//}

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
	TagComp->Name = InName;
	TagComp->Tag = 0;
	return OutEntity;
}

//*******************************HasComponent*************************************************************************************************************/


template<typename T>
bool FScene::HasComponent(const FEntity* InEntity) const
{
	NE_ASSERT(false);
	return false;
}

template<>
bool FScene::HasComponent<FTagComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FTagComponent>(ENTITY(InEntity->GetID()));
}

template<>
bool FScene::HasComponent<FTransformComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FTransformComponent>(ENTITY(InEntity->GetID()));
}

template<>
bool FScene::HasComponent<FSpriteComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
bool FScene::HasComponent<FCameraComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FCameraComponent>(ENTITY(InEntity->GetID()));
}

template<>
bool FScene::HasComponent<FNativeScriptComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FNativeScriptComponent>(ENTITY(InEntity->GetID()));
}

//*******************************GetComponent*************************************************************************************************************/

template<typename T>
T* FScene::GetComponent(const FEntity* InEntity)
{
	NE_ASSERT(false);
	return nullptr;
}

template<>
FTagComponent* FScene::GetComponent<FTagComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FTagComponent>(ENTITY(InEntity->GetID()));
}

template<>
FTransformComponent* FScene::GetComponent<FTransformComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FTransformComponent>(ENTITY(InEntity->GetID()));
}

template<>
FSpriteComponent* FScene::GetComponent<FSpriteComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
FCameraComponent* FScene::GetComponent<FCameraComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FCameraComponent>(ENTITY(InEntity->GetID()));
}

template<>
FNativeScriptComponent* FScene::GetComponent<FNativeScriptComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FNativeScriptComponent>(ENTITY(InEntity->GetID()));
}

//*******************************AddComponent*************************************************************************************************************/

template<typename T>
T* FScene::AddComponent(const FEntity* InEntity)
{
	NE_ASSERT(false);
	return nullptr;
}

template<>
FTagComponent* FScene::AddComponent<FTagComponent>(const FEntity* InEntity)
{
	FTagComponent* Out = &REGISTRY(SceneData)->emplace<FTagComponent>(ENTITY(InEntity->GetID()));

	return Out;
}

template<>
FTransformComponent* FScene::AddComponent<FTransformComponent>(const FEntity* InEntity)
{
	FTransformComponent* Out = &REGISTRY(SceneData)->emplace<FTransformComponent>(ENTITY(InEntity->GetID()));

	return Out;
}

template<>
FSpriteComponent* FScene::AddComponent<FSpriteComponent>(const FEntity* InEntity)
{
	FSpriteComponent* Out = &REGISTRY(SceneData)->emplace<FSpriteComponent>(ENTITY(InEntity->GetID()));

	return Out;
}

template<>
FCameraComponent* FScene::AddComponent<FCameraComponent>(const FEntity* InEntity)
{
	FCameraComponent* Out = &REGISTRY(SceneData)->emplace<FCameraComponent>(ENTITY(InEntity->GetID()));
	Out->SetViewportSize(ViewportWidth, ViewportHeight);
	return Out;
}

template<>
FNativeScriptComponent* FScene::AddComponent<FNativeScriptComponent>(const FEntity* InEntity)
{
	FNativeScriptComponent* Out = &REGISTRY(SceneData)->emplace<FNativeScriptComponent>(ENTITY(InEntity->GetID()));

	return Out;
}
//*******************************RemoveComponent*************************************************************************************************************/

template<typename T>
void FScene::RemoveComponent(const FEntity* InEntity)
{
	NE_ASSERT(false);
}

template<>
void FScene::RemoveComponent<FTagComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FTagComponent>(ENTITY(InEntity->GetID()));
}

template<>
void FScene::RemoveComponent<FTransformComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FTransformComponent>(ENTITY(InEntity->GetID()));
}

template<>
void FScene::RemoveComponent<FSpriteComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
void FScene::RemoveComponent<FCameraComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FCameraComponent>(ENTITY(InEntity->GetID()));
}

template<>
void FScene::RemoveComponent<FNativeScriptComponent>(const FEntity* InEntity)
{
	FNativeScriptComponent* ScriptComponent =  &REGISTRY(SceneData)->get<FNativeScriptComponent>(ENTITY(InEntity->GetID()));
	if (ScriptComponent->Script)
	{
		ScriptComponent->Script->OnDestroy();
		ScriptComponent->Destroy();
	}

	REGISTRY(SceneData)->remove<FNativeScriptComponent>(ENTITY(InEntity->GetID()));
}