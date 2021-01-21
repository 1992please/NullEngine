#include "Application.h"



FApplication::FApplication()
{
	pWindow = FWindow::Create();
	//pWindow->SetEventCallback(&FApplication::OnEvent);
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

void FApplication::OnEvent(FEvent& InEvent)
{

}
