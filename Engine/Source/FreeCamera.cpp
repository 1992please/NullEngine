#include "FreeCamera.h"


static FMatrix YawPitchRoll
(
	float const yaw,
	float const pitch,
	float const roll
)
{
	float tmp_ch = FMath::Cos(yaw);
	float tmp_sh = FMath::Sin(yaw);
	float tmp_cp = FMath::Cos(pitch);
	float tmp_sp = FMath::Sin(pitch);
	float tmp_cb = FMath::Cos(roll);
	float tmp_sb = FMath::Sin(roll);

	FMatrix Result;
	Result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
	Result[0][1] = tmp_sb * tmp_cp;
	Result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
	Result[0][3] = 0;
	Result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
	Result[1][1] = tmp_cb * tmp_cp;
	Result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
	Result[1][3] = 0;
	Result[2][0] = tmp_sh * tmp_cp;
	Result[2][1] = -tmp_sp;
	Result[2][2] = tmp_ch * tmp_cp;
	Result[2][3] = 0;
	Result[3][0] = 0;
	Result[3][1] = 0;
	Result[3][2] = 0;
	Result[3][3] = 1;
	return Result;
}


FreeCamera::FreeCamera() :
	mPosition(FVector(0)),
	mYaw(0),
	mRoll(0),
	mPitch(0),
	mZNear(0.1f),
	mZFar(1000.0f),
	mFOV(45.0f)
{
	mTranslation = FVector(0);
	mSpeed = 2.0f;
}


FreeCamera::~FreeCamera()
{
}

void FreeCamera::Update()
{
	//mPosition += mTranslation;
	//mTranslation = glm::vec3(0.0f);

	//// we need to find a better function
	//FMatrix R = YawPitchRoll(mYaw, mPitch, mRoll);

	//mLook = FVector(R * FVector(0, 0, 1, 0));
	//mUp = FVector(R * FVector(0, 1, 0, 0));
	//mRight = FVector::CrossProduct(mLook, mUp);

	//glm::vec3 target = mPosition + mLook;

	//mV = glm::lookAt(mPosition, target, mUp);
}

void FreeCamera::Walk(const float dt)
{
	mTranslation += (mLook * mSpeed * dt);
}

void FreeCamera::Strafe(const float dt)
{
	mTranslation += (mRight * mSpeed * dt);
}

void FreeCamera::Lift(const float dt)
{
	mTranslation += (mUp * mSpeed * dt);
}

void FreeCamera::SetTranslation(const FVector& t)
{
	mTranslation = t;
}

void FreeCamera::SetSpeed(const float speed)
{
	mSpeed = speed;
}


void FreeCamera ::Rotate(const float yaw, const float pitch, const float roll)
{
	mYaw += FMath::DegreesToRadian(yaw);
	mPitch += FMath::DegreesToRadian(pitch);
	mRoll += FMath::DegreesToRadian(roll);
}


void FreeCamera::SetupProjection(const float aspectRatio)
{
	mAspectRatio = aspectRatio;

	float q = 1.0f / FMath::Tan(FMath::DegreesToRadian(0.5f * mFOV));
	float A = q / mAspectRatio;
	float B = (mZNear + mZFar) / (mZNear - mZFar);
	float C = (2.0f * mZNear * mZFar) / (mZNear - mZFar);

	mP[0][0] = A;
	mP[1][1] = q;
	mP[2][2] = B;
	mP[2][3] = -1.0f;
	mP[3][2] = C;
}

void FreeCamera::SetupProjection(const float fovy, const float aspectRatio, const float InNear /*= 0.1f*/, const float InFar /*= 1000.f*/)
{
	mZNear = InNear;
	mZFar = InFar;
	mFOV = fovy;
	mAspectRatio = aspectRatio;
	SetupProjection(aspectRatio);
}

void FreeCamera::SetRotation(const float yaw, const float pitch, const float roll)
{
	mYaw = FMath::DegreesToRadian(yaw);
	mPitch = FMath::DegreesToRadian(pitch);
	mRoll = FMath::DegreesToRadian(roll);
}

void FreeCamera::SetPosition(const FVector& v)
{
	mPosition = v;
}

void FreeCamera::SetFOV(const float fov)
{
	mFOV = fov;
}
