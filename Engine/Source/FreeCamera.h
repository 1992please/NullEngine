#pragma once
#include "EngineBase.h"
#include "Math/Matrix.h"

class FreeCamera
{
public:
	FreeCamera();
	~FreeCamera();

	void Update();

	void Walk(const float dt);
	void Strafe(const float dt);
	void Lift(const float dt);

	void SetTranslation(const FVector& t);
	FVector GetTranslation() const { return mTranslation; }

	void SetSpeed(const float speed);
	const float GetSpeed() const { return mSpeed; }

	void SetupProjection(const float fovy, const float aspectRatio, const float near = 0.1f, const float far = 1000.f);
	void SetupProjection(const float aspectRatio);
	void SetRotation(const float yaw, const float pitch, const float roll);
	void SetPosition(const FVector& v);
	const FMatrix GetViewMatrix() const { return mV; }
	const FMatrix GetProjectionMatrix() const { return mP; }
	const FVector GetPosition() const { return mPosition; }
	void SetFOV(const float fov);
	const float GetFOV() const { return mFOV; }
	const float GetAspectRatio() const { return mAspectRatio; }
	void Rotate(const float yaw, const float pitch, const float roll);

protected:

	float mYaw, mPitch, mRoll, mFOV, mAspectRatio, mZNear, mZFar;
	FVector mLook;
	FVector mUp;
	FVector mRight;
	FVector mPosition;
	FMatrix mV; //view matrix
	FMatrix mP; //projection matrix

	float mSpeed;			//move speed of camera in m/s
	FVector mTranslation;
};

