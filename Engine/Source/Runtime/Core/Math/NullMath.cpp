#include "NullPCH.h"
#include "NullMath.h"

const FVector FVector::ZeroVector(0.0f, 0.0f, 0.0f);
const FVector FVector::OneVector(1.0f, 1.0f, 1.0f);
const FVector FVector::UpVector(0.0f, 1.0f, 0.0f);
const FVector FVector::DownVector(0.0f, -1.0f, 0.0f);
const FVector FVector::ForwardVector(0.0f, 0.0f, 1.0f);
const FVector FVector::BackwardVector(0.0f, 0.0f, -1.0f);
const FVector FVector::RightVector(1.0f, 0.0f, 0.0f);
const FVector FVector::LeftVector(-1.0f, 0.0f, 0.0f);

const FVector2 FVector2::ZeroVector(0.0f, 0.0f); 
const FVector4 FVector4::ZeroVector(0.0f, 0.0f, 0.0f, 0.0f);