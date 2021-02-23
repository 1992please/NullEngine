#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/ScalarRegister.h"
#include "Core/Math/Matrix.h"

struct alignas(16) FTransform
{
protected:
	VectorRegister	Rotation;
	VectorRegister	Translation;
	VectorRegister Scale3D;
public:
	static const FTransform Identity;

	FORCEINLINE FTransform()
	{
		Rotation = VectorSet_W1(VectorZero());
		Translation = VectorZero();
		Scale3D = VectorSet_W0(VectorOne());
	}

	FORCEINLINE explicit FTransform(const FVector& InTranslation)
	{
		Rotation = VectorSet_W1(VectorZero());
		Translation = MakeVectorRegister(InTranslation.X, InTranslation.Y, InTranslation.Z, 0.0f);
		Scale3D = VectorSet_W0(VectorOne());
	}

	FORCEINLINE explicit FTransform(const FQuat& InRotation)
	{
		Rotation = VectorLoadAligned(&InRotation.X);
		Translation = VectorZero();
		Scale3D = VectorSet_W0(VectorOne());
	}

	FORCEINLINE explicit FTransform(const FRotator& InRotation)
	{
		FQuat InQuatRotation = InRotation.Quaternion();
		Rotation = VectorLoadAligned(&InQuatRotation.X);
		Translation = VectorZero();
		Scale3D = VectorSet_W0(VectorOne());
	}

	FORCEINLINE FTransform(const FQuat& InRotation, const FVector& InTranslation, const FVector& InScale3D = FVector::OneVector)
	{
		Rotation = VectorLoadAligned(&InRotation.X);
		Translation = MakeVectorRegister(InTranslation.X, InTranslation.Y, InTranslation.Z, 0.0f);
		Scale3D = MakeVectorRegister(InScale3D.X, InScale3D.Y, InScale3D.Z, 0.0f);
	}

	FORCEINLINE FTransform(const VectorRegister	& InRotation, const VectorRegister& InTranslation, const VectorRegister& InScale3D)
		: Rotation(InRotation),
		Translation(InTranslation),
		Scale3D(InScale3D)
	{
	}

	FORCEINLINE FTransform(const FRotator& InRotation, const FVector& InTranslation, const FVector& InScale3D = FVector::OneVector)
	{
		FQuat InQuatRotation = InRotation.Quaternion();
		Rotation = VectorLoadAligned(&InQuatRotation.X);
		Translation = MakeVectorRegister(InTranslation.X, InTranslation.Y, InTranslation.Z, 0.0f);
		Scale3D = MakeVectorRegister(InScale3D.X, InScale3D.Y, InScale3D.Z, 0.0f);
	}

	FORCEINLINE explicit FTransform(const FMatrix& InMatrix)
	{
		SetFromMatrix(InMatrix);
	}

	FORCEINLINE FTransform(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InTranslation)
	{
		SetFromMatrix(FMatrix(InX, InY, InZ, InTranslation));
	}

	FORCEINLINE FTransform& operator=(const FTransform& Other)
	{
		Rotation = Other.Rotation;
		Translation = Other.Translation;
		Scale3D = Other.Scale3D;

		return *this;
	}

	FORCEINLINE FQuat GetRotation() const
	{
		FQuat OutRotation;
		VectorStoreAligned(Rotation, &OutRotation);
		return OutRotation;
	}

	FORCEINLINE FVector GetScale3D() const
	{
		FVector OutScale3D;
		VectorStoreFloat3(Scale3D, &OutScale3D);
		return OutScale3D;
	}

	FORCEINLINE FTransform	GetScaled(float Scale) const;
	FORCEINLINE FTransform	GetScaled(FVector Scale) const;
	FORCEINLINE FVector		GetScaledAxis(EAxis::Type InAxis) const;
	FORCEINLINE FVector		GetUnitAxis(EAxis::Type InAxis) const;

