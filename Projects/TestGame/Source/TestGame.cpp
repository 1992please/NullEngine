#include "TestGame.h"
#include "imgui/imgui.h"

TestGame::TestGame()
	: CameraController(16.f / 9.f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, SquareColor(FLinearColor::Green)
{

}

void TestGame::OnAttach()
{
	NE_PROFILE_FUNCTION();
	MarioTexture = ITexture2D::Create("../../Projects/TestGame/Content/mario_logo.png");

}

void TestGame::OnDettach()
{
	NE_PROFILE_FUNCTION();
}

void TestGame::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();
		//NE_LOG("DeltaTime %f", DeltaTime);
	
	{
		NE_PROFILE_SCOPE("Camera Controller");
		CameraController.OnUpdate(DeltaTime);
	}


	{
		NE_PROFILE_SCOPE("Renderer Start");
		FRenderCommand::SetClearColor(FLinearColor(0.1f, 0.1f, 0.1f, 1));
		FRenderCommand::Clear();
	}


	{
		NE_PROFILE_SCOPE("Renderer");
		FRenderer2D::BeginScene(CameraController.GetCamera());

		FRenderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, FVector2(10.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		//FRenderer2D::DrawQuad({ .0f, .0f , 0.5f}, FVector2(1.0f), { 0.3f, 0.2f, 0.8f, 1.0f });
		//FRenderer2D::DrawQuad({ -0.1f, -0.1f, .1f }, FVector2(1.0f), { 0.2f, 0.8f, 0.3f, 1.0f });
		FRenderer2D::DrawQuad({ 0, 0 , 1.0f }, FVector2(1.0f), FLinearColor(1.0f, 1.0f, 1.0f, .5), MarioTexture);

		FRenderer2D::EndScene();
	}
	/*FlatShader->Bind();
	FlatShader->SetVector4("u_Color", SquareColor);*/
}

void TestGame::OnImGuiRender()
{
	NE_PROFILE_FUNCTION()
	ImGui::Begin("Test");
	ImGui::ColorEdit4("Color", &SquareColor.R);
	ImGui::End();
}

void TestGame::OnEvent(IEvent& InEvent)
{
	CameraController.OnEvent(InEvent);
}

FGraphicLayer* GetExternalLayer()
{
	return new TestGame;
}