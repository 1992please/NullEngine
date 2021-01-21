#pragma once
#include "CoreTypes.h"
#include "Templates/TypeCompatibleBytes.h"

template <typename FuncType>
struct IDelegateInstance;

template <typename InRetType, typename... ArgTypes>
struct IDelegateInstance<InRetType(ArgTypes...)>
{
public:
	typedef InRetType RetType;
	/**
 * Virtual destructor.
 */
	virtual ~IDelegateInstance() = default;


	/**
	 * Execute the delegate.  If the function pointer is not valid, an error will occur.
	 */
	virtual RetType Execute(ArgTypes...) const = 0;
};



typedef TAlignedBytes<16, 16> FAlignedInlineDelegateType;

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