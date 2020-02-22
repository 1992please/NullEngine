#include "FreeCamera.h"
#include "Math/PrespectiveMatrix.h"
#include "Math/RotationMatrix.h"


FreeCamera::FreeCamera()
{
	HalfFOV = 30.0f;
	Width = 500;
	Height = 500;
	MinZ = 0.3f;
	MaxZ = 6000.0f;
	CamUP = FVector(0.0f, 0.0f, 1.0f);
	Location = FVector(-5.0f, 0.0f, 0.0f);
	CamForward = FVector(1.0f, 0.0f, 0.0f);
	HAngle = 0.f;
	VAngle = 0.f;
	bUpdate = true;
}

void FreeCamera::InitCameraProjection(float InHalfFOV, float InWidth, float InHeight, float InMinZ, float InMaxZ)
{
	HalfFOV = InHalfFOV;
	Width = InWidth;
	Height = InHeight;
	MinZ = InMinZ;
	MaxZ = InMaxZ;
	UpdateProjectionMatrix();
}

void FreeCamera::MoveForward(float Distance)
{
	if (Distance)
	{
		Location += CamForward * Distance;
		bUpdate = true;
	}
}

void FreeCamera::MoveUp(float Distance)
{
	if (Distance)
	{
		Location += CamUP * Distance;
		bUpdate = true;
	}
}

void FreeCamera::MoveRight(float Distance)
{
	if (Distance)
	{
		Location += CamRight * Distance;
		bUpdate = true;
	}
}

void FreeCamera::OnUpdate(float DeltaTime)
{
	if (bUpdate)
	{
		UpdateViewMatrix();
		bUpdate = false;
	}
}

void FreeCamera::Turn(float value)
{
	if (value)
	{
		HAngle += value;
		bUpdate = true;
	}
}

void FreeCamera::LookUp(float value)
{
	if (value)
	{
		VAngle += value;
		bUpdate = true;
	}
}

void FreeCamera::UpdateViewMatrix()
{
	FRotationMatrix RotMatrix(FRotator(VAngle, HAngle, 0.0f));
	//CamForward = FVector(RotMatrix[0][0], RotMatrix[1][0], RotMatrix[2][0]);
	CamForward = RotMatrix.TransformVector(FVector::ForwardVector);
	const FVector ZAxis = CamForward;
	const FVector XAxis = (CamUP ^ ZAxis).GetSafeNormal();
	const FVector YAxis = ZAxis ^ XAxis;

	CamRight = XAxis;

	for (uint32 RowIndex = 0; RowIndex < 3; RowIndex++)
	{
		ViewMat[RowIndex][0] = (&XAxis.X)[RowIndex];
		ViewMat[RowIndex][1] = (&YAxis.X)[RowIndex];
		ViewMat[RowIndex][2] = (&ZAxis.X)[RowIndex];
		ViewMat[RowIndex][3] = 0.0f;
	}
	ViewMat[3][0] = -Location | XAxis;
	ViewMat[3][1] = -Location | YAxis;
	ViewMat[3][2] = -Location | ZAxis;
	ViewMat[3][3] = 1.0f;

	//ViewMat *= RotMatrix;
}

void FreeCamera::UpdateTargetVector()
{

}

void FreeCamera::UpdateProjectionMatrix()
{
	ProjectionMat = FPerspectiveMatrix(FMath::DegreesToRadian(HalfFOV), Width, Height, MinZ, MaxZ);
}