	FORCEINLINE FMatrix ToMatrixWithScale() const
	{

		FMatrix OutMatrix;
		VectorRegister DiagonalsXYZ;
		VectorRegister Adds;
		VectorRegister Subtracts;

		ToMatrixInternal(DiagonalsXYZ, Adds, Subtracts);
		const VectorRegister DiagonalsXYZ_W0 = VectorSet_W0(DiagonalsXYZ);

		// OutMatrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale.X;    // Diagonal.X
		// OutMatrix.M[0][1] = (xy2 + wz2) * Scale.X;             // Adds.X
		// OutMatrix.M[0][2] = (xz2 - wy2) * Scale.X;             // Subtracts.Z
		// OutMatrix.M[0][3] = 0.0f;                              // DiagonalsXYZ_W0.W
		const VectorRegister AddX_DC_DiagX_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 0, 0, 0, 0);
		const VectorRegister SubZ_DC_DiagW_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 2, 0, 3, 0);
		const VectorRegister Row0 = VectorShuffle(AddX_DC_DiagX_DC, SubZ_DC_DiagW_DC, 2, 0, 0, 2);

		// OutMatrix.M[1][0] = (xy2 - wz2) * Scale.Y;             // Subtracts.X
		// OutMatrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale.Y;    // Diagonal.Y
		// OutMatrix.M[1][2] = (yz2 + wx2) * Scale.Y;             // Adds.Y
		// OutMatrix.M[1][3] = 0.0f;                            // DiagonalsXYZ_W0.W
		const VectorRegister SubX_DC_DiagY_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 0, 0, 1, 0);
		const VectorRegister AddY_DC_DiagW_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 1, 0, 3, 0);
		const VectorRegister Row1 = VectorShuffle(SubX_DC_DiagY_DC, AddY_DC_DiagW_DC, 0, 2, 0, 2);

		// OutMatrix.M[2][0] = (xz2 + wy2) * Scale.Z;             // Adds.Z
		// OutMatrix.M[2][1] = (yz2 - wx2) * Scale.Z;             // Subtracts.Y
		// OutMatrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale.Z;    // Diagonals.Z
		// OutMatrix.M[2][3] = 0.0f;                              // DiagonalsXYZ_W0.W
		const VectorRegister AddZ_DC_SubY_DC = VectorShuffle(Adds, Subtracts, 2, 0, 1, 0);
		const VectorRegister Row2 = VectorShuffle(AddZ_DC_SubY_DC, DiagonalsXYZ_W0, 0, 2, 2, 3);

		VectorStoreAligned(Row0, &(OutMatrix.M[0][0]));
		VectorStoreAligned(Row1, &(OutMatrix.M[1][0]));
		VectorStoreAligned(Row2, &(OutMatrix.M[2][0]));

		// OutMatrix.M[3][0] = Translation.X;
		// OutMatrix.M[3][1] = Translation.Y;
		// OutMatrix.M[3][2] = Translation.Z;
		// OutMatrix.M[3][3] = 1.0f;
		const VectorRegister Row3 = VectorSet_W1(Translation);
		VectorStoreAligned(Row3, &(OutMatrix.M[3][0]));

		return OutMatrix;
	}

	FORCEINLINE FMatrix ToMatrixNoScale() const
	{
		FMatrix OutMatrix;
		VectorRegister DiagonalsXYZ;
		VectorRegister Adds;
		VectorRegister Subtracts;

		ToMatrixInternalNoScale(DiagonalsXYZ, Adds, Subtracts);
		const VectorRegister DiagonalsXYZ_W0 = VectorSet_W0(DiagonalsXYZ);

		// OutMatrix.M[0][0] = (1.0f - (yy2 + zz2));			// Diagonal.X
		// OutMatrix.M[0][1] = (xy2 + wz2);						// Adds.X
		// OutMatrix.M[0][2] = (xz2 - wy2);						// Subtracts.Z
		// OutMatrix.M[0][3] = 0.0f;                            // DiagonalsXYZ_W0.W
		const VectorRegister AddX_DC_DiagX_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 0, 0, 0, 0);
		const VectorRegister SubZ_DC_DiagW_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 2, 0, 3, 0);
		const VectorRegister Row0 = VectorShuffle(AddX_DC_DiagX_DC, SubZ_DC_DiagW_DC, 2, 0, 0, 2);

		// OutMatrix.M[1][0] = (xy2 - wz2);			            // Subtracts.X
		// OutMatrix.M[1][1] = (1.0f - (xx2 + zz2));		    // Diagonal.Y
		// OutMatrix.M[1][2] = (yz2 + wx2);						// Adds.Y
		// OutMatrix.M[1][3] = 0.0f;                            // DiagonalsXYZ_W0.W
		const VectorRegister SubX_DC_DiagY_DC = VectorShuffle(Subtracts, DiagonalsXYZ_W0, 0, 0, 1, 0);
		const VectorRegister AddY_DC_DiagW_DC = VectorShuffle(Adds, DiagonalsXYZ_W0, 1, 0, 3, 0);
		const VectorRegister Row1 = VectorShuffle(SubX_DC_DiagY_DC, AddY_DC_DiagW_DC, 0, 2, 0, 2);

		// OutMatrix.M[2][0] = (xz2 + wy2);						// Adds.Z
		// OutMatrix.M[2][1] = (yz2 - wx2);						// Subtracts.Y
		// OutMatrix.M[2][2] = (1.0f - (xx2 + yy2));		    // Diagonals.Z
		// OutMatrix.M[2][3] = 0.0f;                            // DiagonalsXYZ_W0.W
		const VectorRegister AddZ_DC_SubY_DC = VectorShuffle(Adds, Subtracts, 2, 0, 1, 0);
		const VectorRegister Row2 = VectorShuffle(AddZ_DC_SubY_DC, DiagonalsXYZ_W0, 0, 2, 2, 3);

		VectorStoreAligned(Row0, &(OutMatrix.M[0][0]));
		VectorStoreAligned(Row1, &(OutMatrix.M[1][0]));
		VectorStoreAligned(Row2, &(OutMatrix.M[2][0]));

		// OutMatrix.M[3][0] = Translation.X;
		// OutMatrix.M[3][1] = Translation.Y;
		// OutMatrix.M[3][2] = Translation.Z;
		// OutMatrix.M[3][3] = 1.0f;
		const VectorRegister Row3 = VectorSet_W1(Translation);
		VectorStoreAligned(Row3, &(OutMatrix.M[3][0]));

		return OutMatrix;
	}

	FORCEINLINE FMatrix ToInvMatrixNoScale() const
	{
		return InverseFastNoScale().ToMatrixNoScale();
	}

	void SetFromMatrix(const FMatrix& InMatrix)
	{
		FMatrix M = InMatrix;

		// Get the 3D scale from the matrix
		FVector InScale = M.ExtractScaling();
		Scale3D = VectorLoadFloat3_W0(&InScale);

		// If there is negative scaling going on, we handle that here
		if (InMatrix.Determinant() < 0.f)
		{
			// Assume it is along X and modify transform accordingly. 
			// It doesn't actually matter which axis we choose, the 'appearance' will be the same			
			Scale3D = VectorMultiply(Scale3D, GlobalVectorConstants::FloatMinus1_111);
			M.SetAxis(0, -M.GetScaledAxis(EAxis::X));
		}

		FQuat InRotation = FQuat(M);
		Rotation = VectorLoadAligned(&InRotation);
		FVector InTranslation = InMatrix.GetOrigin();
		Translation = VectorLoadFloat3_W0(&InTranslation);

		// Normalize rotation
		Rotation = VectorNormalizeSafe(Rotation, GlobalVectorConstants::Float0001);
	}

	FString ToString() const;

	FORCEINLINE FVector4	TransformFVector4(const FVector4& V) const;
	FORCEINLINE FVector4	TransformFVector4NoScale(const FVector4& V) const;
	FORCEINLINE FVector		TransformPosition(const FVector& V) const;
	FORCEINLINE FVector		TransformPositionNoScale(const FVector& V) const;


	/** Inverts the transform and then transforms V - correctly handles scaling in this transform. */
	FORCEINLINE FVector		InverseTransformPosition(const FVector &V) const;
	FORCEINLINE FVector		InverseTransformPositionNoScale(const FVector &V) const;
	FORCEINLINE FVector		TransformVector(const FVector& V) const;
	FORCEINLINE FVector		TransformVectorNoScale(const FVector& V) const;

	FORCEINLINE FVector InverseTransformVector(const FVector &V) const;
	FORCEINLINE FVector InverseTransformVectorNoScale(const FVector &V) const;

	FORCEINLINE FQuat TransformRotation(const FQuat& Q) const;
	FORCEINLINE FQuat InverseTransformRotation(const FQuat& Q) const;

	FORCEINLINE bool IsRotationNormalized() const
	{
		const VectorRegister TestValue = VectorAbs(VectorSubtract(VectorOne(), VectorDot4(Rotation, Rotation)));
		return !VectorAnyGreaterThan(TestValue, GlobalVectorConstants::ThreshQuatNormalized);
	}

	FORCEINLINE FVector GetPosition() const
	{
		FVector OutTranslation;
		VectorStoreFloat3(Translation, &OutTranslation);
		return OutTranslation;
	}

	FORCEINLINE FRotator Rotator() const
	{
		FQuat OutRotation;
		VectorStoreAligned(Rotation, &OutRotation);
		return OutRotation.Rotator();
	}

	FORCEINLINE float GetDeterminant() const
	{
		//TODO: vectorized version of this
		FVector4 OutScale3D;
		VectorStoreAligned(Scale3D, &OutScale3D);
		return OutScale3D.X * OutScale3D.Y * OutScale3D.Z;
	}

	FORCEINLINE void SetTranslation(const FVector& Origin)
	{
		Translation = VectorLoadFloat3_W0(&Origin);
	}

	FORCEINLINE void SetRotation(const FQuat& NewRotation)
	{
		Rotation = VectorLoadAligned(&NewRotation);
	}

	FORCEINLINE void SetScale3D(const FVector& NewScale3D)
	{
		Scale3D = VectorLoadFloat3_W0(&NewScale3D);
	}

	FORCEINLINE FTransform InverseFast() const;

	FORCEINLINE FTransform InverseFastNoScale() const;

	FORCEINLINE void  Translate(const FVector& DeltaTranslation)
	{
		Translation = VectorAdd(Translation, VectorLoadFloat3_W0(&DeltaTranslation));
	}

	FORCEINLINE void ConcatenateRotation(const FQuat& DeltaRotation)
	{
		Rotation = VectorQuaternionMultiply2(Rotation, VectorLoadAligned(&DeltaRotation));
	}

	FORCEINLINE void CopyTranslation(const FTransform& Other)
	{
		Translation = Other.Translation;
	}

	FORCEINLINE void CopyRotation(const FTransform& Other)
	{
		Rotation = Other.Rotation;
	}

	FORCEINLINE void CopyScale3D(const FTransform& Other)
	{
		Scale3D = Other.Scale3D;
	}

