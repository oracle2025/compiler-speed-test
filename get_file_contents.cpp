#include "get_file_contents.h"
#include <fstream>
#include <streambuf>


std::string get_file_contents(const char* filename)
{
	std::ifstream t(filename);
	std::string str((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
	return str;
}

