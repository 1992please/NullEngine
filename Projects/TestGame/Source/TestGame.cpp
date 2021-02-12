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

	FRenderer2D::ResetStatistics();
	{
		NE_PROFILE_SCOPE("Renderer");
		FRenderer2D::BeginScene(CameraController.GetCamera());

		for(int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
			{
				FLinearColor Color = ((i + j) % 2) ? FLinearColor::Black : FLinearColor::White;
				Color.A = 0.3f;

				FRenderer2D::DrawQuad({ i * 0.11f - 5 * 0.11f, j * 0.11f - 5 * 0.11f , 0.0f }, FVector2(0.1f), Color);
			}

		FRenderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, FVector2(0.1f), FLinearColor::White, MarioTexture);
		static float rotation = 0;
		rotation += 50 * DeltaTime;
		FRenderer2D::DrawRotatedQuad({ 0.0f, 0.2f, 0.1f }, FVector2(0.1f), rotation, FLinearColor::White, MarioTexture);
		FRenderer2D::DrawRotatedQuad({ .5f, .5f, 0.1f }, FVector2(0.1f), rotation, FLinearColor::Yellow);

		FRenderer2D::EndScene();
	}
	/*FlatShader->Bind();
	FlatShader->SetVector4("u_Color", SquareColor);*/
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