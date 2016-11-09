#include "last_modified.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::time_t last_modified(const char* filename)
{
	fs::path p(filename);
	return fs::last_write_time(p);
}

