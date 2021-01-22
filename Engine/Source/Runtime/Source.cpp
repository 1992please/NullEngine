#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Delegates/Delegate.h"

TDelegate<void(int, bool, int*)> LOL;
TDelegate<void(int, bool, int*)> LOLX;
TMulticastDelegate<void(int, bool, int*)> LOLM;

void shit(int x, bool y, int* z)
{

}

void TestDelegates()
{
	LOLM.AddStatic(shit);
	int t = 5;
	int* Pt = &t;
	LOLM.Broadcast(10, true, Pt);
	LOLM.Clear();
	LOL.BindStatic(shit);
	LOL.Execute(10, true, Pt);
	if (LOL.IsBound())
	{
		LOL.Unbind();
	}
}