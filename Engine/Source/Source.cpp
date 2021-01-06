#include "Core/NullEngine.h"
#include "Core/Containers/BitArray.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/SparseArray.h"

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
		printf("(%d, %d)\n", lol, haha);
	}

	~WTF() {
		printf("WTF");
	}
};

int main(int argc, char** argv)
{
	system("color f0");
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
	//FBitArray BitArray;
	TSparseArray<WTF> SShit;
	WTF XZ(300, 50);
	SShit.Add(XZ);
	SShit.RemoveAt(1, 5);
	SShit.Emplace(56, 6);
	SShit.Emplace(5, 7);
	SShit.Emplace(6, 10);
	SShit.Emplace(5, 10);
	SShit.Emplace(8, 30);
	SShit.Emplace(5, 30);
	SShit.Emplace(21, 6);
	SShit.Emplace(5, 6);

	SShit.Reserve(50);
	SShit.Reset();

	//SShit.Sort();

	for (WTF& Element : SShit)
	{
		printf("shit");
	}
	TArray<WTF> Shit;
	{
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

		Hello += { {32, 32}, { 23,34 } };

		WTF X(300, 50);
		Hello.AddUnique(X);
		int32 Found = Hello.Find(X);
		Hello.Remove(WTF(5, 6));
		Hello.FindByPredicate([](const WTF& InElement) {return InElement.haha == 50; });
		Hello.Contains(WTF(5, 30));
		if (Hello.ContainsByPredicate([](const WTF& InElement) {return InElement.haha == 456; }))
		{
			printf("shit");
		}
		else
		{
			printf("shit");
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

	NullEngine* pApp = new NullEngine();

	pApp->Run();
	delete pApp;

	return 0;
}
