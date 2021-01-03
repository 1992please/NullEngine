#include "Core/NullEngine.h"
#include "Core/Containers/BitArray.h"
#include "Core/Containers/Array.h"


int main(int argc, char** argv)
{
	system("color f0");
	FBitArray BitArray;
	FBitArray BitArray1(1, 30);
	FBitArray BitArray2(BitArray1);
	FBitArray BitArray3(MoveTemp(BitArray1));

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

	//TBitArray BitArray;

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
