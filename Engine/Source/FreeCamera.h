#pragma once
#include "Math/Matrix.h"

class FreeCamera
{
public:
	FreeCamera();

	void InitCameraProjection(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
	void MoveForward(float Distance);
	void MoveUp(float Distance);
	void MoveRight(float Distance);	
	void OnUpdate(float DeltaTime);
	void Turn(float value);
	void LookUp(float value);
	const FMatrix& GetViewMatrix(){ return ViewMat;}
	const FMatrix& GetProjectionMatrix() {return ProjectionMat; }
private:
	float HalfFOV;
	float Width;
	float Height; 
	float MinZ; 
	float MaxZ;

	bool bUpdate;

	FVector Location;
	FVector CamForward;
	FVector CamUP;
	FVector CamRight;

	float HAngle;
	float VAngle;

	FMatrix ViewMat;
	FMatrix ProjectionMat;
	void UpdateViewMatrix();
	void UpdateTargetVector();

	void UpdateProjectionMatrix();
};

