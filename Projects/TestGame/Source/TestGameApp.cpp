#include "NullEngine.h"
#include "TestGame.h"

class FTestGame : public FApplication
{
public:
	FTestGame()
		: FApplication("Game")
	{
		PushLayer(new TestGame());
	}

	~FTestGame()
	{
	}
};

FApplication* CreateApplication()
{
	return new FTestGame();
}