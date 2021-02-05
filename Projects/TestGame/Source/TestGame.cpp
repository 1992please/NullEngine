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


}

void TestGame::OnDettach()
{

}

void TestGame::OnUpdate(float DeltaTime)
{
	//NE_LOG("DeltaTime %f", DeltaTime);

	CameraController.OnUpdate(DeltaTime);

	FRenderer2D::BeginScene(CameraController.GetCamera());
	
	FRenderer2D::DrawQuad(FVector2::ZeroVector, FVector2(1.0), { 0.8f, 0.2f, 0.3f, 1.0f });

	FRenderer2D::EndScene();
	/*FlatShader->Bind();
	FlatShader->SetVector4("u_Color", SquareColor);*/
}

void TestGame::OnImGuiRender()
{
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