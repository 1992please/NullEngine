#pragma once
#include "Matrix.h"
/** Combined rotation and translation matrix */
class FRotationTranslationMatrix
	: public FMatrix
{
public:
	FORCEINLINE FRotationTranslationMatrix(const FRotator& Rot, const FVector& Origin)
	{
		//float SP, SY, SR;
		//float CP, CY, CR;
		//FMath::SinCos(&SP, &CP, FMath::DegreesToRadian(Rot.Pitch));
		//FMath::SinCos(&SY, &CY, FMath::DegreesToRadian(Rot.Yaw));
		//FMath::SinCos(&SR, &CR, FMath::DegreesToRadian(Rot.Roll));
		const VectorRegister Angles = MakeVectorRegister(Rot.Pitch, Rot.Yaw, Rot.Roll, 0.0f);
		const VectorRegister HalfAngles = VectorMultiply(Angles, GlobalVectorConstants::DEG_TO_RAD);

		union { VectorRegister v; float f[4]; } SinAngles, CosAngles;
		VectorSinCos(&SinAngles.v, &CosAngles.v, &HalfAngles);

		const float	SinPitch = SinAngles.f[0];
		const float	SinYaw = SinAngles.f[1];
		const float	SinRoll = SinAngles.f[2];
		const float	CosPitch = CosAngles.f[0];
		const float	CosYaw = CosAngles.f[1];
		const float	CosRoll = CosAngles.f[2];

		M[0][0] = CosYaw * CosRoll;
		M[0][1] = -CosYaw * SinRoll;
		M[0][2] = SinYaw;
		M[0][3] = 0.f;

		M[1][0] = -(SinPitch * SinYaw * CosRoll - CosPitch * SinRoll);
		M[1][1] = SinPitch * SinYaw * SinRoll + CosPitch * CosRoll;
		M[1][2] = -SinPitch * CosYaw;
		M[1][3] = 0.f;

		M[2][0] = CosPitch * SinYaw * CosRoll + SinPitch * SinRoll;
		M[2][1] = CosRoll * SinPitch - CosPitch * SinYaw * SinRoll;
		M[2][2] = CosPitch * CosYaw;
		M[2][3] = 0.f;

		M[3][0] = Origin.X;
		M[3][1] = Origin.Y;
		M[3][2] = Origin.Z;
		M[3][3] = 1.f;
	}
};

//M[0][0] = CP * CY;
//M[0][1] = CP * SY;
//M[0][2] = SP;
//M[0][3] = 0.f;
//
//M[1][0] = SR * SP * CY - CR * SY;
//M[1][1] = SR * SP * SY + CR * CY;
//M[1][2] = -SR * CP;
//M[1][3] = 0.f;
//
//M[2][0] = -(CR * SP * CY + SR * SY);
//M[2][1] = CY * SR - CR * SP * SY;
//M[2][2] = CR * CP;
//M[2][3] = 0.f;