private:

	FORCEINLINE void ToMatrixInternal(VectorRegister& OutDiagonals, VectorRegister& OutAdds, VectorRegister& OutSubtracts) const
	{
		const VectorRegister RotationX2Y2Z2 = VectorAdd(Rotation, Rotation);	// x2, y2, z2
		const VectorRegister RotationXX2YY2ZZ2 = VectorMultiply(RotationX2Y2Z2, Rotation);	// xx2, yy2, zz2		

		// The diagonal terms of the rotation matrix are:
		//   (1 - (yy2 + zz2)) * scale
		//   (1 - (xx2 + zz2)) * scale
		//   (1 - (xx2 + yy2)) * scale
		const VectorRegister yy2_xx2_xx2 = VectorSwizzle(RotationXX2YY2ZZ2, 1, 0, 0, 0);
		const VectorRegister zz2_zz2_yy2 = VectorSwizzle(RotationXX2YY2ZZ2, 2, 2, 1, 0);
		const VectorRegister DiagonalSum = VectorAdd(yy2_xx2_xx2, zz2_zz2_yy2);
		const VectorRegister Diagonals = VectorSubtract(VectorOne(), DiagonalSum);
		OutDiagonals = VectorMultiply(Diagonals, Scale3D);

		// Grouping the non-diagonal elements in the rotation block by operations:
		//    ((x*y2,y*z2,x*z2) + (w*z2,w*x2,w*y2)) * scale.xyz and
		//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2)) * scale.yxz
		// Rearranging so the LHS and RHS are in the same order as for +
		//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2)) * scale.yxz

		// RotBase = x*y2, y*z2, x*z2
		// RotOffset = w*z2, w*x2, w*y2
		const VectorRegister x_y_x = VectorSwizzle(Rotation, 0, 1, 0, 0);
		const VectorRegister y2_z2_z2 = VectorSwizzle(RotationX2Y2Z2, 1, 2, 2, 0);
		const VectorRegister RotBase = VectorMultiply(x_y_x, y2_z2_z2);

		const VectorRegister w_w_w = VectorReplicate(Rotation, 3);
		const VectorRegister z2_x2_y2 = VectorSwizzle(RotationX2Y2Z2, 2, 0, 1, 0);
		const VectorRegister RotOffset = VectorMultiply(w_w_w, z2_x2_y2);

		// Adds = (RotBase + RotOffset)*Scale3D :  (x*y2 + w*z2) * Scale3D.X , (y*z2 + w*x2) * Scale3D.Y, (x*z2 + w*y2) * Scale3D.Z
		// Subtracts = (RotBase - RotOffset)*Scale3DYZX :  (x*y2 - w*z2) * Scale3D.Y , (y*z2 - w*x2) * Scale3D.Z, (x*z2 - w*y2) * Scale3D.X
		const VectorRegister Adds = VectorAdd(RotBase, RotOffset);
		OutAdds = VectorMultiply(Adds, Scale3D);
		const VectorRegister Scale3DYZXW = VectorSwizzle(Scale3D, 1, 2, 0, 3);
		const VectorRegister Subtracts = VectorSubtract(RotBase, RotOffset);
		OutSubtracts = VectorMultiply(Subtracts, Scale3DYZXW);
	}

	FORCEINLINE void ToMatrixInternalNoScale(VectorRegister& OutDiagonals, VectorRegister& OutAdds, VectorRegister& OutSubtracts) const
	{	
		const VectorRegister RotationX2Y2Z2 = VectorAdd(Rotation, Rotation);	// x2, y2, z2
		const VectorRegister RotationXX2YY2ZZ2 = VectorMultiply(RotationX2Y2Z2, Rotation);	// xx2, yy2, zz2		

		// The diagonal terms of the rotation matrix are:
		//   (1 - (yy2 + zz2))
		//   (1 - (xx2 + zz2))
		//   (1 - (xx2 + yy2))
		const VectorRegister yy2_xx2_xx2 = VectorSwizzle(RotationXX2YY2ZZ2, 1, 0, 0, 0);
		const VectorRegister zz2_zz2_yy2 = VectorSwizzle(RotationXX2YY2ZZ2, 2, 2, 1, 0);
		const VectorRegister DiagonalSum = VectorAdd(yy2_xx2_xx2, zz2_zz2_yy2);
		OutDiagonals = VectorSubtract(VectorOne(), DiagonalSum);

		// Grouping the non-diagonal elements in the rotation block by operations:
		//    ((x*y2,y*z2,x*z2) + (w*z2,w*x2,w*y2)) and
		//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2))
		// Rearranging so the LHS and RHS are in the same order as for +
		//    ((x*y2,y*z2,x*z2) - (w*z2,w*x2,w*y2))

		// RotBase = x*y2, y*z2, x*z2
		// RotOffset = w*z2, w*x2, w*y2
		const VectorRegister x_y_x = VectorSwizzle(Rotation, 0, 1, 0, 0);
		const VectorRegister y2_z2_z2 = VectorSwizzle(RotationX2Y2Z2, 1, 2, 2, 0);
		const VectorRegister RotBase = VectorMultiply(x_y_x, y2_z2_z2);

		const VectorRegister w_w_w = VectorReplicate(Rotation, 3);
		const VectorRegister z2_x2_y2 = VectorSwizzle(RotationX2Y2Z2, 2, 0, 1, 0);
		const VectorRegister RotOffset = VectorMultiply(w_w_w, z2_x2_y2);

		// Adds = (RotBase + RotOffset):  (x*y2 + w*z2) , (y*z2 + w*x2), (x*z2 + w*y2)
		// Subtracts = (RotBase - RotOffset) :  (x*y2 - w*z2) , (y*z2 - w*x2), (x*z2 - w*y2)
		OutAdds = VectorAdd(RotBase, RotOffset);
		OutSubtracts = VectorSubtract(RotBase, RotOffset);
	}

	static FORCEINLINE VectorRegister GetSafeScaleReciprocal(const VectorRegister& InScale, const ScalarRegister& Tolerance = ScalarRegister(GlobalVectorConstants::SmallNumber))
	{
		// SafeReciprocalScale.X = (InScale.X == 0) ? 0.f : 1/InScale.X; // same for YZW
		VectorRegister SafeReciprocalScale;

		/// VectorRegister( 1.0f / InScale.x, 1.0f / InScale.y, 1.0f / InScale.z, 1.0f / InScale.w )
		const VectorRegister ReciprocalScale = VectorReciprocalAccurate(InScale);

		//VectorRegister( Vec1.x == Vec2.x ? 0xFFFFFFFF : 0, same for yzw )
		const VectorRegister ScaleZeroMask = VectorCompareGE(Tolerance.Value, VectorAbs(InScale));

		//const VectorRegister ScaleZeroMask = VectorCompareEQ(InScale, VectorZero());

		// VectorRegister( for each bit i: Mask[i] ? Vec1[i] : Vec2[i] )
		SafeReciprocalScale = VectorSelect(ScaleZeroMask, VectorZero(), ReciprocalScale);

		return SafeReciprocalScale;
	}

};


