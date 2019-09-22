#include "NullEngine.h"
#include <stdio.h>
int main(int argc, char** argv)
{
	system("color f0");
	NullEngine* pApp = new NullEngine();

	pApp->Run();
	delete pApp;
	return 0;
}
