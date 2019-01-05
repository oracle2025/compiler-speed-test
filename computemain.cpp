#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include "clew.h"

//#include <boost/compute/core.hpp>
/*#include <boost/compute/algorithm/copy.hpp>
#include <boost/compute/container/vector.hpp>*/
#include <boost/compute/utility/source.hpp>
#include <iostream>

#include "shadertest.h"
#include "last_modified.h"
#include <thread>
#include <map>

//namespace compute = boost::compute;

bool has_file_changed(std::map<std::string, std::time_t> &last_changed_files)
{
	bool result = false;

	/*for (auto const& x : symbolTable)
	{
    	std::cout << x.first  // string (key)
            << ':' 
            << x.second // string's value 
            << std::endl ;
	}*/

	for ( auto const& x: last_changed_files) {
		if (last_modified(x.first.c_str()) != x.second) {
			last_changed_files[x.first] = last_modified(x.first.c_str());
			result = true;
		}
	}
	return result;
}

int main(int argc, const char *argv[])
{
	int error;
    bool clpresent = 0 == (error = clewInit());
    if( !clpresent ) {
		std::cout << "opencl library not found: " << error << std::endl;
        return -1;
    }
    /*
	for (const auto& device: compute::system::devices()) {
		std::cout << "Device: " << device.name() << std::endl;
	}
	for (const auto& device: compute::system::platforms()) {
		std::cout << "Platform: " << device.name() << std::endl;
	}*/

	const char *source = BOOST_COMPUTE_STRINGIZE_SOURCE(
			);
	const char *inputfile = "../Lenna2176.png";
	const char *outputfile = "out.png";
	const char *sourcefile = "../shader.cl";
	const char *raster_shader = "../raster.cl";
	const char *scale_shader = "../scale.cl";
	const char *scramble_shader = "../scramble.cl";
	const char *unscramble_shader = "../unscramble.cl";

	ShaderTest shader_test(inputfile, outputfile, "");
	std::map<std::string, std::time_t> last_changed_files;
	last_changed_files[scale_shader] = last_modified(scale_shader);
	last_changed_files[raster_shader] = last_modified(raster_shader);
	last_changed_files[scramble_shader] = last_modified(scramble_shader);
	last_changed_files[unscramble_shader] = last_modified(unscramble_shader);

	/* Use this to run the test last edited
	 *
	 * std::map< unsigned int, std::function<int(int,int)> > callbackMap;
	 * */
	while (true) {
		auto last_change_time = last_modified(sourcefile);
		std::cout << "Run" << std::endl;
		shader_test.reload_input();
		shader_test.scale(scale_shader);
		shader_test.raster(raster_shader);
		shader_test.scramble(scramble_shader);
		shader_test.overscramble(2200, scramble_shader);
		shader_test.unscramble(unscramble_shader);
		shader_test.write_out_image();
		while (!has_file_changed(last_changed_files)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}


		//shader_test.run(sourcefile);
		/*while (last_change_time == last_modified(sourcefile)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}*/
		//shader_test.scale(scale_shader);
		//shader_test.raster(raster_shader);
		//shader_test.scramble(scramble_shader);
		//shader_test.unscramble(unscramble_shader);
	}
	
	return 0;
}
