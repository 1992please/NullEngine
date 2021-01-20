#include "Application.h"



FApplication::FApplication()
{
	pWindow = FWindow::Create();
	bRunning = true;
}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	while (bRunning)
	{
		pWindow->OnUpdate();
	}
}
