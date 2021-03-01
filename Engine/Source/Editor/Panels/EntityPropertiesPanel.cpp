#include "EntityPropertiesPanel.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

static char TagBuff[32];

namespace ImGui
{
	static bool MyDragFloatN(const char* label, void* p_data, int components, float v_speed = 1.0f, const float p_min = 0.0f, const float p_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		BeginGroup();

		float FullWidth = CalcItemWidth();

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100);

		const char* label_end = FindRenderedTextEnd(label);
		if (label != label_end)
		{
			TextEx(label, label_end);
			SameLine(0, g.Style.ItemInnerSpacing.x);
		}

		ImGui::NextColumn();

		PushID(label);
		PushMultiItemsWidths(components, FullWidth);
		size_t type_size = sizeof(float);
		for (int i = 0; i < components; i++)
		{
			PushID(i);
			if (i > 0)
				SameLine(0, g.Style.ItemInnerSpacing.x);
			value_changed |= DragScalar("", ImGuiDataType_Float, p_data, v_speed, &p_min, &p_max, format, flags);
			PopID();
			PopItemWidth();
			p_data = (void*)((char*)p_data + type_size);
		}
		PopID();

		ImGui::Columns(1);


		EndGroup();
		return value_changed;
	}
};

FEntityPropertiesPanel::FEntityPropertiesPanel(FScene& InScene, FEntity& SelectedEntity)
	: Scene(InScene)
	, SelectedEntity(SelectedEntity)
{

}

template<typename T, typename UIFunction>
static void DrawComponent(const char* InLabel, FEntity SelectedEntity, UIFunction InFunc)
{
	if (SelectedEntity.HasComponent<T>())
	{
		const ImGuiTreeNodeFlags Flags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_Framed;


		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
		ImVec2 RegionAvail = ImGui::GetContentRegionAvail();
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

		ImGui::Separator();
		bool bOpen = (ImGui::TreeNodeEx((void*)(size_t)FSceneStorage::GetComponentStaticID<T>(), Flags, InLabel));
		ImGui::PopStyleVar();

		bool bRemoveComponent = false;
		

		ImGui::SameLine(RegionAvail.x - lineHeight * .5f);
		if (ImGui::Button("-", ImVec2(lineHeight, lineHeight)))
		{
			ImGui::OpenPopup("Component Options");
		}

		if (ImGui::BeginPopup("Component Options"))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				bRemoveComponent = true;
			}


			ImGui::EndPopup();
		}

		if (bOpen)
		{
			T& Component = SelectedEntity.GetComponent<T>();

			InFunc(Component);

			ImGui::TreePop();
		}

		if (bRemoveComponent)
		{
			SelectedEntity.RemoveComponent<T>();
		}
	}
}

void FEntityPropertiesPanel::OnImGUIRender()
{
	ImGui::Begin("Entity Properties");

	if (SelectedEntity)
	{
		ImVec2 RegionAvail = ImGui::GetContentRegionAvail();
		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

		FTagComponent& TagComp = SelectedEntity.GetComponent<FTagComponent>();
		FCString::Strcpy(TagBuff, TagComp.Name);
		if (ImGui::InputText("", TagBuff, sizeof(TagBuff)))
		{
			FCString::Strcpy(TagComp.Name, TagBuff);
		}

		ImGui::SameLine(RegionAvail.x - lineHeight * .5f);
		
		if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!SelectedEntity.HasComponent<FCameraComponent>())
				if (ImGui::MenuItem("Camera"))
				{
					SelectedEntity.AddComponent<FCameraComponent>();
				}

			if (!SelectedEntity.HasComponent<FSpriteComponent>())
				if (ImGui::MenuItem("Sprite"))
				{
					SelectedEntity.AddComponent<FSpriteComponent>();
				}

			ImGui::EndPopup();
		}


		DrawComponent<FTransformComponent>("Transform", SelectedEntity, [](FTransformComponent& TransformComp)
		{
			FTransform& Transform = TransformComp.Transform;
			FVector Position = Transform.GetPosition();
			FRotator Rotation = Transform.GetRotation().Rotator();
			FVector Scale = Transform.GetScale3D();
			if (ImGui::MyDragFloatN("Position", &Position.X, 3, .1f))
			{
				Transform.SetPosition(Position);
			}

			if (ImGui::MyDragFloatN("Rotation", &Rotation.Pitch, 3, .1f))
			{
				Transform.SetRotation(Rotation.Quaternion());
			}

			if (ImGui::MyDragFloatN("Scale", &Scale.X, 3, .1f))
			{
				Transform.SetScale3D(Scale);
			}

		});

		DrawComponent<FCameraComponent>("Camera", SelectedEntity, [](FCameraComponent& CameraComp)
		{
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
			else if (CameraComp.GetProjectionType() == FCameraComponent::PROJ_Perspective)
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

		});

		DrawComponent<FSpriteComponent>("Sprite", SelectedEntity, [](FSpriteComponent& SpriteComponent)
		{
			ImGui::ColorEdit4("Color", &(SpriteComponent.Color.R));

		});
	}
	ImGui::End();
}
