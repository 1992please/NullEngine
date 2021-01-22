#pragma once
#include "CoreTypes.h"

// forward declaration of TDelegate;
class FDelegateBase;

struct IDelegateInstance
{
	virtual ~IDelegateInstance() = default;

	virtual bool HasSameObject(const void* InUserObject) const = 0;
};

template <typename FuncType>
struct IBaseDelegateInstance;

template <typename InRetType, typename... ArgTypes>
struct IBaseDelegateInstance<InRetType(ArgTypes...)> : IDelegateInstance
{
	typedef InRetType RetType;

	virtual RetType Execute(ArgTypes...) const = 0;

	virtual void CreateCopy(FDelegateBase& Base) = 0;
};

template <bool Const, typename Class, typename FuncType>
struct TMemFunPtrType;

template <typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<false, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...);
};

template <typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<true, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...) const;
};