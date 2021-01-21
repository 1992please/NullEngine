#pragma once
#include "CoreTypes.h"
#include "IDelegateInstance.h"
#include "Templates/TypeTraits.h"
#include "Logging/Logger.h"

/**
 * Implements a delegate binding for C++ member functions.
 */
template <bool bConst, class UserClass, typename FuncType, typename... VarTypes>
class TBaseRawMethodDelegateInstance;

template <bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseRawMethodDelegateInstance<bConst, UserClass, WrappedRetValType(ParamTypes...), VarTypes...> : public IDelegateInstance<WrappedRetValType(ParamTypes...)>
{
private:
	typedef IDelegateInstance<WrappedRetValType(ParamTypes...)> Super;
	typedef typename Super::RetValType RetValType;
	typedef TBaseRawMethodDelegateInstance<bConst, UserClass, WrappedRetValType(ParamTypes...), VarTypes...> UnwrappedThisType;
public:
	typedef typename TMemFunPtrType<bConst, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type FMethodPtr;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject An arbitrary object (templated) that hosts the member function.
	 * @param InMethodPtr C++ member function pointer for the method to bind.
	 */
	TBaseRawMethodDelegateInstance(UserClass* InUserObject, FMethodPtr InMethodPtr)
		: UserObject(InUserObject)
		, MethodPtr(InMethodPtr)
	{
		// Non-expirable delegates must always have a non-null object pointer on creation (otherwise they could never execute.)
		check(InUserObject != nullptr && MethodPtr != nullptr);
	}

	RetValType Execute(ParamTypes... Params) const final
	{
		using MutableUserClass = typename TRemoveConst<UserClass>::Type;

		// Safely remove const to work around a compiler issue with instantiating template permutations for 
		// overloaded functions that take a function pointer typedef as a member of a templated class.  In
		// all cases where this code is actually invoked, the UserClass will already be a const pointer.
		MutableUserClass* MutableUserObject = const_cast<MutableUserClass*>(UserObject);

		NE_ASSERT(MethodPtr != nullptr);

		return (*MutableUserObject->*MethodPtr)(Params...);
	}

protected:

	// Pointer to the user's class which contains a method we would like to call.
	UserClass* UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;
};


/**
 * Implements a delegate binding for regular C++ functions.
 */
template <typename FuncType, typename... VarTypes>
class TBaseStaticDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseStaticDelegateInstance<WrappedRetValType(ParamTypes...), VarTypes...> : public IDelegateInstance<WrappedRetValType(ParamTypes...)>
{
private:
	typedef IDelegateInstance<WrappedRetValType(ParamTypes...)> Super;
	typedef typename Super::RetValType RetValType;
	typedef TBaseStaticDelegateInstance<WrappedRetValType(ParamTypes...), VarTypes...> UnwrappedThisType;
public:
	typedef RetValType(*FFuncPtr)(ParamTypes..., VarTypes...);

	TBaseStaticDelegateInstance(FFuncPtr InStaticFuncPtr)
		: StaticFuncPtr(InStaticFuncPtr)
	{
		NE_ASSERT(StaticFuncPtr != nullptr);
	}

	RetValType Execute(ParamTypes... Params) const final
	{
		NE_ASSERT(StaticFuncPtr != nullptr);

		return (*StaticFuncPtr)(Params...);
	}
private:

	// C++ function pointer.
	FFuncPtr StaticFuncPtr;
};


/**
 * Implements a delegate binding for C++ functors, e.g. lambdas.
 */
template <typename FuncType, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance<WrappedRetValType(ParamTypes...), FunctorType, VarTypes...> : public IDelegateInstance<WrappedRetValType(ParamTypes...)>
{
private:
	typedef IDelegateInstance<WrappedRetValType(ParamTypes...)> Super;
	typedef typename Super::RetValType RetValType;
	typedef TBaseFunctorDelegateInstance<WrappedRetValType(ParamTypes...), FunctorType, VarTypes...> UnwrappedThisType;
public:
	TBaseFunctorDelegateInstance(const FunctorType& InFunctor)
		: Functor(InFunctor)
	{
	}

	TBaseFunctorDelegateInstance(FunctorType&& InFunctor)
		: Functor(MoveTemp(InFunctor))
	{
	}

	RetValType Execute(ParamTypes... Params) const final
	{	
		return Functor(Params...);
	}
private:
	// C++ functor
	// We make this mutable to allow mutable lambdas to be bound and executed.  We don't really want to
	// model the Functor as being a direct subobject of the delegate (which would maintain transivity of
	// const - because the binding doesn't affect the substitutability of a copied delegate.
	mutable typename TRemoveConst<FunctorType>::Type Functor;
};