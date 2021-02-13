#pragma once
#include "Core/CoreTypes.h"
#include <math.h>
#include <intrin.h>

/*-----------------------------------------------------------------------------
	Floating point constants.
-----------------------------------------------------------------------------*/

#undef  PI
#define PI 					(3.1415926535897932f)	/* Extra digits if needed: 3.1415926535897932384626433832795f */
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536f)
#define UE_GOLDEN_RATIO		(1.6180339887498948482045868343656381f)	/* Also known as divine proportion, golden mean, or golden section - related to the Fibonacci Sequence = (1 + sqrt(5)) / 2 */

// Copied from float.h
#define MAX_FLT 3.402823466e+38F

// Aux constants.
#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)

// Common square roots
#define UE_SQRT_2		(1.4142135623730950488016887242097f)
#define UE_SQRT_3		(1.7320508075688772935274463415059f)
#define UE_INV_SQRT_2	(0.70710678118654752440084436210485f)
#define UE_INV_SQRT_3	(0.57735026918962576450914878050196f)
#define UE_HALF_SQRT_2	(0.70710678118654752440084436210485f)
#define UE_HALF_SQRT_3	(0.86602540378443864676372317075294f)


// Magic numbers for numerical precision.
#define DELTA			(0.00001f)

/**
 * Lengths of normalized vectors (These are half their maximum values
 * to assure that dot products with normalized vectors don't overflow).
 */
#define FLOAT_NORMAL_THRESH				(0.0001f)

 //
 // Magic numbers for numerical precision.
 //
#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.00002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
#define THRESH_UVS_ARE_SAME			    (0.0009765625f)/* Two UV are same if within this threshold (1.0f/1024f) */
													/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
													/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_NORMALS_ARE_PARALLEL		(0.999845f)	/* Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
#define THRESH_NORMALS_ARE_ORTHOGONAL	(0.017455f)	/* Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */

class FMath
{
public:
	// Returns e^Value
	static FORCEINLINE float Exp(float Value) { return expf(Value); }
	// Returns 2^Value
	static FORCEINLINE float Exp2(float Value) { return powf(2.f, Value); /*exp2f(Value);*/ }
	static FORCEINLINE float Loge(float Value) { return logf(Value); }

	static inline float RadianToDegrees(float RadianAngle){return RadianAngle * 180.0f / PI;}
	static inline float DegreesToRadian(float DegreesAngle){return DegreesAngle * PI / 180.0f;}

	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Sinh(float Value) { return sinhf(Value); }
	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float Atan(float Value) { return atanf(Value); }
	static float Atan2(float Y, float X);
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float Pow(float A, float B) { return powf(A, B); }
	/** Multiples value by itself */
	template< class T >
	static FORCEINLINE T Square(const T A){	return A * A;}
	/**
	 * Converts a float to an integer with truncation towards zero.
	 * @param F		Floating point value to convert
	 * @return		Truncated integer.
	 */
	static FORCEINLINE int32 TruncToInt(float F)
	{
		return (int32)F;
	}
	/**
	 * Converts a float to an integer value with truncation towards zero.
	 * @param F		Floating point value to convert
	 * @return		Truncated integer value.
	 */
	 /**
  *	Checks if two floating point numbers are nearly equal.
  *	@param A				First number to compare
  *	@param B				Second number to compare
  *	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
  *	@return					true if A and B are nearly equal
  */
	static FORCEINLINE bool IsNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<float>(A - B) <= ErrorTolerance;
	}

	/**
	 *	Checks if two floating point numbers are nearly equal.
	 *	@param A				First number to compare
	 *	@param B				Second number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering them as 'nearly equal'
	 *	@return					true if A and B are nearly equal
	 */
	static FORCEINLINE bool IsNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<double>(A - B) <= ErrorTolerance;
	}

	static FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	/** Clamps X to be between Min and Max, inclusive */
	template< class T >
	static FORCEINLINE T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	/**
	 * Converts a float to a nearest less or equal integer.
	 * @param F		Floating point value to convert
	 * @return		An integer less or equal to 'F'.
	 */
	static FORCEINLINE int32 FloorToInt(float F)
	{
		return TruncToInt(floorf(F));
	}

	/**
	* Converts a float to the nearest less or equal integer.
	* @param F		Floating point value to convert
	* @return		An integer less or equal to 'F'.
	*/
	static FORCEINLINE float FloorToFloat(float F)
	{
		return floorf(F);
	}
	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value);
	static float Fmod(float X, float Y);
	static FORCEINLINE float InvSqrt(float F);

	// Note:  We use FASTASIN_HALF_PI instead of HALF_PI inside of FastASin(), since it was the value that accompanied the minimax coefficients below.
