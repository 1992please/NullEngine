#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Transform.h"

class FEditorCamera
{
public:
	enum EProjectionType
	{
		PROJ_Perspective = 0,
		PROJ_Orthographic = 1
	};

	FEditorCamera(float InAspectRatio);

	void OnUpdate(float DeltaTime);
	void OnEvent(class IEvent& InEvent);

	void OnResize(float InWidth, float InHeight);
	FMatrix GetViewMatrix() const { return CameraTransform.ToInvMatrixNoScale(); }
	const FMatrix& GetProjectionMatrix() const { return ProjectionMatrix; }
private:
	bool OnMouseScrolled(class FMouseScrolledEvent& InEvent);
	bool OnWindowResized(class FWindowResizeEvent& InEvent);

	void RecalculateProjection();
private:
	EProjectionType ProjectionType;

	float AspectRatio;

	// in case of perespective
	float PerspectiveFOV;

	// in case of orthogonal
	float OrthographicSize;

	float NearClip;
	float FarClip;

	float CameraTranslationSpeed;
	float CameraRotationSpeed;

	FTransform CameraTransform;
	FMatrix ProjectionMatrix;
};