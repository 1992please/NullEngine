#include "Vector.h"
#include "Vector4.h"

const FVector FVector::ZeroVector(0.0f, 0.0f, 0.0f);
const FVector FVector::OneVector(1.0f, 1.0f, 1.0f);
const FVector FVector::UpVector(0.0f, 0.0f, 1.0f);
const FVector FVector::DownVector(0.0f, 0.0f, -1.0f);
const FVector FVector::ForwardVector(1.0f, 0.0f, 0.0f);
const FVector FVector::BackwardVector(-1.0f, 0.0f, 0.0f);
const FVector FVector::RightVector(0.0f, 1.0f, 0.0f);
const FVector FVector::LeftVector(0.0f, -1.0f, 0.0f);

FVector::FVector(struct FVector4 V) : FVector(V.X, V.Y, V.Z) {}


