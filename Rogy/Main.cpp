#include <iostream>
#include "Engine\Rogy.h"
using namespace std;

int main()
{
	Rogy engine;
	cout << "Initializing Rogy Engine ..." << endl;
	
	// Enable/Disable Editor mode in top Rogy.h

	if (!engine.Init())
		return -1;
		
	engine.MainLoop();
	
	return 0;
}
