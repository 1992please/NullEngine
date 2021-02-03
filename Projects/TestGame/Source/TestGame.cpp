#include "TestGame.h"
#include "imgui/imgui.h"

TestGame::TestGame()
	: Camera(3.2f, 1.8f, 0.0f, 1.0f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, ObjectPosition(0.0f)
	, SquarColor(FLinearColor::Green)
{

}

void TestGame::OnAttach()
{
	OurShaderLibrary.Load("../../Engine/Shaders/TextureShader.glsl");
	OurShaderLibrary.Load("../../Engine/Shaders/TestShader2.glsl");
	OurShaderLibrary.Load("../../Engine/Shaders/TestShader.glsl");
	Texture = ITexture2D::Create("../../Projects/TestGame/Content/super_mario.jpg");
	Texture2 = ITexture2D::Create("../../Projects/TestGame/Content/mario_logo.png");
	OurShaderLibrary.Get("TextureShader")->Bind();
	OurShaderLibrary.Get("TextureShader")->SetInt("u_Texture", 0);

	float Verteces[] = {
		0.0f,	0.0f,	0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f,	0.5f,	-0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f,	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f };
	uint32 indices[3] = { 0, 1, 2 };

	float Verteces2[] = {
		0.0f,	.5f,	.5f,	1.f,	1.f,
		0.0f,	.5f,	-.5f,	1.f,	0.f,
		0.0f,	-.5f,	-.5f,	0.f,	0.f,
		0.0f,	-.5f,	.5f,	0.f,	1.f
	};

	uint32 indices2[6] = { 0, 1, 2, 0, 2, 3 };

	IVertexBuffer* VertexBuffer = nullptr;
	IIndexBuffer* IndexBuffer = nullptr;

	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "a_Position" ,  EShaderDataType::Float3 },
			{ "a_Color" , EShaderDataType::Float4 },
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
			{ "a_Position" ,  EShaderDataType::Float3 },
			{ "a_TexCoords" ,  EShaderDataType::Float2 }
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
	//NE_LOG("DeltaTime %f", DeltaTime);
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

	if (IApplicationInput::IsKeyPressed(NE_KEY_I))
	{
		ObjectPosition.Z += DeltaTime;
	}
	else if (IApplicationInput::IsKeyPressed(NE_KEY_K))
	{
		ObjectPosition.Z -= DeltaTime;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_L))
	{
		ObjectPosition.Y += DeltaTime;
	}
	else if (IApplicationInput::IsKeyPressed(NE_KEY_J))
	{
		ObjectPosition.Y -= DeltaTime;
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

	FMatrix ScaleMat = FScaleMatrix(.1f);

	Texture->Bind();
	FRenderer::Submit(OurShaderLibrary.Get("TextureShader"), VertexArray2, FScaleMatrix(1.0f));
	Texture2->Bind();
	FRenderer::Submit(OurShaderLibrary.Get("TextureShader"), VertexArray2, FScaleMatrix(0.3f));
	
	//FRenderer::Submit(Shader, VertexArray);

	FRenderer::EndScene();
}

void TestGame::OnImGuiRender()
{
	ImGui::Begin("Test");
	ImGui::ColorEdit4("Color", &SquarColor.R);
	ImGui::End();
}

void TestGame::OnEvent(IEvent& InEvent)
{

}

FGraphicLayer* GetExternalLayer()
{
	return new TestGame;
}