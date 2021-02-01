#include "TestGame.h"

TestGame::TestGame()
	: Camera(3.2f, 1.8f, 0.0f, 1.0f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
{

}

void TestGame::OnAttach()
{
	Shader = IShader::Create("../../Engine/Shaders/TestShader.glsl");
	Shader2 = IShader::Create("../../Engine/Shaders/TestShader2.glsl");

	float Verteces[] = {
		0.0f,	0.0f,	0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f,	0.5f,	-0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f,	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f };
	uint32 indices[3] = { 0, 1, 2 };

	float Verteces2[] = {
	0.0f,	0.75f,	0.75f,
	0.0f,	0.75f,	-0.75f,
	0.0f,	-0.75f,	-0.75f,
	0.0f, -0.75f, 0.75f };
	uint32 indices2[6] = { 0, 1, 2, 0, 2, 3 };

	IVertexBuffer* VertexBuffer = nullptr;
	IIndexBuffer* IndexBuffer = nullptr;

	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "Position" ,  EShaderDataType::Float3 },
			{ "Color" , EShaderDataType::Float4 },
		};

		VertexBuffer->SetLayout(Layout);
	}

	IndexBuffer = IIndexBuffer::Create(indices, 3);

	VertexArray = IVertexArray::Create();
	VertexArray->AddVertexBuffer(VertexBuffer);
	VertexArray->SetIndexBuffer(IndexBuffer);
	VertexArray->UnBind();

	VertexBuffer = IVertexBuffer::Create(Verteces2, sizeof(Verteces2));
	{
		FBufferLayout Layout = {
			{ "Position" ,  EShaderDataType::Float3 }
		};

		VertexBuffer->SetLayout(Layout);
	}
	IndexBuffer = IIndexBuffer::Create(indices2, 6);

	VertexArray2 = IVertexArray::Create();
	VertexArray2->AddVertexBuffer(VertexBuffer);
	VertexArray2->SetIndexBuffer(IndexBuffer);
	VertexArray2->UnBind();

}

void TestGame::OnDettach()
{

}

void TestGame::OnUpdate(float DeltaTime)
{
	NE_LOG("DeltaTime %f", DeltaTime);
	if (IApplicationInput::IsKeyPressed(NE_KEY_UP))
	{
		CameraPosition.Z += DeltaTime;
	}
	else if (IApplicationInput::IsKeyPressed(NE_KEY_DOWN))
	{
		CameraPosition.Z -= DeltaTime;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_RIGHT))
	{
		CameraPosition.Y += DeltaTime;
	}
	else if (IApplicationInput::IsKeyPressed(NE_KEY_LEFT))
	{
		CameraPosition.Y -= DeltaTime;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_D))
	{
		CameraRotation += DeltaTime * 180;
	}
	else if (IApplicationInput::IsKeyPressed(NE_KEY_A))
	{
		CameraRotation -= DeltaTime * 180;
	}

	Camera.SetPosition(CameraPosition);
	Camera.SetRotation(CameraRotation);

	FRenderer::BeginScene(Camera);

	FRenderer::Submit(Shader2, VertexArray2);

	FRenderer::Submit(Shader, VertexArray);

	FRenderer::EndScene();
}

void TestGame::OnImGuiRender()
{

}

void TestGame::OnEvent(IEvent& InEvent)
{

}

FGraphicLayer* GetExternalLayer()
{
	return new TestGame;
}