#include "NullEngine.h"
#include <fbxsdk.h>

int main(int argc, char** argv)
{
	system("color f0");

	// Change the following filename to a suitable filename value.
	const char* lFilename = "file.fbx";

	// Initialize the SDK manager. This object handles memory management.
	FbxManager* lSdkManager = FbxManager::Create();


	NullEngine* pApp = new NullEngine();

	pApp->Run();
	delete pApp;
	return 0;
}
