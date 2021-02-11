#include "TestGame.h"
#include "imgui/imgui.h"

TestGame::TestGame()
	: CameraController(16.f / 9.f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, SquareColor(FLinearColor::Green)
{
	FRotator x(33, 40, 130);
	FQuat q = x.Quaternion();
	FRotator xfromq = q.Rotator();
	NE_LOG("%s", *xfromq.ToString())
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

		for(int i = 0; i < 10; i++)
			for(int j = 0; j < 10; j++)
				FRenderer2D::DrawQuad({ i * 0.11f - 5 * 0.11f, j * 0.11f - 5 * 0.11f , 0.0f }, FVector2(0.1f), FLinearColor::Green);

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