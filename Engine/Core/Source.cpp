#include "NullEngine.h"

int main(int argc, char** argv)
{
	NullEngine* pApp = new NullEngine();
	pApp->Run();
	delete pApp;
	return 0;
}