FORCEINLINE FVector4 FTransform::TransformFVector4NoScale(const FVector4& V) const
{
	// if not, this won't work
	NE_CHECK(V.W == 0.f || V.W == 1.f);

	const VectorRegister InputVector = VectorLoadAligned(&V);

	//Transform using QST is following
	//QST(P) = Q.Rotate(S*P) + T where Q = quaternion, S = 1.0f, T = translation

	//RotatedVec = Q.Rotate(V.X, V.Y, V.Z, 0.f)
	const VectorRegister InputVectorW0 = VectorSet_W0(InputVector);
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, InputVectorW0);

	// NewVect.XYZ += Translation * W
	// NewVect.W += 1 * W
	const VectorRegister WWWW = VectorReplicate(InputVector, 3);
	const VectorRegister TranslatedVec = VectorMultiplyAdd(Translation, WWWW, RotatedVec);

	FVector4 NewVectOutput;
	VectorStoreAligned(TranslatedVec, &NewVectOutput);
	return NewVectOutput;
}

FORCEINLINE FVector4 FTransform::TransformFVector4(const FVector4& V) const
{
	// if not, this won't work
	NE_CHECK(V.W == 0.f || V.W == 1.f);

	const VectorRegister InputVector = VectorLoadAligned(&V);

	//Transform using QST is following
	//QST(P) = Q.Rotate(S*P) + T where Q = quaternion, S = scale, T = translation

	//RotatedVec = Q.Rotate(Scale*V.X, Scale*V.Y, Scale*V.Z, 0.f)
	const VectorRegister InputVectorW0 = VectorSet_W0(InputVector);
	const VectorRegister ScaledVec = VectorMultiply(Scale3D, InputVectorW0);
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, ScaledVec);

	// NewVect.XYZ += Translation * W
	// NewVect.W += 1 * W
	const VectorRegister WWWW = VectorReplicate(InputVector, 3);
	const VectorRegister TranslatedVec = VectorMultiplyAdd(Translation, WWWW, RotatedVec);

	FVector4 NewVectOutput;
	VectorStoreAligned(TranslatedVec, &NewVectOutput);
	return NewVectOutput;
}


