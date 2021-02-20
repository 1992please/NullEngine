#include "TestGame.h"
#include "imgui/imgui.h"

TestGame::TestGame()
	: CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, SquareColor(FLinearColor::Green)
{
}

void TestGame::OnAttach()
{
	NE_PROFILE_FUNCTION();
	MarioTexture = ITexture2D::Create("../../Projects/TestGame/Content/mario_logo.png");
	SpriteSheet = ITexture2D::Create("../../Projects/TestGame/Content/RPGpack_sheet_2X.png");
	FFrameBufferInfo FrameBufferInfo;
	FrameBufferInfo.Width = 1280;
	FrameBufferInfo.Height = 720;
	FrameBufferInfo.Attachments.Add(EFramebufferTextureFormat::RGBA8);
	FrameBufferInfo.Attachments.Add(EFramebufferTextureFormat::Depth);
}

void TestGame::OnDettach()
{
	NE_PROFILE_FUNCTION();
}

void TestGame::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();

	FRenderer2D::ResetStatistics();
	{
		NE_PROFILE_SCOPE("Renderer Start");
		FRenderCommand::SetClearColor(FLinearColor(0.1f, 0.1f, 0.1f, 1));
		FRenderCommand::Clear();
	}
}

void TestGame::OnImGuiRender()
{
	NE_PROFILE_FUNCTION()

	ImGui::Begin("Test");
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", FRenderer2D::GetStatistics().DrawCalls);
	ImGui::Text("Quads: %d", FRenderer2D::GetStatistics().QuadCount);
	ImGui::Text("Vertices: %d", FRenderer2D::GetStatistics().VertexCount());
	ImGui::Text("Indices: %d", FRenderer2D::GetStatistics().IndexCount());
	ImGui::ColorEdit4("Color", &SquareColor.R);


	ImGui::Image((void*)(size_t)MarioTexture->GetRendererID(), ImVec2(1280, 720));
	ImGui::End();

	//ImGui::End();

}

void TestGame::OnEvent(IEvent& InEvent)
{

}