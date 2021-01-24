#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Core/Application.h"

//#include "GLFW/glfw3.h"

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

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


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

void FImGuiLayer::OnUpdate(float DeltaTime)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool show_demo_window = true;
	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
