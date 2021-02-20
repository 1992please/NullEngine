#include "NullPCH.h"
#include "CameraComponent.h"
#include "Core/Math/PrespectiveMatrix.h"
#include "Core/Math/OrthoMatrix.h"

FCameraComponent::FCameraComponent()
	: AspectRatio(1.7f)
	, PerspectiveFOV(45.f)
	, PerspectiveNear(.01f)
	, PerspectiveFar(1000.0f)
	, OrthographicSize(10.0f)
	, OrthographicNear(-1.0f)
	, OrthographicFar(1.0f)
	, bPrimaryCamera(true)
	, bFixedAspectRatio(false)
	, ProjectionType(PROJ_Orthographic)
{
	RecalculateProjection();
}

void FCameraComponent::SetOrthographic(float InSize, float InNear, float InFar)
{
	ProjectionType = EProjectionType::PROJ_Orthographic;
	OrthographicSize = InSize;
	OrthographicNear = InNear;
	OrthographicFar = InFar;
	RecalculateProjection();
}

void FCameraComponent::SetPerspective(float InFov, float InNear, float InFar)
{
	ProjectionType = EProjectionType::PROJ_Perspective;
	PerspectiveFOV = InFov;
	PerspectiveNear = InNear;
	PerspectiveFar = InFar;
	RecalculateProjection();
}

void FCameraComponent::SetViewportSize(uint32 Width, uint32 Height)
{
	AspectRatio = (float)Width / (float)Height;
	RecalculateProjection();
}

void FCameraComponent::RecalculateProjection()
{
	if (ProjectionType == EProjectionType::PROJ_Perspective)
	{
		ProjectionMatrix = FPerspectiveMatrix(PerspectiveFOV, AspectRatio, PerspectiveNear, PerspectiveFar);
	}
	else
	{
		const float HalfWidth = OrthographicSize * AspectRatio * 0.5f;
		const float HalfHeight = OrthographicSize * 0.5f;
		const float OrthoLeft = -HalfWidth;
		const float OrthoRight = HalfWidth;
		const float OrthoBottom = -HalfHeight;
		const float OrthoTop = HalfHeight;

		ProjectionMatrix = FOrthoMatrix(OrthoLeft, OrthoRight, OrthoTop, OrthoBottom, OrthographicNear, OrthographicFar);
	}
}
