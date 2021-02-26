#include "EntityPropertiesPanel.h"
#include "imgui/imgui.h"
static char TagBuff[32];

FEntityPropertiesPanel::FEntityPropertiesPanel(FScene& InScene, FEntity& SelectedEntity)
	: Scene(InScene)
	, SelectedEntity(SelectedEntity)
{

}

void FEntityPropertiesPanel::OnImGUIRender()
{
	ImGui::Begin("Entity Info");
	if (SelectedEntity)
	{

		FTagComponent& TagComp = SelectedEntity.GetComponent<FTagComponent>();
		FCString::Strcpy(TagBuff, TagComp.Name);
		if (ImGui::InputText("", TagBuff, sizeof(TagBuff)))
		{
			FCString::Strcpy(TagComp.Name, TagBuff);
		}

		if (SelectedEntity.HasComponent<FTransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)0, ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				FTransform& Transform = SelectedEntity.GetComponent<FTransformComponent>().Transform;
				FVector Position = Transform.GetPosition();
				FRotator Rotation = Transform.GetRotation().Rotator();
				FVector Scale = Transform.GetScale3D();
				if (ImGui::DragFloat3("Position", &Position.X, .1f))
				{
					Transform.SetPosition(Position);
				}

				if (ImGui::DragFloat3("Rotation", &Rotation.Pitch, .1f))
				{
					Transform.SetRotation(Rotation.Quaternion());
				}

				if (ImGui::DragFloat3("Scale", &Scale.X, .1f))
				{
					Transform.SetScale3D(Scale);
				}
				ImGui::TreePop();
			}
		}

		if (SelectedEntity.HasComponent<FCameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)1, ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				FCameraComponent& CameraComp = SelectedEntity.GetComponent<FCameraComponent>();

				const char* ProjectionComboStr[] = { "Perspective", "Orthographic" };
				if (ImGui::BeginCombo("Projection", ProjectionComboStr[(int32)CameraComp.GetProjectionType()], 0))
				{
					for (int32 i = 0; i < 2; i++)
					{
						const bool IsSelected = (i == (int32)CameraComp.GetProjectionType());
						if (ImGui::Selectable(ProjectionComboStr[i], IsSelected))
						{
							CameraComp.SetProjectionType((FCameraComponent::EProjectionType) i);
						}
					}
					ImGui::EndCombo();
				}

				if (CameraComp.GetProjectionType() == FCameraComponent::PROJ_Orthographic)
				{
					float Size = CameraComp.GetOrthographicSize();
					float Near = CameraComp.GetOrthographicNear();
					float Far = CameraComp.GetOrthographicFar();
					if (ImGui::DragFloat("Size", &Size))
					{
						CameraComp.SetOrthographicSize(Size);
					}

					if (ImGui::DragFloat("Near", &Near))
					{
						CameraComp.SetOrthographicNear(Near);
					}

					if (ImGui::DragFloat("Far", &Far))
					{
						CameraComp.SetOrthographicFar(Far);
					}
				}
				else if(CameraComp.GetProjectionType() == FCameraComponent::PROJ_Perspective)
				{
					float FOV = CameraComp.GetPerspectiveFOV();
					float Near = CameraComp.GetPerspectiveNear();
					float Far = CameraComp.GetPerspectiveFar();
					if (ImGui::DragFloat("Size", &FOV))
					{
						CameraComp.SetPerspectiveFOV(FOV);
					}

					if (ImGui::DragFloat("Near", &Near))
					{
						CameraComp.SetPerspectiveNear(Near);
					}

					if (ImGui::DragFloat("Far", &Far))
					{
						CameraComp.SetPerspectiveNear(Far);
					}

				}

				ImGui::TreePop();
			}
		}

		if (SelectedEntity.HasComponent<FSpriteComponent>())
		{
			if (ImGui::TreeNodeEx((void*)2, ImGuiTreeNodeFlags_DefaultOpen, "Sprite"))
			{
				FSpriteComponent& SpriteComponent = SelectedEntity.GetComponent<FSpriteComponent>();

				ImGui::ColorEdit4("Color", &(SpriteComponent.Color.R));

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
	ImGui::ShowMetricsWindow();
}
