#pragma once
#include "Core/Math/Matrix.h"

class FCameraComponent
{
public:
	enum EProjectionType
	{
		PROJ_Perspective = 0,
		PROJ_Orthographic = 1
	};

	FCameraComponent();

	void SetOrthographic(float InSize, float InNear, float InFar);
	void SetPerspective(float InFov, float InNear, float InFar);

	void SetViewportSize(uint32 Width, uint32 Height);

	FORCEINLINE float GetPerspectiveFOV() const { return PerspectiveFOV; }
	FORCEINLINE void SetPerspectiveFOV(float val) { PerspectiveFOV = val; RecalculateProjection(); }
	FORCEINLINE float GetPerspectiveNear() const { return PerspectiveNear; }
	FORCEINLINE void SetPerspectiveNear(float val) { PerspectiveNear = val; RecalculateProjection(); }
	FORCEINLINE float GetPerspectiveFar() const { return PerspectiveFar; }
	FORCEINLINE void SetPrespectiveFar(float val) { PerspectiveFar = val; RecalculateProjection(); }

	FORCEINLINE float GetOrthographicSize() const { return OrthographicSize; }
	FORCEINLINE void SetOrthographicSize(float val) { OrthographicSize = val; RecalculateProjection(); }
	FORCEINLINE float GetOrthographicNear() const { return OrthographicNear; }
	FORCEINLINE void SetOrthographicNear(float val) { OrthographicNear = val; RecalculateProjection(); }
	FORCEINLINE float GetOrthographicFar() const { return OrthographicFar; }
	FORCEINLINE void SetOrthographicFar(float val) { OrthographicFar = val; RecalculateProjection(); }

	FORCEINLINE EProjectionType GetProjectionType() const { return ProjectionType; }
	FORCEINLINE void SetProjectionType(EProjectionType val) { ProjectionType = val; RecalculateProjection(); }

	FORCEINLINE const FMatrix& GetProjectionMatrix() const { return ProjectionMatrix; }

private:
	void RecalculateProjection();
private:
	FMatrix ProjectionMatrix;

	float AspectRatio;

	// in case of perespective
	float PerspectiveFOV;
	float PerspectiveNear;
	float PerspectiveFar;

	// in case of orthogonal
	float OrthographicSize;
	float OrthographicNear;
	float OrthographicFar;

	EProjectionType ProjectionType;

public:
	bool bPrimaryCamera;
	bool bFixedAspectRatio;
};