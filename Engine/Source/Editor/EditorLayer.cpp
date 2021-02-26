#include "EditorLayer.h"
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

		//Transform.Translate(FVector((bPositiveDirection? 1.0f : -1.0f) * DeltaTime, 0.0f, 0.0f));
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

	{
		FEntity Entity = Scene.CreateEntity("Green Square");
		FSpriteComponent& Comp = Entity.AddComponent<FSpriteComponent>();
		Comp.Color = FLinearColor::Green;
		FMovemntClass* Movement = Entity.AddScript<FMovemntClass>();
	}

	{
		CameraEntity = Scene.CreateEntity("Camera Entity");
		CameraEntity.AddComponent<FCameraComponent>();
		//CameraEntity.AddScript<FMovemntClass>();
	}

	{
		FEntity RedEntity = Scene.CreateEntity("Red Square");
		RedEntity.AddComponent<FSpriteComponent>();
	}

	Scene.CreateEntity("Empty Entity");
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

		Scene.OnUpdateRuntime(DeltaTime);
		//Scene->OnUpdateEditor(DeltaTime, FSceneView{EditorCamera.GetProjectionMatrix(), EditorCamera.GetViewMatrix()});
		FrameBuffer->Unbind();
	}
}

void FEditorLayer::OnImGuiRender()
{
	NE_PROFILE_FUNCTION()
#if 0
	static bool p_open = true;
	// In 99% case you should be able to just call DockSpaceOverViewport() and ignore all the code below!
	// In this specific demo, we are not using DockSpaceOverViewport() because:
	// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
	// - we allow the host window to have padding (when opt_padding == true)
	// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
	// TL;DR; this demo is more complicated than what you would normally use.
	// If we removed all the options we are showcasing, this demo would become:
	//     void ShowExampleAppDockSpace()
	//     {
	//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//     }

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
			ImGui::Separator();

			if (ImGui::MenuItem("Close", NULL, false))
				p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

#endif
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//if (ImGui::BeginMenuBar())
	//{
	//	ImGui::EndMenuBar();
	//}
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