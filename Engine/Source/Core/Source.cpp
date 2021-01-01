#include "NullEngine.h"
#include "Containers/Array.h"

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
//void heapify(int arr[], int n, int i)
//{
//	int largest = i; // Initialize largest as root
//	int l = 2 * i + 1; // left = 2*i + 1
//	int r = 2 * i + 2; // right = 2*i + 2
//
//	// If left child is larger than root
//	if (l < n && arr[l] > arr[largest])
//		largest = l;
//
//	// If right child is larger than largest so far
//	if (r < n && arr[r] > arr[largest])
//		largest = r;
//
//	// If largest is not root
//	if (largest != i) {
//		Swap(arr[i], arr[largest]);
//
//		// Recursively heapify the affected sub-tree
//		heapify(arr, n, largest);
//	}
//}
//
//// main function to do heap sort
//void heapSort(int arr[], int n)
//{
//	// Build heap (rearrange array)
//	for (int i = n / 2 - 1; i >= 0; i--)
//		heapify(arr, n, i);
//
//	// One by one extract an element from heap
//	for (int i = n - 1; i > 0; i--) {
//		// Move current root to end
//		Swap(arr[0], arr[i]);
//
//		// call max heapify on the reduced heap
//		heapify(arr, i, 0);
//	}
//}

int main(int argc, char** argv)
{
	system("color f0");

	struct WTF
	{
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
			printf("(%d, %d)", lol, haha);
		}

		~WTF() {
			printf("WTF");
		}
	};
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
