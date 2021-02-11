#pragma once
#include "Core/CoreTypes.h"
#include "Engine/RendererCamera.h"
#include "Core/Math/Transform.h"

class F2DCameraController
{
public:
	F2DCameraController(float InAspectRatio);

	void OnUpdate(float DeltaTime);
	void OnEvent(class IEvent& InEvent);

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

	FVector2 CameraPos;

	float CameraTranslationSpeed;
	float CameraRotationSpeed;
};