FORCEINLINE FVector FTransform::TransformPosition(const FVector& V) const
{
	const VectorRegister InputVectorW0 = VectorLoadFloat3_W0(&V);

	//Transform using QST is following
	//QST(P) = Q.Rotate(S*P) + T where Q = quaternion, S = scale, T = translation

	//RotatedVec = Q.Rotate(Scale*V.X, Scale*V.Y, Scale*V.Z, 0.f)
	const VectorRegister ScaledVec = VectorMultiply(Scale3D, InputVectorW0);
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, ScaledVec);

	const VectorRegister TranslatedVec = VectorAdd(RotatedVec, Translation);

	FVector Result;
	VectorStoreFloat3(TranslatedVec, &Result);
	return Result;
}

FORCEINLINE FVector FTransform::TransformPositionNoScale(const FVector& V) const
{
	const VectorRegister InputVectorW0 = VectorLoadFloat3_W0(&V);

	//Transform using QST is following
	//QST(P) = Q.Rotate(S*P) + T where Q = quaternion, S = 1.0f, T = translation

	//RotatedVec = Q.Rotate(V.X, V.Y, V.Z, 0.f)
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, InputVectorW0);

	const VectorRegister TranslatedVec = VectorAdd(RotatedVec, Translation);

	FVector Result;
	VectorStoreFloat3(TranslatedVec, &Result);
	return Result;
}

