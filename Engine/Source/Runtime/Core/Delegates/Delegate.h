#pragma once
#include "Core/CoreTypes.h"
#include "DelegateInstance.h"
#include "DelegateInstancesImpl.h"
#include "DelegateBase.h"
#include "MulticastDelegateBase.h"


#define DECLARE_DELEGATE_RetVal( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

/** Declares a broadcast delegate that can bind to multiple native functions simultaneously */
#define DECLARE_MULTICAST_DELEGATE( MulticastDelegateName, ... ) \
	typedef TMulticastDelegate<void(__VA_ARGS__)> MulticastDelegateName;

#define DECLARE_DELEGATE(DelegateName, ...) DECLARE_DELEGATE_RetVal(DelegateName, void, __VA_ARGS__)
