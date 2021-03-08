#include "EditorLayer.h"
#include "Core/FileSystem/FileUtils.h"
#include "Core/FileSystem/CFile.h"
#include "imgui/imgui.h"

struct FMovemntClass : FScript
{
	bool bPositiveDirection;
	FMovemntClass()
		: bPositiveDirection(false)
	{

	}

	virtual void Start() {}

	virtual void Update(float DeltaTime) override
	{
		FTransform& Transform = GetComponent<FTransformComponent>().Transform;
		FVector Position = Transform.GetPosition();
		if (FApplicationInput::IsKeyPressed(NE_KEY_0))
		{
			bPositiveDirection = false;
		}
		else
		{
			bPositiveDirection = true;
		}

		Transform.Translate(FVector((bPositiveDirection? 1.0f : -1.0f) * DeltaTime, 0.0f, 0.0f));
	}
	virtual void OnDestroy() {}
};

FEditorLayer::FEditorLayer()
	: EditorCamera(16.f / 9.f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, bIsViewportFocused(true)
	, bIsViewportHovered(false)
	, ViewportSize(1.0f, 1.0f)
	, SceneHierarchyPanel(Scene, SelectedEntity)
	, EntityPropertiesPanel(Scene, SelectedEntity)
{
}
struct TestClass
{
	enum EProjectionType : uint8
	{
		PROJ_Perspective = 0,
		PROJ_Orthographic = 1
	};

	FMatrix ProjectionMatrix;

	float AspectRatio;

	// in case of perespective
	float PerspectiveFOV;
	float PerspectiveNear;
	float PerspectiveFar;

	// in case of orthogonal
	float OrthographicSize;
	float OrthographicNear;
	float OrthographicFar;



	EProjectionType ProjectionType;

	bool bPrimaryCamera;
	bool bFixedAspectRatio;
};

void FEditorLayer::OnAttach()
{
	NE_PROFILE_FUNCTION();
	MarioTexture = ITexture2D::Create("../../Projects/TestGame/Content/mario_logo.png");
	SpriteSheet = ITexture2D::Create("../../Projects/TestGame/Content/RPGpack_sheet_2X.png");



	{
		FFrameBufferInfo FrameBufferInfo;
		FrameBufferInfo.Width = 1280;
		FrameBufferInfo.Height = 720;
		FrameBufferInfo.Attachments.Add(EFramebufferTextureFormat::RGBA8);
		FrameBufferInfo.Attachments.Add(EFramebufferTextureFormat::Depth);
		FrameBuffer = IFrameBuffer::Create(FrameBufferInfo);
	}
	TestClass Camera = { 0 };
	//Camera.AspectRatio = 1.33f;
	//Camera.OrthographicFar = 1.33f;
	//TestClass Camera;
	//Camera.DoSomeCalculations();
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);
	Writer << Camera;

	//{
	//	FEntity Entity = Scene.CreateEntity("Green Square");
	//	FSpriteComponent& Comp = Entity.AddComponent<FSpriteComponent>();
	//	Comp.Color = FLinearColor::Green;
	//	//FMovemntClass* Movement = Entity.AddScript<FMovemntClass>();
	//}

	//{
	//	CameraEntity = Scene.CreateEntity("Camera Entity");
	//	CameraEntity.AddComponent<FCameraComponent>();
	//	//CameraEntity.AddScript<FMovemntClass>();
	//}

	//{
	//	FEntity RedEntity = Scene.CreateEntity("Red Square");
	//	RedEntity.AddComponent<FSpriteComponent>();
	//}
	//Scene.CreateEntity("Empty Entity");
}

void FEditorLayer::OnDettach()
{
	NE_PROFILE_FUNCTION();
}

void FEditorLayer::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();
	const FFrameBufferInfo& FrameBufferInfo = FrameBuffer->GetInfo();
	if (!ViewportSize.IsNearlyZero() && 
		(ViewportSize.X != FrameBufferInfo.Width || ViewportSize.Y != FrameBufferInfo.Height))
	{
		FrameBuffer->Resize((uint32)ViewportSize.X, (uint32)ViewportSize.Y);
		EditorCamera.OnResize(ViewportSize.X, ViewportSize.Y);
		Scene.OnViewportResize((uint32)ViewportSize.X, (uint32)ViewportSize.Y);
	}

	if(bIsViewportFocused)
		EditorCamera.OnUpdate(DeltaTime);

	FRenderer2D::ResetStatistics();
	{
		NE_PROFILE_SCOPE("Renderer Draw");
		FrameBuffer->Bind();
		FRenderCommand::SetClearColor(FLinearColor(0.1f, 0.1f, 0.1f, 1));
		FRenderCommand::Clear();

		//Scene.OnUpdateRuntime(DeltaTime);
		Scene.OnUpdateEditor(DeltaTime, FSceneView{EditorCamera.GetProjectionMatrix(), EditorCamera.GetViewMatrix()});
		FrameBuffer->Unbind();
	}
}

