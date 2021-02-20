#include "NullPCH.h"
#include "EditorCamera.h"
#include "Core/Math/OrthoMatrix.h"
#include "Core/Math/PrespectiveMatrix.h"
#include "Core/Events/Event.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Application/ApplicationInput.h"

FEditorCamera::FEditorCamera(float InAspectRatio)
	: AspectRatio(InAspectRatio)
	, CameraTranslationSpeed(1.0f)
	, CameraRotationSpeed(180.0f)
	, PerspectiveFOV(45.f)
	, NearClip(0.01f)
	, FarClip(1000.0f)
	, OrthographicSize(10.0f)
	, ProjectionType(PROJ_Orthographic)
{
	RecalculateProjection();
}

void FEditorCamera::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();

	FVector2 CameraDelta(ForceInit);
	const float Delta = DeltaTime * CameraTranslationSpeed;


	if (FApplicationInput::IsKeyPressed(NE_KEY_W))
	{
		CameraDelta.Y += Delta;
	}

	if (FApplicationInput::IsKeyPressed(NE_KEY_S))
	{
		CameraDelta.Y -= Delta;
	}

	if (FApplicationInput::IsKeyPressed(NE_KEY_D))
	{
		CameraDelta.X += Delta;
	}

	if (FApplicationInput::IsKeyPressed(NE_KEY_A))
	{
		CameraDelta.X -= Delta;
	}

	if (!CameraDelta.IsNearlyZero())
	{
		CameraTransform.Translate(CameraDelta);
	}
}

void FEditorCamera::OnEvent(IEvent& InEvent)
{
	NE_PROFILE_FUNCTION();

	switch (InEvent.GetEventType())
	{
		case EventType::WindowResize:	InEvent.bHandled |= OnWindowResized(static_cast<FWindowResizeEvent&>(InEvent)); break;
		case EventType::MouseScrolled:	InEvent.bHandled |= OnMouseScrolled(static_cast<FMouseScrolledEvent&>(InEvent)); break;
	}
}

void FEditorCamera::OnResize(float InWidth, float InHeight)
{
	NE_PROFILE_FUNCTION();
	AspectRatio = InWidth / InHeight;
	RecalculateProjection();
}

bool FEditorCamera::OnMouseScrolled(class FMouseScrolledEvent& InEvent)
{
	NE_PROFILE_FUNCTION();
	if (ProjectionType == PROJ_Orthographic)
	{
		OrthographicSize -= InEvent.GetYOffset() * .1f;
		OrthographicSize = FMath::Max(OrthographicSize, .25f);
		RecalculateProjection();
	}
	return false;
}

bool FEditorCamera::OnWindowResized(class FWindowResizeEvent& InEvent)
{
	OnResize((float)InEvent.GetWidth(), (float)InEvent.GetHeight());
	return false;
}

void FEditorCamera::RecalculateProjection()
{
	if (ProjectionType == PROJ_Perspective)
	{
		ProjectionMatrix = FPerspectiveMatrix(PerspectiveFOV, AspectRatio, NearClip, FarClip);
	}
	else
	{
		const float HalfWidth = OrthographicSize * AspectRatio * 0.5f;
		const float HalfHeight = OrthographicSize * 0.5f;
		const float OrthoLeft = -HalfWidth;
		const float OrthoRight = HalfWidth;
		const float OrthoBottom = -HalfHeight;
		const float OrthoTop = HalfHeight;

		ProjectionMatrix = FOrthoMatrix(OrthoLeft, OrthoRight, OrthoTop, OrthoBottom, 0.0f, FarClip);
	}
}
