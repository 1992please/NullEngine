#include "NullPCH.h"
#include "Scene.h"
#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Scene/Entity.h"
#include "Renderer/Components/Renderer2D.h"


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

void FScene::OnUpdate(float DeltaTime)
{
	REGISTRY(SceneData)->group<FTransformComponent>(entt::get<FSpriteComponent>).each([](FTransformComponent& TransComp, FSpriteComponent& SpriteComp) {
		FRenderer2D::DrawQuad(TransComp.Transform.ToMatrixWithScale(), SpriteComp.Color);
	});

}

FEntity FScene::CreateEntity(const FString& InName)
{
	return { (uint32)REGISTRY(SceneData)->create(), this };
}

template<typename T>
bool FScene::HasComponent(const FEntity* InEntity) const
{
	return float;
}

template<>
bool FScene::HasComponent<FSpriteComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
bool FScene::HasComponent<FTransformComponent>(const FEntity* InEntity) const
{
	return REGISTRY(SceneData)->has<FTransformComponent>(ENTITY(InEntity->GetID()));
}

template<typename T>
T* FScene::GetComponent(const FEntity* InEntity)
{
	return nullptr;
}

template<>
FSpriteComponent* FScene::GetComponent<FSpriteComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
FTransformComponent* FScene::GetComponent<FTransformComponent>(const FEntity* InEntity)
{
	return &REGISTRY(SceneData)->get<FTransformComponent>(ENTITY(InEntity->GetID()));
}

template<typename T>
T* FScene::AddComponent(const FEntity* InEntity)
{
	return nullptr;
}

template<>
FSpriteComponent* FScene::AddComponent<FSpriteComponent>(const FEntity* InEntity)
{
	FSpriteComponent* Out = &REGISTRY(SceneData)->emplace<FSpriteComponent>(ENTITY(InEntity->GetID()));

	return Out;
}

template<>
FTransformComponent* FScene::AddComponent<FTransformComponent>(const FEntity* InEntity)
{
	FTransformComponent* Out = &REGISTRY(SceneData)->emplace<FTransformComponent>(ENTITY(InEntity->GetID()));

	return Out;
}

template<typename T>
void FScene::RemoveComponent(const FEntity* InEntity)
{
	NE_ASSERT(false);
}

template<>
void FScene::RemoveComponent<FSpriteComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FSpriteComponent>(ENTITY(InEntity->GetID()));
}

template<>
void FScene::RemoveComponent<FTransformComponent>(const FEntity* InEntity)
{
	REGISTRY(SceneData)->remove<FTransformComponent>(ENTITY(InEntity->GetID()));
}

