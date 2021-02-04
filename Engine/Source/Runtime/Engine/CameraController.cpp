#include "CameraController.h"

FOrthographicCameraController::FOrthographicCameraController(float InAspectRatio, bool InCanRotate)
	: AspectRatio(InAspectRatio)
	, ZoomLevel(1.0f)
	, Camera(ZoomLevel * AspectRatio, ZoomLevel, 0.0f, 1.f)
	, CameraPosition(FVector::ZeroVector)
	, CameraRotation(0.0f)
	, CameraTranslationSpeed(5.0f)
	, CameraRotationSpeed(180.0f)
	, bCanRotate(InCanRotate)
{ }

void FOrthographicCameraController::OnUpdate(float DeltaTime)
{

}

void FOrthographicCameraController::OnEvent(FEvent& InEvent)
{

}

void FOrthographicCameraController::OnResize(float InWidth, float InHeight)
{

}

bool FOrthographicCameraController::OnMouseScrolled(class FMouseScrolledEvent& InEvent)
{
	return true;
}

bool FOrthographicCameraController::OnWindowResized(class FWindowResizeEvent& InEvent)
{
	return true;
}
