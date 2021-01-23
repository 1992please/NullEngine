#pragma once
#include "Core/CoreTypes.h"
#include "DelegateBase.h"
#include "Core/Containers/Array.h"
#include "Core/Math/MathUtility.h"

template <typename DelegateSignature>
class TMulticastDelegate
{
	static_assert(sizeof(DelegateSignature) == 0, "Expected a function signature for the delegate template parameter");
};

template <typename RetValType, typename... ParamTypes>
class TMulticastDelegate<RetValType(ParamTypes...)>
{
	static_assert(sizeof(RetValType) == 0, "The return type of a multicast delegate must be void");
};

template <typename... ParamTypes>
class TMulticastDelegate<void(ParamTypes...)>
{
	typedef IBaseDelegateInstance<void(ParamTypes...)> DelegateInstanceType;
	typedef TDelegate<void(ParamTypes...)> DelegateType;
	typedef TArray<FDelegateBase> InvocationListType;
public:
	/** Hidden default constructor. */
	FORCEINLINE TMulticastDelegate()
		: CompactionThreshold(2)
		, InvocationListLockCount(0)
	{ }

	TMulticastDelegate(const TMulticastDelegate& Other)
	{
		*this = Other;
	}

	TMulticastDelegate& operator=(const TMulticastDelegate& Other)
	{
		if (&Other != this)
		{
			Clear();

			for (const FDelegateBase& OtherDelegateRef : Other.GetInvocationList())
			{
				if (IDelegateInstance* OtherInstance = OtherDelegateRef.GetDelegateInstanceProtected())
				{
					FDelegateBase TempDelegate;
					((DelegateInstanceType*)OtherInstance)->CreateCopy(TempDelegate);
					AddDelegateInstance(MoveTemp(TempDelegate));
				}
			}
		}

		return *this;
	}

	void Clear()
	{
		for (FDelegateBase& DelegateBaseRef : InvocationList)
		{
			DelegateBaseRef.Unbind();
		}

		CompactionThreshold = 2;
		InvocationList.Reset();
		if (InvocationList.Max() > CompactionThreshold)
		{
			InvocationList.Shrink();
		}
	}

	inline bool IsBound() const
	{
		for (const FDelegateBase& DelegateBaseRef : InvocationList)
		{
			if (DelegateBaseRef.GetDelegateInstanceProtected())
			{
				return true;
			}
		}
		return false;
	}

	inline bool IsBoundToObject(void const* InUserObject) const
	{
		for (const FDelegateBase& DelegateBaseRef : InvocationList)
		{
			IBaseDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
			if ((DelegateInstance != nullptr) && DelegateInstance->HasSameObject(InUserObject))
			{
				return true;
			}
		}

		return false;
	}

	int32 RemoveAll(const void* InUserObject)
	{
		int32 Result = 0;
		if (InvocationListLockCount > 0)
		{
			for (FDelegateBase& DelegateBaseRef : InvocationList)
			{
				IBaseDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
				if ((DelegateInstance != nullptr) && DelegateInstance->HasSameObject(InUserObject))
				{
					// Manually unbind the delegate here so the compaction will find and remove it.
					DelegateBaseRef.Unbind();
					++Result;
				}
			}

			// can't compact at the moment, but set out threshold to zero so the next add will do it
			if (Result > 0)
			{
				CompactionThreshold = 0;
			}
		}
		else
		{
			// compact us while shuffling in later delegates to fill holes
			for (int32 InvocationListIndex = 0; InvocationListIndex < InvocationList.Num();)
			{
				FDelegateBase& DelegateBaseRef = InvocationList[InvocationListIndex];

				IBaseDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
				if (DelegateInstance == nullptr
					|| DelegateInstance->HasSameObject(InUserObject))
				{
					InvocationList.RemoveAtSwap(InvocationListIndex, 1, false);
					++Result;
				}
				else
				{
					InvocationListIndex++;
				}
			}

			CompactionThreshold = FMath::Max(2, 2 * InvocationList.Num());

			InvocationList.Shrink();
		}

		return Result;
	}

	bool Add(DelegateType&& InNewDelegate)
	{
		if (InNewDelegate.GetDelegateInstanceProtected())
		{
			AddDelegateInstance(MoveTemp(InNewDelegate));
			return true;
		}

		return false;
	}

	/**
	 * Adds a delegate instance to this multicast delegate's invocation list.
	 *
	 * @param Delegate The delegate to add.
	 */
	bool Add(const DelegateType& InNewDelegate)
	{
		if (InNewDelegate.GetDelegateInstanceProtected())
		{
			AddDelegateInstance(CopyTemp(InNewDelegate));
			return true;
		}

		return false;
	}

