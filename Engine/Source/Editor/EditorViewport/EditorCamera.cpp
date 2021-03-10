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
	, CameraTranslationSpeed(10.0f)
	, CameraRotationSpeed(180.f)
	, PerspectiveFOV(45.f)
	, NearClip(0.01f)
	, FarClip(1000.0f)
	, OrthographicSize(10.0f)
	, ProjectionType(PROJ_Perspective)
	, FocalPoint(FVector::ZeroVector)
	, MousePosition(FVector::ZeroVector)
	, bIsMovingCamera(false)
{
	RecalculateProjection();
}

void FEditorCamera::OnUpdate(float DeltaTime)
{
	NE_PROFILE_FUNCTION();


	FVector CurrentMousePosition = FApplicationInput::GetMousePos();
	const FVector DeltaMouse = CurrentMousePosition - MousePosition;
	MousePosition = CurrentMousePosition;
	const bool LAlt = FApplicationInput::IsKeyPressed(EKeyCode::NE_KEY_LEFT_ALT);

	bIsMovingCamera = false;
	if (FApplicationInput::IsMouseButtonPressed(EKeyCode::NE_MOUSE_BUTTON_MIDDLE))
	{
		// Pan the camera based on the focal length
	}
	else if (FApplicationInput::IsMouseButtonPressed(EKeyCode::NE_MOUSE_BUTTON_LEFT))
	{
		if (LAlt)
		{
			// Rotate around focal

		}
	}
	else if (FApplicationInput::IsMouseButtonPressed(EKeyCode::NE_MOUSE_BUTTON_RIGHT))
	{
		if (LAlt)
		{
			// Move Camera forward
		}
		else
		{
			FVector CameraDelta(FVector::ZeroVector);

			// First Person Camera Movement
			CameraTranslationSpeed = FMath::Max(CameraTranslationSpeed, 1.0f);
			FVector ForwardVec = CameraTransform.GetUnitAxis(EAxis::Z);
			FVector RightVec = CameraTransform.GetUnitAxis(EAxis::X);
			FVector UpVec = CameraTransform.GetUnitAxis(EAxis::Y);

			float MovementSpeed = DeltaTime * CameraTranslationSpeed;
			if (FApplicationInput::IsKeyPressed(NE_KEY_W))
			{
				CameraDelta += ForwardVec * MovementSpeed;
			}
			else if (FApplicationInput::IsKeyPressed(NE_KEY_S))
			{
				CameraDelta -= ForwardVec * MovementSpeed;
			}
			if (FApplicationInput::IsKeyPressed(NE_KEY_D))
			{
				CameraDelta += RightVec * MovementSpeed;
			}
			else if (FApplicationInput::IsKeyPressed(NE_KEY_A))
			{
				CameraDelta -= RightVec * MovementSpeed;
			}
			if (FApplicationInput::IsKeyPressed(NE_KEY_Q))
			{
				CameraDelta -= UpVec * MovementSpeed;
			}
			else if (FApplicationInput::IsKeyPressed(NE_KEY_E))
			{
				CameraDelta += UpVec * MovementSpeed;
			}

			FQuat CurrentRotation = CameraTransform.GetRotation();
			CurrentRotation = FQuat(FVector::UpVector, FMath::DegreesToRadian(DeltaMouse.X) * .3f) * FQuat(RightVec, FMath::DegreesToRadian(DeltaMouse.Y) * .3f) * CurrentRotation;
			CameraTransform.SetRotation(CurrentRotation);
			CameraTransform.Translate(CameraDelta);
			bIsMovingCamera = true;
		}
	}




}

void FEditorCamera::OnEvent(IEvent& InEvent)
{
	NE_PROFILE_FUNCTION();

	switch (InEvent.GetEventType())
	{
		//case EventType::WindowResize:	InEvent.bHandled |= OnWindowResized(static_cast<FWindowResizeEvent&>(InEvent)); break;
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

	ScrollOffset += InEvent.GetYOffset();
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