FORCEINLINE FVector FTransform::TransformVector(const FVector& V) const
{
	const VectorRegister InputVectorW0 = VectorLoadFloat3_W0(&V);

	//RotatedVec = Q.Rotate(Scale*V.X, Scale*V.Y, Scale*V.Z, 0.f)
	const VectorRegister ScaledVec = VectorMultiply(Scale3D, InputVectorW0);
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, ScaledVec);

	FVector Result;
	VectorStoreFloat3(RotatedVec, &Result);
	return Result;
}

FORCEINLINE FVector FTransform::TransformVectorNoScale(const FVector& V) const
{
	const VectorRegister InputVectorW0 = VectorLoadFloat3_W0(&V);

	//RotatedVec = Q.Rotate(V.X, V.Y, V.Z, 0.f)
	const VectorRegister RotatedVec = VectorQuaternionRotateVector(Rotation, InputVectorW0);

	FVector Result;
	VectorStoreFloat3(RotatedVec, &Result);
	return Result;
}

// do backward operation when inverse, translation -> rotation -> scale
FORCEINLINE FVector FTransform::InverseTransformPosition(const FVector &V) const
{
	const VectorRegister InputVector = VectorLoadFloat3_W0(&V);

	// (V-Translation)
	const VectorRegister TranslatedVec = VectorSet_W0(VectorSubtract(InputVector, Translation));

	// ( Rotation.Inverse() * (V-Translation) )
	const VectorRegister VR = VectorQuaternionInverseRotateVector(Rotation, TranslatedVec);

	// GetSafeScaleReciprocal(Scale3D);
	const VectorRegister SafeReciprocal = GetSafeScaleReciprocal(Scale3D);

	// ( Rotation.Inverse() * (V-Translation) ) * GetSafeScaleReciprocal(Scale3D);
	const VectorRegister VResult = VectorMultiply(VR, SafeReciprocal);

	FVector Result;
	VectorStoreFloat3(VResult, &Result);
	return Result;
}