	/**
	 * Adds a raw C++ pointer global function delegate
	 *
	 * @param	InFunc	Function pointer
	 */
	template <typename... VarTypes>
	inline bool AddStatic(typename TBaseStaticDelegateInstance<void(ParamTypes...)>::FFuncPtr InFunc)
	{
		return Add(DelegateType::CreateStatic(InFunc));
	}

	/**
	 * Adds a C++ lambda delegate
	 * technically this works for any functor types, but lambdas are the primary use case
	 *
	 * @param	InFunctor	Functor (e.g. Lambda)
	 */
	template<typename FunctorType>
	inline bool AddLambda(FunctorType&& InFunctor)
	{
		return Add(DelegateType::CreateLambda(Forward<FunctorType>(InFunctor)));
	}

	template <typename UserClass>
	inline bool AddRaw(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(ParamTypes...)>::Type InFunc)
	{
		static_assert(!TIsConst<UserClass>::Value, "Attempting to bind a delegate with a const object pointer and non-const member function.");

		return Add(DelegateType::CreateRaw(InUserObject, InFunc));
	}

	template <typename UserClass>
	inline bool AddRaw(const UserClass* InUserObject, typename TMemFunPtrType<true, UserClass, void(ParamTypes...)>::Type InFunc)
	{
		return Add(DelegateType::CreateRaw(InUserObject, InFunc));
	}

	void Broadcast(ParamTypes... Params) const
	{
		bool NeedsCompaction = false;

		LockInvocationList();
		{
			const InvocationListType& LocalInvocationList = GetInvocationList();

			// call bound functions in reverse order, so we ignore any instances that may be added by callees
			for (int32 InvocationListIndex = LocalInvocationList.Num() - 1; InvocationListIndex >= 0; --InvocationListIndex)
			{
				// this down-cast is OK! allows for managing invocation list in the base class without requiring virtual functions
				const FDelegateBase& DelegateBase = LocalInvocationList[InvocationListIndex];

				DelegateInstanceType* DelegateInstanceInterface = (DelegateInstanceType*)DelegateBase.GetDelegateInstanceProtected();
				if (DelegateInstanceInterface)
				{
					DelegateInstanceInterface->Execute(Params...);
				}
				else
				{
					NeedsCompaction = true;
				}

			}
		}
		UnlockInvocationList();

		if (NeedsCompaction)
		{
			const_cast<TMulticastDelegate*>(this)->CompactInvocationList();
		}
	}

private:
	FORCEINLINE void AddDelegateInstance(DelegateType&& NewDelegateBaseRef)
	{
		// compact but obey threshold of when this will trigger
		CompactInvocationList(true);
		InvocationList.Add(MoveTemp(NewDelegateBaseRef));
	}

	void CompactInvocationList(bool CheckThreshold = false)
	{
		// if locked and no object, just return
		if (InvocationListLockCount > 0)
		{
			return;
		}

		// if checking threshold, obey but decay. This is to ensure that even infrequently called delegates will
		// eventually compact during an Add()
		if (CheckThreshold 	&& --CompactionThreshold > InvocationList.Num())
		{
			return;
		}

		int32 OldNumItems = InvocationList.Num();

		// Find anything null remove it
		for (int32 InvocationListIndex = 0; InvocationListIndex < InvocationList.Num();)
		{
			auto& DelegateBaseRef = InvocationList[InvocationListIndex];

			IDelegateInstance* DelegateInstance = DelegateBaseRef.GetDelegateInstanceProtected();
			if (DelegateInstance == nullptr)
			{
				InvocationList.RemoveAtSwap(InvocationListIndex);
			}
			else
			{
				InvocationListIndex++;
			}
		}

		CompactionThreshold = FMath::Max(2, 2 * InvocationList.Num());

		if (OldNumItems > CompactionThreshold)
		{
			// would be nice to shrink down to threshold, but reserve only grows..?
			InvocationList.Shrink();
		}
	}

	/**
	 * Gets a read-only reference to the invocation list.
	 *
	 * @return The invocation list.
	 */
	inline InvocationListType& GetInvocationList()
	{
		return InvocationList;
	}

	inline const InvocationListType& GetInvocationList() const
	{
		return InvocationList;
	}

	/** Increments the lock counter for the invocation list. */
	FORCEINLINE void LockInvocationList() const
	{
		++InvocationListLockCount;
	}

	/** Decrements the lock counter for the invocation list. */
	FORCEINLINE void UnlockInvocationList() const
	{
		--InvocationListLockCount;
	}

	/** Holds the collection of delegate instances to invoke. */
	InvocationListType InvocationList;

	/** Used to determine when a compaction should happen. */
	int32 CompactionThreshold;

	/** Holds a lock counter for the invocation list. */
	mutable int32 InvocationListLockCount;
};
