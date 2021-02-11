#include "NullPCH.h"
#include "CameraController.h"
#include "Core/Events/Event.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Application/ApplicationInput.h"

F2DCameraController::F2DCameraController(float InAspectRatio)
	: AspectRatio(InAspectRatio)
	, ZoomLevel(1.0f)
	, Camera(2 * ZoomLevel * AspectRatio, 2 * ZoomLevel)
	, CameraTranslationSpeed(1.0f)
	, CameraRotationSpeed(180.0f)
	, CameraPos(ForceInit)
{ }

void F2DCameraController::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();

	FVector2 CameraDelta(ForceInit);
	const float Delta = DeltaTime * CameraTranslationSpeed;


	if (IApplicationInput::IsKeyPressed(NE_KEY_W))
	{
		CameraDelta.Y += Delta;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_S))
	{
		CameraDelta.Y -= Delta;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_D))
	{
		CameraDelta.X += Delta;
	}

	if (IApplicationInput::IsKeyPressed(NE_KEY_A))
	{
		CameraDelta.X -= Delta;
	}

	if (!CameraDelta.IsNearlyZero())
	{
		CameraPos += CameraDelta;
		Camera.SetPosition(CameraPos);
	}
}

void F2DCameraController::OnEvent(IEvent& InEvent)
{
	NE_PROFILE_FUNCTION();

	switch (InEvent.GetEventType())
	{
		case EventType::WindowResize:	InEvent.bHandled |= OnWindowResized(static_cast<FWindowResizeEvent&>(InEvent)); break;
		case EventType::MouseScrolled:	InEvent.bHandled |= OnMouseScrolled(static_cast<FMouseScrolledEvent&>(InEvent)); break;
	}
}

void F2DCameraController::OnResize(float InWidth, float InHeight)
{
	NE_PROFILE_FUNCTION();
	AspectRatio = InWidth / InHeight;
	Camera.SetProjection(ZoomLevel * AspectRatio, ZoomLevel);
}

bool F2DCameraController::OnMouseScrolled(class FMouseScrolledEvent& InEvent)
{
	NE_PROFILE_FUNCTION();
	ZoomLevel -= InEvent.GetYOffset() * .1f;
	ZoomLevel = FMath::Max(ZoomLevel, .25f);
	Camera.SetProjection(ZoomLevel * AspectRatio, ZoomLevel);
	return false;
}

bool F2DCameraController::OnWindowResized(class FWindowResizeEvent& InEvent)
{
	OnResize((float)InEvent.GetWidth(), (float)InEvent.GetHeight());
	return false;
}
