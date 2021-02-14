#include "NullEngine.h"
#include "EditorLayer.h"

class FEditorApp : public FApplication
{
public:
	FEditorApp()
		: FApplication("Null Editor")
	{
		PushLayer(new FEditorLayer());
	}

	~FEditorApp()
	{
	}
};

FApplication* CreateApplication()
{
	return new FEditorApp();
}