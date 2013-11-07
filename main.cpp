#include <iostream>
#include <stdio.h>
#include <fstream>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include "Robot/Robot.h"
#include "Debug/Debug.h"

using namespace std;
using namespace boost;

// Starting point for our robot program
// Testing comment
int main()
{
	try{
		Robot robot("./settings.xml");
		cout << "Robot created" << endl;
		Debug debug(&robot);
		debug.testSegmentation(filesystem::path("../MovementConstraints/Camera/database segmentation"));
	}
	catch(char const* error){
		cout << error << endl;
	}
}