void FEditorLayer::OnImGuiRender()
{
	NE_PROFILE_FUNCTION();

	ImGuiStyle& Style = ImGui::GetStyle();
	float MinWinSizeX = Style.WindowMinSize.x;
	Style.WindowMinSize.x = 300;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//if (ImGui::BeginMenuBar())
	//{
	//	ImGui::EndMenuBar();
	//}
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				//FMemoryWriter Ar(SceneSerializationData);
				//Ar << Scene;
				SaveScene();
			}

			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				/*FMemoryReader Ar(SceneSerializationData);
				Ar << Scene;*/
				OpenScene();
			}

			if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S"))
			{
				//FMemoryWriter Ar(SceneSerializationData);
				//Ar << Scene;
				SaveSceneAs();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	Style.WindowMinSize.x = MinWinSizeX;

	ImGui::Begin("Inspector");
	{
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", FRenderer2D::GetStatistics().DrawCalls);
		ImGui::Text("Quads: %d", FRenderer2D::GetStatistics().QuadCount);
		ImGui::Text("Vertices: %d", FRenderer2D::GetStatistics().VertexCount());
		ImGui::Text("Indices: %d", FRenderer2D::GetStatistics().IndexCount());
	}
	ImGui::End();


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	{
		ImGui::Begin("Viewport");
		{
			bIsViewportFocused = ImGui::IsWindowFocused();
			bIsViewportHovered = ImGui::IsWindowHovered();
			FApplication::GetApplication()->SetImGUIBlockingEvents(!bIsViewportFocused || !bIsViewportHovered);
			ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
			ViewportSize = { ViewportPanelSize.x, ViewportPanelSize.y };

			ImGui::Image((void*)(size_t)FrameBuffer->GetColorAttachmentRendererID(), ImVec2(ViewportSize.X, ViewportSize.Y), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
	}
	ImGui::PopStyleVar();

	SceneHierarchyPanel.OnImGUIRender();
	EntityPropertiesPanel.OnImGUIRender();
}

void FEditorLayer::OnEvent(IEvent& InEvent)
{
	EditorCamera.OnEvent(InEvent);
}

void FEditorLayer::NewScene()
{
	CurrentScenePath.Reset();
	Scene.Reset();
}

void FEditorLayer::OpenScene()
{
	FString SceneFilePath;

	if (FFileUtils::OpenFileDialog("", "SceneFiles\0*.scene\0", SceneFilePath))
	{
		bool ReadSuccess = false;
		TArray<uint8> FileData;
		{
			FCFile SceneFile(*SceneFilePath, false);
			ReadSuccess = SceneFile.ReadData(FileData);
		}

		if (ReadSuccess)
		{
			FMemoryReader Ar(FileData);
			Ar << Scene;
			CurrentScenePath = SceneFilePath;
		}
	}
}

void FEditorLayer::SaveScene()
{
	if (CurrentScenePath.IsEmpty())
	{
		SaveSceneAs();
	}
	else
	{
		TArray<uint8> FileData;
		FMemoryWriter Ar(FileData);
		Ar << Scene;

		{
			FCFile SceneFile(*CurrentScenePath, true);
			SceneFile.WriteData(FileData);
		}
	}
}

void FEditorLayer::SaveSceneAs()
{
	FString SceneFilePath;
	if (FFileUtils::SaveFileDialog("", "SceneFiles\0*.scene\0", SceneFilePath))
	{
		TArray<uint8> FileData;
		FMemoryWriter Ar(FileData);
		Ar << Scene;

		{
			FCFile SceneFile(*SceneFilePath, true);
			if (SceneFile.WriteData(FileData))
			{
				CurrentScenePath = SceneFilePath;
			}
		}
	}
}
