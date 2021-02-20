#include "NullEngine.h"


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
	IShader* FlatShader;
	class ITexture2D* MarioTexture;
	class ITexture2D* SpriteSheet;

	class IVertexArray* VertexArray;
	FVector CameraPosition;
	float CameraRotation;

	FLinearColor SquareColor;
	//F2DCameraController CameraController;
};