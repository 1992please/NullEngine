#include "Engine.h"
#include "Containers/BitArray.h"
#include "Containers/Array.h"
#include "Containers/SparseArray.h"
#include "Containers/Set.h"
#include "Containers/Map.h"
#include "Containers/String.h"


struct WTF
{
	void* Shit;
	int lol;
	int haha;
	WTF() {};
	WTF(int InLol, int inHaha) : lol(InLol), haha(inHaha) {}
	bool operator==(const WTF& inWTF) const
	{
		return inWTF.lol == lol && inWTF.haha == haha;
	}

	bool operator<(const WTF& inWTF) const
	{
		return lol < inWTF.lol;
	}

	bool operator>(const WTF& inWTF) const
	{
		return lol < inWTF.lol;
	}

	void Print()
	{
		NE_CORE_LOG("(%d, %d)", lol, haha);
	}

	~WTF() {
		NE_CORE_LOG("WTF");
	}
};

struct Test
{
	int value;
};

FORCEINLINE uint32 GetTypeHash(const WTF& A)
{
	return FCrc::MemCrc32(&A, sizeof(WTF));
}

class TestGame : public Application
{
public:
	TestGame()
	{
		NE_CORE_ERROR("Hello Bitch");
		NE_CORE_WARN("Hello Bitch = %s %d", "WTF", 5);
		NE_CORE_LOG("Hello Bitch");

		{
			TArray<WTF> Shit;
			TArray<WTF> Hello = { {32, 32}, {23,34} };
			if (Hello[0] < Hello[1])
			{
				Hello.SetNum(5);
			}

			Hello.Emplace(56, 6);
			Hello.Emplace(5, 7);
			Hello.Emplace(6, 10);
			Hello.Emplace(5, 10);
			Hello.Emplace(8, 30);
			Hello.Emplace(5, 30);
			Hello.Emplace(21, 6);
			Hello.Emplace(5, 6);
			//Hello.RemoveSwap(Hello[5]);
			Hello += { {32, 32}, { 23,34 } };

			WTF X(300, 50);
			Hello.AddUnique(X);
			int32 Found = Hello.Find(X);
			Hello.Remove(WTF(5, 6));
			Hello.FindByPredicate([](const WTF& InElement) {return InElement.haha == 50; });
			Hello.Contains(WTF(5, 30));
			if (Hello.ContainsByPredicate([](const WTF& InElement) {return InElement.haha == 456; }))
			{
				NE_CORE_LOG("shit");
			}
			else
			{
				NE_CORE_LOG("shit");
			}
			Hello.Sort([](const WTF& RHS, const WTF& LHS) {
				return (RHS.lol + RHS.haha) < (LHS.lol + LHS.haha);
			});
			Hello.HeapSort();

			for (WTF& Element : Hello)
			{
				Element.Print();
			}
		}
		{
			FBitArray BitArray;
			FBitArray BitArray1(1, 30);
			FBitArray BitArray2(BitArray1);
			FBitArray BitArray3(MoveTemp(BitArray1));
			BitArray.Add(0, 10);
			BitArray.Add(1, 30);
			BitArray.Add(0, 25);
			BitArray.Add(1, 15);
			BitArray.Add(0, 200);
			BitArray.Add(1, 100);
			BitArray1.Add(0);
			BitArray1.Add(0);
			BitArray1.Add(0);
			BitArray1.Add(1);
			BitArray1.Add(0);
			BitArray1.Add(1);
			BitArray1.Add(0);
			BitArray1.Add(1);
			BitArray1.Add(0);
			BitArray1.Add(1);
			bool Value = BitArray[150];

			FConstSetBitIterator BitIterator(BitArray1, 5);
		}
		{
			//FBitArray BitArray;
			TSparseArray<WTF> SShit;
			WTF XZ(300, 50);
			SShit.Add(XZ);
			SShit.RemoveAt(0, 1);
			SShit.Emplace(56, 6);
			SShit.Emplace(5, 7);
			SShit.Emplace(6, 10);
			SShit.Emplace(5, 10);
			SShit.Emplace(8, 30);
			SShit.Emplace(5, 30);
			SShit.Emplace(21, 6);
			SShit.Emplace(5, 6);

			SShit.Reserve(50);
			//SShit.Reset();

			SShit.Sort();
			for (WTF& Element : SShit)
			{
				NE_CORE_LOG("shit");
			}
		}
		{
			TArray<WTF> A7A = { {32, 32}, {23,34} };
			TSet<WTF> Hello(A7A);
			TSparseArray<WTF> Shit;
			WTF X(300, 50);
			Hello.Add(X);
			Hello.Add(WTF(300, 50));
			Hello.Reset();
			Hello.Empty();
			WTF* HHH = Hello.Find(X);
			for (WTF& Element : Hello)
			{
				Element.Print();
			}
		}
		{
			TMap<int, WTF> hello;
			hello.Add(5, WTF(300000, 50));
			hello.Add(2, WTF(300, 50));
			hello.Add(3, WTF(3000, 50));
			hello.Add(1, WTF(30, 50));
			WTF& OutValue = hello[5];
			hello.Reserve(50);
			hello.Num();
			hello.Reset();
			hello.Compact();
			hello.GetAllocatedSize();

			hello.FindOrAdd(5);
			WTF& WTFValue = hello.Add(6);

			for (TPair<int, WTF>&it : hello)
			{

			}
		}
		{
			FString Shit;
			Shit = "haha nad the big";
			Shit += " fuck";
			Shit += FString::Printf("my age is %d", 28);
			NE_CORE_LOG("%s", *Shit);
		}
	}
	virtual void Run()
	{
		Application::Run();
	}
	~TestGame()
	{

	}
};

Application* CreateApplication()
{

	return new TestGame();
}