// do backward operation when inverse, translation -> rotation
FORCEINLINE FVector FTransform::InverseTransformPositionNoScale(const FVector &V) const
{
	const VectorRegister InputVector = VectorLoadFloat3_W0(&V);

	// (V-Translation)
	const VectorRegister TranslatedVec = VectorSet_W0(VectorSubtract(InputVector, Translation));

	// ( Rotation.Inverse() * (V-Translation) )
	const VectorRegister VResult = VectorQuaternionInverseRotateVector(Rotation, TranslatedVec);

	FVector Result;
	VectorStoreFloat3(VResult, &Result);
	return Result;
}


// do backward operation when inverse, translation -> rotation -> scale
FORCEINLINE FVector FTransform::InverseTransformVector(const FVector &V) const
{
	const VectorRegister InputVector = VectorLoadFloat3_W0(&V);

	// ( Rotation.Inverse() * V ) aka. FVector FQuat::operator*( const FVector& V ) const
	const VectorRegister VR = VectorQuaternionInverseRotateVector(Rotation, InputVector);

	// GetSafeScaleReciprocal(Scale3D);
	const VectorRegister SafeReciprocal = GetSafeScaleReciprocal(Scale3D);

	// ( Rotation.Inverse() * V) * GetSafeScaleReciprocal(Scale3D);
	const VectorRegister VResult = VectorMultiply(VR, SafeReciprocal);

	FVector Result;
	VectorStoreFloat3(VResult, &Result);
	return Result;
}

// do backward operation when inverse, translation -> rotation
FORCEINLINE FVector FTransform::InverseTransformVectorNoScale(const FVector &V) const
{
	VectorRegister InputVector = VectorLoadFloat3_W0(&V);

	// ( Rotation.Inverse() * V )
	VectorRegister VResult = VectorQuaternionInverseRotateVector(Rotation, InputVector);

	FVector Result;
	VectorStoreFloat3(VResult, &Result);
	return Result;
}

FORCEINLINE FQuat FTransform::TransformRotation(const FQuat& Q) const
{
	return GetRotation() * Q;
}

FORCEINLINE FQuat FTransform::InverseTransformRotation(const FQuat& Q) const
{
	return GetRotation().Inverse() * Q;
}

