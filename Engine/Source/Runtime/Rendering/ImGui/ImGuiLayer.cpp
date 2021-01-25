#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Core/Application/ApplicationWindow.h"
#include "Core/Application/Application.h"

#include "GLFW/glfw3.h"

FImGuiLayer::FImGuiLayer()
	: FGraphicLayer("ImGuiLayer")
	, bBlockEvents(true)
{
}

FImGuiLayer::~FImGuiLayer()
{
}

void FImGuiLayer::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*) FApplication::GetApplication()->GetWindow()->GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 430 core");
}

void FImGuiLayer::OnDettach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void FImGuiLayer::OnEvent(IEvent& InEvent)
{
	if (bBlockEvents)
	{
		ImGuiIO& io = ImGui::GetIO();
		InEvent.bHandled |= InEvent.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		InEvent.bHandled |= InEvent.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}
}

void FImGuiLayer::OnImGuiUpdate()
{
	static bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

void FImGuiLayer::Begin()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void FImGuiLayer::End()
{
	// Rendering
	ImGuiIO& io = ImGui::GetIO();
	IApplicationWindow* Window = FApplication::GetApplication()->GetWindow();
	io.DisplaySize = ImVec2((float)Window->GetWidth(), (float)Window->GetHeight());

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
