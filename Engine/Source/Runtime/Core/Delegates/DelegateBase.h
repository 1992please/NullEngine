#pragma once
#include "CoreTypes.h"
#include "Templates/TypeCompatibleBytes.h"
#include "Templates/NullTemplate.h"
#include "Templates/IsConst.h"
#include "Memory/NullMemory.h"
#include "Math/MathUtility.h"

typedef TAlignedBytes<16, 16> FAlignedInlineDelegateType;

class FDelegateBase
{

public:
	/**
 * Creates and initializes a new instance.
 *
 * @param InDelegateInstance The delegate instance to assign.
 */
	FORCEINLINE explicit FDelegateBase()
		: DelegateSize(0)
		, DelegateData(nullptr)
	{
	}

	FORCEINLINE ~FDelegateBase()
	{
		Unbind();
	}

	/**
	 * Move constructor.
	 */
	FORCEINLINE FDelegateBase(FDelegateBase&& Other)
	{
		MoveToEmpty(Other);
	}
	/**
	 * Move assignment.
	 */
	FDelegateBase& operator=(FDelegateBase&& Other)
	{
		Unbind();
		MoveToEmpty(Other);
		DelegateSize = Other.DelegateSize;
		Other.DelegateSize = 0;
		return *this;
	}

	/**
	 * Unbinds this delegate
	 */
	FORCEINLINE void Unbind()
	{
		if (IDelegateInstance* Ptr = GetDelegateInstanceProtected())
		{
			Ptr->~IDelegateInstance();
			ResizeAllocation(0, sizeof(FAlignedInlineDelegateType));
			DelegateSize = 0;
		}
	}


	FORCEINLINE IDelegateInstance* GetDelegateInstanceProtected() const
	{
		return DelegateSize ? (IDelegateInstance*)DelegateData : nullptr;
	}

private:
	FORCEINLINE void MoveToEmpty(FDelegateBase& Other)
	{
		NE_ASSERT(this != &Other);

		if (DelegateData)
		{
			FMemory::Free(DelegateData);
		}

		DelegateData = Other.DelegateData;
		Other.DelegateData = nullptr;

		DelegateSize = Other.DelegateSize;
		Other.DelegateSize = 0;
	}

	FORCEINLINE void ResizeAllocation(int32 NumElements, size_t NumBytesPerElement)
	{
		// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
		if (DelegateData || NumElements)
		{
			//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
			DelegateData = (FAlignedInlineDelegateType*)FMemory::Realloc(DelegateData, NumElements*NumBytesPerElement);
		}
	}

	friend void* operator new(size_t Size, FDelegateBase& Base);

	void* Allocate(int32 Size)
	{
		if (IDelegateInstance* CurrentInstance = GetDelegateInstanceProtected())
		{
			CurrentInstance->~IDelegateInstance();
		}

		int32 NewDelegateSize = FMath::DivideAndRoundUp(Size, (int32)sizeof(FAlignedInlineDelegateType));
		if (DelegateSize != NewDelegateSize)
		{
			ResizeAllocation(NewDelegateSize, sizeof(FAlignedInlineDelegateType));
			DelegateSize = NewDelegateSize;
		}

		return DelegateData;
	}

	FAlignedInlineDelegateType* DelegateData;
	int32 DelegateSize;
};

template <typename DelegateSignature>
class TDelegate
{
	static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
};

template <typename InRetValType, typename... ParamTypes>
class TDelegate<InRetValType(ParamTypes...)> : public FDelegateBase
{
	typedef IBaseDelegateInstance<InRetValType(ParamTypes...)> DelegateInstanceType;
public:

	/** Type definition for return value type. */
	typedef InRetValType RetValType;

public:

	/**
	 * Default constructor
	 */
	FORCEINLINE TDelegate()
	{
	}

	/**
	 * Destructor.
	 */
	FORCEINLINE ~TDelegate()
	{
		Unbind();
	}

	/**
	 * Move constructor.
	 *
	 * @param Other The delegate object to move from.
	 */
	FORCEINLINE TDelegate(TDelegate&& Other)
	{
		*this = MoveTemp(Other);
	}

	/**
	 * Creates and initializes a new instance from an existing delegate object.
	 *
	 * @param Other The delegate object to copy from.
	 */
	FORCEINLINE TDelegate(const TDelegate& Other)
	{
		*this = Other;
	}

	/**
	 * Move assignment operator.
	 *
	 * @param	OtherDelegate	Delegate object to copy from
	 */
	FORCEINLINE TDelegate& operator=(TDelegate&& Other)
	{
		if (&Other != this)
		{
			// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
			DelegateInstanceType* OtherInstance = (DelegateInstanceType *)Other.GetDelegateInstanceProtected();

			if (OtherInstance != nullptr)
			{
				OtherInstance->CreateCopy(*this);
			}
			else
			{
				Unbind();
			}
		}

		return *this;
	}