// It is important to use exactly the same value in all places inside this function to ensure that FastASin(0.0f) == 0.0f.
// For comparison:
//		HALF_PI				== 1.57079632679f == 0x3fC90FDB
//		FASTASIN_HALF_PI	== 1.5707963050f  == 0x3fC90FDA
#define FASTASIN_HALF_PI (1.5707963050f)
	/**
	* Computes the ASin of a scalar value.
	*
	* @param Value  input angle
	* @return ASin of Value
	*/
	static FORCEINLINE float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		bool nonnegative = (Value >= 0.0f);
		float x = FMath::Abs(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		float root = FMath::Sqrt(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		result *= root;  // acos(|x|)
		// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
	}
#undef FASTASIN_HALF_PI

	/** Divides two integers and rounds up */
	template <class T>
	static FORCEINLINE T DivideAndRoundUp(T Dividend, T Divisor)
	{
		return (Dividend + Divisor - 1) / Divisor;
	}

	static FORCEINLINE uint32 CeilLogTwo(uint32 Arg)
	{
		int32 Bitmask = ((int32)(CountLeadingZeros(Arg) << 26)) >> 31;
		return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
	}

	static FORCEINLINE uint32 RoundUpToPowerOfTwo(uint32 Arg)
	{
		return 1 << CeilLogTwo(Arg);
	}

	template <typename T>
	static FORCEINLINE bool IsPowerOfTwo(T Value)
	{
		return ((Value & (Value - 1)) == (T)0);
	}

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanReverse64)

	static FORCEINLINE uint32 FloorLog2(uint32 Value)
	{
		// Use BSR to return the log2 of the integer
		unsigned long Log2;
		if (_BitScanReverse(&Log2, Value) != 0)
		{
			return Log2;
		}

		return 0;
	}


	static FORCEINLINE uint32 CountLeadingZeros(uint32 Value)
	{
		unsigned long Log2;
		_BitScanReverse64(&Log2, (uint64(Value) << 1) | 1);
		return 32 - Log2;
	}

	static FORCEINLINE uint32 CountTrailingZeros(uint32 Value)
	{
		if (Value == 0)
		{
			return 32;
		}
		unsigned long BitIndex;	// 0-based, where the LSB is 0 and MSB is 31
		_BitScanForward(&BitIndex, Value);	// Scans from LSB to MSB
		return BitIndex;
	}

	/** Computes absolute value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	/** Returns 1, 0, or -1 depending on relation of T to 0 */
	template< class T >
	static CONSTEXPR FORCEINLINE T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** Returns higher value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	/** Returns lower value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	/** Divides two integers and rounds down */
	template <class T>
	static FORCEINLINE T DivideAndRoundDown(T Dividend, T Divisor)
	{
		return Dividend / Divisor;
	}

	/** Divides two integers and rounds to nearest */
	template <class T>
	static FORCEINLINE T DivideAndRoundNearest(T Dividend, T Divisor)
	{
		return (Dividend >= 0)
			? (Dividend + Divisor / 2) / Divisor
			: (Dividend - Divisor / 2 + 1) / Divisor;
	}

	template< class T, class U >
	static FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	// returns a number between 0 and RAND_MAX
	static FORCEINLINE int32 Rand() { return rand(); }
	static FORCEINLINE float FRand() { return (float)Rand() / (float)RAND_MAX; }
	static FORCEINLINE void RandInit(int32 Seed) { srand(Seed); }
};

FORCEINLINE float FMath::InvSqrt(float F)
{
	// Performs two passes of Newton-Raphson iteration on the hardware estimate
	//    v^-0.5 = x
	// => x^2 = v^-1
	// => 1/(x^2) = v
	// => F(x) = x^-2 - v
	//    F'(x) = -2x^-3

	//    x1 = x0 - F(x0)/F'(x0)
	// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
	// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
	// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
	//
	// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
	// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

	const __m128 fOneHalf = _mm_set_ss(0.5f);
	__m128 Y0, X0, X1, X2, FOver2;
	float temp;

	Y0 = _mm_set_ss(F);
	X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)
	FOver2 = _mm_mul_ss(Y0, fOneHalf);

	// 1st Newton-Raphson iteration
	X1 = _mm_mul_ss(X0, X0);
	X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
	X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

	// 2nd Newton-Raphson iteration
	X2 = _mm_mul_ss(X1, X1);
	X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
	X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

	_mm_store_ss(&temp, X2);
	return temp;
}

FORCEINLINE void FMath::SinCos(float* ScalarSin, float* ScalarCos, float Value)
{
	// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
	float quotient = (INV_PI*0.5f)*Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float y = Value - (2.0f*PI)*quotient;

	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > HALF_PI)
	{
		y = PI - y;
		sign = -1.0f;
	}
	else if (y < -HALF_PI)
	{
		y = -PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	// 11-degree minimax approximation
	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}