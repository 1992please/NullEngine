#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/RendererCamera.h"

class FOrthographicCameraController
{
public:
	FOrthographicCameraController(float InAspectRatio, bool InCanRotate);

	void OnUpdate(float DeltaTime);
	void OnEvent(struct FEvent& InEvent);

	void OnResize(float InWidth, float InHeight);

	FOrthographicCamera& GetCamera() { return Camera; }
	const FOrthographicCamera& GetCamera() const { return Camera; }

	float GetZoomLevel() const { return ZoomLevel; }
	void SetZoomLevel(float level) { ZoomLevel = level; }
private:
	bool OnMouseScrolled(class FMouseScrolledEvent& InEvent);
	bool OnWindowResized(class FWindowResizeEvent& InEvent);

private:
	float AspectRatio;
	float ZoomLevel;
	FOrthographicCamera Camera;

	bool bCanRotate;

	FVector CameraPosition;
	float CameraRotation;
	float CameraTranslationSpeed;
	float CameraRotationSpeed;
};