	/**
	 * Assignment operator.
	 *
	 * @param	OtherDelegate	Delegate object to copy from
	 */
	inline TDelegate& operator=(const TDelegate& Other)
	{
		if (&Other != this)
		{
			// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
			DelegateInstanceInterfaceType* OtherInstance = Other.GetDelegateInstanceProtected();

			if (OtherInstance != nullptr)
			{
				OtherInstance->CreateCopy(*this);
			}
			else
			{
				Unbind();
			}
		}

		return *this;
	}


	/**
	 * Checks to see if the user object bound to this delegate is still valid.
	 *
	 * @return True if the user object is still valid and it's safe to execute the function call.
	 */
	FORCEINLINE bool IsBound() const
	{
		return (GetDelegateInstanceProtected() != nullptr);
	}

public:

	template <typename UserClass>
	inline void BindRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes...)>::Type InFunc)
	{
		static_assert(!TIsConst<UserClass>::Value, "Attempting to bind a delegate with a const object pointer and non-const member function.");
		TBaseRawMethodDelegateInstance<false, UserClass, RetValType(ParamTypes...)>::Create(*this, InUserObject, InFunc);
	}

	template <typename UserClass>
	inline void BindRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes...)>::Type InFunc)
	{
		TBaseRawMethodDelegateInstance<true, const UserClass, RetValType(ParamTypes...)>::Create(Result, InUserObject, InFunc);
	}

	FORCEINLINE void BindStatic(typename TBaseStaticDelegateInstance<RetValType(ParamTypes...)>::FFuncPtr InFunc)
	{
		TBaseStaticDelegateInstance<RetValType(ParamTypes...)>::Create(*this, InFunc);
	}

	template<typename FunctorType>
	FORCEINLINE void BindLambda(FunctorType&& InFunctor)
	{
		TBaseFunctorDelegateInstance<RetValType(ParamTypes...), typename TRemoveReference<FunctorType>::Type>::Create(*this, Forward<FunctorType>(InFunctor));
	}
public:

	/**
	 * Static: Creates a raw C++ pointer global function delegate
	 */
	FORCEINLINE static TDelegate<RetValType(ParamTypes...)> CreateStatic(RetValType(*InFunc)(ParamTypes...) )
	{
		TDelegate<RetValType(ParamTypes...)> Result;
		TBaseStaticDelegateInstance<RetValType(ParamTypes...)>::Create(Result, InFunc);
		return Result;
	}

	template<typename FunctorType>
	FORCEINLINE static TDelegate<RetValType(ParamTypes...)> CreateLambda(FunctorType&& InFunctor)
	{
		TDelegate<RetValType(ParamTypes...)> Result;
		TBaseFunctorDelegateInstance<RetValType(ParamTypes...), typename TRemoveReference<FunctorType>::Type>::Create(Result, Forward<FunctorType>(InFunctor));
		return Result;
	}

	/**
	 * Static: Creates a raw C++ pointer member function delegate.
	 *
	 * Raw pointer doesn't use any sort of reference, so may be unsafe to call if the object was
	 * deleted out from underneath your delegate. Be careful when calling Execute()!
	 */
	template <typename UserClass>
	FORCEINLINE static TDelegate<RetValType(ParamTypes...)> CreateRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes...)>::Type InFunc)
	{
		static_assert(!TIsConst<UserClass>::Value, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		TDelegate<RetValType(ParamTypes...)> Result;
		TBaseRawMethodDelegateInstance<false, UserClass, RetValType(ParamTypes...)>::Create(Result, InUserObject, InFunc);
		return Result;
	}
	template <typename UserClass>
	FORCEINLINE static TDelegate<RetValType(ParamTypes...)> CreateRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes...)>::Type InFunc)
	{
		TDelegate<RetValType(ParamTypes...)> Result;
		TBaseRawMethodDelegateInstance<true, const UserClass, RetValType(ParamTypes...)>::Create(Result, InUserObject, InFunc);
		return Result;
	}


	FORCEINLINE RetValType Execute(ParamTypes... Params) const
	{
		DelegateInstanceType* LocalDelegateInstance = (DelegateInstanceType*)GetDelegateInstanceProtected();

		// If this assert goes off, Execute() was called before a function was bound to the delegate.
		// Consider using ExecuteIfSafe() instead.
		NE_ASSERT(LocalDelegateInstance != nullptr);

		return LocalDelegateInstance->Execute(Params...);
	}


};

inline void* operator new(size_t Size, FDelegateBase& Base)
{
	return Base.Allocate((int32)Size);
}