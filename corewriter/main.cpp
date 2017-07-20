// corewriter.cpp : Defines the entry point for the console application.
//

#include "CoreWriter.h"

#include <iostream>

int main(int argc, char** argv)
{
	CoreWriter coreWriter;
	try {
		coreWriter.run(argc, argv);
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what();
		return 1;
	} catch (...) {
		std::cerr << "Unknown error";
		return 2;
	}
    return 0;
}