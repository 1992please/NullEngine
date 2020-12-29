#include "NullEngine.h"

int main(int argc, char** argv)
{
	system("color f0");

	struct WTF
	{
		int lol;
		int haha;
		~WTF() {
			printf("WTF");
		}
	};
	{
		NArray<WTF> Hello;
		Hello.SetNum(20);
	}

	NullEngine* pApp = new NullEngine();

	pApp->Run();
	delete pApp;

	return 0;
}
