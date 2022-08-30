#include <iostream>
#include "Engine\Rogy.h"

int main(int argc, char* argv[])
{
	std::string startipProject = "Assets";
	
	// Chec for the startup project name in the file "core\\startup", 
	// if didn't exist create a new one and start project "Assets"
	std::fstream newfile;
	newfile.open("core\\startup", ios::in);
	if (newfile.is_open()) 
	{ 
		getline(newfile, startipProject);
		newfile.close(); 
	}
	else
	{
		std::ofstream fout("core\\startup");
		fout << startipProject;
		fout.close();
	}

	Rogy engine;
	std::cout << "Initializing Rogy Engine (" << startipProject << ") ..." << std::endl;
	
	// Enable/Disable Editor mode in top Rogy.h
	engine.ProjectResourcesFolder = startipProject;

	if (!engine.Init())
		return -1;
		
	engine.MainLoop();
	
	return 0;
}
