#include "NullEngine.h"

class TestGame : public Application
{
public:
	TestGame()
	{

	}
	~TestGame()
	{

	}
};

Application* CreateApplication()
{
	return new TestGame();
}