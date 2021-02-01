#include "Engine.h"


class TestGame : public FGraphicLayer
{
public:
	TestGame();
	virtual ~TestGame() = default;

	virtual void OnAttach() override;
	virtual void OnDettach() override;

	void OnUpdate(float DeltaTime) override;
	virtual void OnImGuiRender() override;
	void OnEvent(IEvent& InEvent) override;

private:
	class IShader* Shader;
	class IShader* Shader2;
	class IVertexArray* VertexArray;
	class IVertexArray* VertexArray2;

	FVector CameraPosition;
	float CameraRotation;

	FOrthographicCamera Camera;
};