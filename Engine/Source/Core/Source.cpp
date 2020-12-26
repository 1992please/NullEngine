#include "NullEngine.h"
#include "Containers/Array.h"

int main(int argc, char** argv)
{
	system("color f0");

	NArray<int> Hello;
	Hello.Push(5);
	Hello.Push(7);
	Hello.Push(9);
	Hello.Push(10);



	NullEngine* pApp = new NullEngine();

	pApp->Run();
	delete pApp;
	return 0;
}