FORCEINLINE FTransform FTransform::InverseFast() const
{
	// Inverse QST (A) = QST (~A)
	// Since A*~A = Identity, 
	// A(P) = Q(A)*S(A)*P*-Q(A) + T(A)
	// ~A(A(P)) = Q(~A)*S(~A)*(Q(A)*S(A)*P*-Q(A) + T(A))*-Q(~A) + T(~A) = Identity
	// Q(~A)*Q(A)*S(~A)*S(A)*P*-Q(A)*-Q(~A) + Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A) = Identity
	// [Q(~A)*Q(A)]*[S(~A)*S(A)]*P*-[Q(~A)*Q(A)] + [Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A)] = I

	// Identity Q = (0, 0, 0, 1) = Q(~A)*Q(A)
	// Identity Scale = 1 = S(~A)*S(A)
	// Identity Translation = (0, 0, 0) = [Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A)]

	//	Q(~A) = Q(~A)
	//	S(~A) = 1.f/S(A)
	//	T(~A) = - (Q(~A)*S(~A)*T(A)*Q(A))	
	NE_CHECK(IsRotationNormalized());
	NE_CHECK(VectorAnyGreaterThan(VectorAbs(Scale3D), GlobalVectorConstants::SmallNumber));

	// Invert the scale
	const VectorRegister InvScale = VectorSet_W0(GetSafeScaleReciprocal(VectorSet_W1(Scale3D), ScalarRegister(GlobalVectorConstants::SmallNumber)));

	// Invert the rotation
	const VectorRegister InvRotation = VectorQuaternionInverse(Rotation);

	// Invert the translation
	const VectorRegister ScaledTranslation = VectorMultiply(InvScale, Translation);
	const VectorRegister t2 = VectorQuaternionRotateVector(InvRotation, ScaledTranslation);
	const VectorRegister InvTranslation = VectorSet_W0(VectorNegate(t2));

	return FTransform(InvRotation, InvTranslation, InvScale);
}

FORCEINLINE FTransform FTransform::InverseFastNoScale() const
{
	// Inverse QST (A) = QST (~A)
	// Since A*~A = Identity, 
	// A(P) = Q(A)*S(A)*P*-Q(A) + T(A)
	// ~A(A(P)) = Q(~A)*S(~A)*(Q(A)*S(A)*P*-Q(A) + T(A))*-Q(~A) + T(~A) = Identity
	// Q(~A)*Q(A)*S(~A)*S(A)*P*-Q(A)*-Q(~A) + Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A) = Identity
	// [Q(~A)*Q(A)]*[S(~A)*S(A)]*P*-[Q(~A)*Q(A)] + [Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A)] = I

	// Identity Q = (0, 0, 0, 1) = Q(~A)*Q(A)
	// Identity Scale = 1 = S(~A)*S(A)
	// Identity Translation = (0, 0, 0) = [Q(~A)*S(~A)*T(A)*-Q(~A) + T(~A)]

	//	Q(~A) = Q(~A)
	//	S(~A) = 1.f/S(A)
	//	T(~A) = - (Q(~A)*S(~A)*T(A)*Q(A))	
	NE_CHECK(IsRotationNormalized());
	NE_CHECK(VectorAnyGreaterThan(VectorAbs(Scale3D), GlobalVectorConstants::SmallNumber));

	// Invert the rotation
	const VectorRegister InvRotation = VectorQuaternionInverse(Rotation);

	// Invert the translation
	const VectorRegister t2 = VectorQuaternionRotateVector(InvRotation, Translation);
	const VectorRegister InvTranslation = VectorSet_W0(VectorNegate(t2));

	return FTransform(InvRotation, InvTranslation, Scale3D);
}

FORCEINLINE FTransform FTransform::GetScaled(float InScale) const
{
	FTransform A(*this);

	VectorRegister VScale = VectorLoadFloat1(&InScale);
	A.Scale3D = VectorMultiply(A.Scale3D, VScale);

	return A;
}

FORCEINLINE FTransform FTransform::GetScaled(FVector InScale) const
{
	FTransform A(*this);

	VectorRegister VScale = VectorLoadFloat3_W0(&InScale);
	A.Scale3D = VectorMultiply(A.Scale3D, VScale);

	return A;
}

FORCEINLINE FVector FTransform::GetScaledAxis(EAxis::Type InAxis) const
{
	if (InAxis == EAxis::X)
	{
		return TransformVector(FVector(1.f, 0.f, 0.f));
	}
	else if (InAxis == EAxis::Y)
	{
		return TransformVector(FVector(0.f, 1.f, 0.f));
	}

	return TransformVector(FVector(0.f, 0.f, 1.f));
}

// x = 0, y = 1, z = 2
FORCEINLINE FVector FTransform::GetUnitAxis(EAxis::Type InAxis) const
{
	if (InAxis == EAxis::X)
	{
		return TransformVectorNoScale(FVector(1.f, 0.f, 0.f));
	}
	else if (InAxis == EAxis::Y)
	{
		return TransformVectorNoScale(FVector(0.f, 1.f, 0.f));
	}

	return TransformVectorNoScale(FVector(0.f, 0.f, 1.f));
}
