#include "shadertest.h"
//#include "shadertestimpl.h"
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include "clew.h"
#include <boost/compute/core.hpp>
#include <boost/compute/container/vector.hpp>
#include <iostream>
#include <boost/compute/interop/opencv/core.hpp>
#include <boost/compute/interop/opencv/highgui.hpp>
//#include <boost/compute/utility/source.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "get_file_contents.h"
/*#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"*/

namespace compute = boost::compute;

struct ShaderTest::impl
{
	impl(const char* inputfile, const char* outputfile, const char* maskfile) :
		gpu(boost::compute::system::find_device("Hainan")),
    	context(gpu),
		queue(context, gpu),
		m_inputfile(inputfile),
		m_outputfile(outputfile),
	{
		std::cout << "OpenCL Device: " << gpu.name() << std::endl;
    	std::cout << "Vender: " << gpu.vendor() << std::endl;
    	std::cout << "Profile: " << gpu.profile() << std::endl;
    	std::cout << "Version: " << gpu.version() << std::endl;
    	std::cout << "Driver Version: " << gpu.driver_version() << std::endl;
    	std::cout << "Max Width: " << gpu.get_info<size_t>(CL_DEVICE_IMAGE2D_MAX_WIDTH) << std::endl;
    	std::cout << "Max Height: " << gpu.get_info<size_t>(CL_DEVICE_IMAGE2D_MAX_HEIGHT) << std::endl;


    	auto image = cv::imread(inputfile, CV_LOAD_IMAGE_COLOR);

		cv::cvtColor(image, image, CV_BGR2BGRA);

    	gpu_image = compute::opencv_create_image2d_with_mat(
    			image, compute::image2d::read_write, queue);


		gpu_out_image = compute::image2d(
				context,
				gpu_image.width(),
				gpu_image.height(),
				gpu_image.format(),
				compute::image2d::read_write
				);
		{
			cv::Mat m(4000,4000, CV_8UC4, cv::Scalar(255,255,255,255));
			//boost::compute::image_format format(CL_BGRA, CL_UNORM_INT8);
			//compute::image_format format(CL_RGBA, CL_UNSIGNED_INT8);
			auto output_image = boost::compute::opencv_create_image2d_with_mat(m, boost::compute::image2d::read_write, queue);
			/*boost::compute::image2d output_image(
        			context,
        			4000,
        			4000,
        			format,
        			boost::compute::image2d::read_write
    				);
    		boost::compute::int4_ black(255, 255, 255, 255);
        	queue.enqueue_fill_image(output_image, &black, output_image.origin(), output_image.size());*/
			auto mat = boost::compute::opencv_create_mat_with_image2d(output_image, queue);
			cv::imwrite("out_blank.png", mat);
		}

	}
	~impl()
	{
	}
	void run(const char* sourcefile)
	{
    	auto image_kernel = load_kernel(sourcefile);
		image_kernel.set_arg(0, gpu_image);
		image_kernel.set_arg(1, gpu_out_image);
		int mw, mh;
		auto mask = loadmask("..\\gg-map-vac160-cyan.png", mw, mh);
		image_kernel.set_arg(2, mask);
		image_kernel.set_arg(3, mw);
		image_kernel.set_arg(4, mh);	
		/*
	 	 * mask, mw, mh
	 	 */

		size_t origin[2] = { 0, 0 };
		size_t region[2] = { gpu_image.width(), gpu_image.height() };
		queue.enqueue_nd_range_kernel(image_kernel, 2, origin, region, 0);


	}
	boost::compute::vector<float> loadmask(const char* filename, int &w, int &h)
	{
		int depth;
    	unsigned char* mask;// = stbi_load(filename, &w, &h, &depth, 1);
		std::vector<float> mask_vector(w*h);
		for (int i = 0; i < (w*h); i++) {
			mask_vector[i] = mask[i] / 255.0f;
		}
		//stbi_image_free(mask);
		boost::compute::vector<float> device_mask(w*h, context);

    	// copy data to the device
		boost::compute::copy(
        	mask_vector.begin(), mask_vector.end(), device_mask.begin(), queue
    	);
    	return device_mask;
	}
	void scale(const char* sourcefile)
	{
		run_shader_on_gpu_image(sourcefile);
	}
	void raster(const char* sourcefile)
	{
		run_shader_on_gpu_image(sourcefile);
	}
	void scramble(const char* sourcefile)
	{
    	auto image_kernel = load_kernel(sourcefile);
		gpu_image = gpu_out_image;
		gpu_out_image = compute::image2d(
				context,
				gpu_image.width(),
				gpu_image.height(),
				gpu_image.format(),
				compute::image2d::read_write
				);

		m_output_height = gpu_image.height() * 2;
 		output_buffer = boost::compute::vector<unsigned char>(m_output_height*2048, context);
 		boost::compute::fill(output_buffer.begin(), output_buffer.end(), 0, queue);

		image_kernel.set_arg(0, gpu_image);
		image_kernel.set_arg(1, output_buffer);
		image_kernel.set_arg(8, 0); //offset
		size_t origin[2] = { 0, 0 };
		size_t region[2] = { 512, m_output_height };
		queue.enqueue_nd_range_kernel(image_kernel, 2, origin, region, 0);
	}
	void overscramble(int offset, const char* sourcefile)
	{
    	auto image_kernel = load_kernel(sourcefile);
		gpu_image = gpu_out_image;
		gpu_out_image = compute::image2d(
				context,
				gpu_image.width(),
				gpu_image.height(),
				gpu_image.format(),
				compute::image2d::read_write
				);

		//m_output_height = gpu_image.height() * 2;
 		//output_buffer = boost::compute::vector<unsigned char>(m_output_height*2048, context);
 		//boost::compute::fill(output_buffer.begin(), output_buffer.end(), 0, queue);

		image_kernel.set_arg(0, gpu_image);
		image_kernel.set_arg(1, output_buffer);
		image_kernel.set_arg(8, offset);
		size_t origin[2] = { 0, 0 };
		size_t region[2] = { 512, m_output_height };
		queue.enqueue_nd_range_kernel(image_kernel, 2, origin, region, 0);
	}
	boost::compute::kernel load_kernel(const char* sourcefile)
	{
		auto source = get_file_contents(sourcefile);
    	compute::program image_program =
        	compute::program::create_with_source(source, context);
    	try {
    		image_program.build();
    	} catch (std::exception &error) {
    		std::cerr << "Build Error: " << error.what() << std::endl;
    	}

		return compute::kernel(image_program, "image_kernel");
	}
	void unscramble(const char* sourcefile)
	{
    	auto image_kernel = load_kernel(sourcefile);

		gpu_image = gpu_out_image;
		gpu_out_image = compute::image2d(
				context,
				gpu_image.width()*2,
				gpu_image.height()*2,
				gpu_image.format(),
				compute::image2d::read_write
				);


		image_kernel.set_arg(0, output_buffer);
		image_kernel.set_arg(1, gpu_out_image);
		size_t origin[2] = { 0, 0 };
		size_t region[2] = { 512, m_output_height};
		queue.enqueue_nd_range_kernel(image_kernel, 2, origin, region, 0);
	}
	void run_shader_on_gpu_image(const char* sourcefile)
	{
    	auto image_kernel = load_kernel(sourcefile);

		gpu_image = gpu_out_image;
		gpu_out_image = compute::image2d(
				context,
				gpu_image.width(),
				gpu_image.height(),
				gpu_image.format(),
				compute::image2d::read_write
				);
		image_kernel.set_arg(0, gpu_image);
		image_kernel.set_arg(1, gpu_out_image);
		size_t origin[2] = { 0, 0 };
		size_t region[2] = { gpu_image.width(), gpu_image.height() };
		queue.enqueue_nd_range_kernel(image_kernel, 2, origin, region, 0);
	}
	void write_out_image()
	{
    	auto out_image = compute::opencv_create_mat_with_image2d(gpu_out_image, queue);
		cv::imwrite(m_outputfile, out_image);
	}
	void reload_input()
	{
    	auto image = cv::imread(m_inputfile, CV_LOAD_IMAGE_COLOR);

		cv::cvtColor(image, image, CV_BGR2BGRA);

    	gpu_image = compute::opencv_create_image2d_with_mat(
    			image, compute::image2d::read_write, queue);
		gpu_out_image = gpu_image;
	}
	boost::compute::device gpu;
    boost::compute::context context;
    boost::compute::command_queue queue;
	boost::compute::image2d gpu_image;
	boost::compute::image2d gpu_out_image;
	boost::compute::vector<unsigned char> output_buffer;
	size_t m_output_height;
	std::string m_outputfile;
	std::string m_inputfile;
};
void ShaderTest::ImplDeleter::operator()(ShaderTest::impl *p)
{
	delete p;
}
ShaderTest::ShaderTest(const char* inputfile, const char* outputfile, const char* maskfile) :
	pImpl(new impl(inputfile, outputfile, maskfile))
{
}
ShaderTest::~ShaderTest()
{
}
void ShaderTest::run(const char* sourcefile)
{
	pImpl->run(sourcefile);
}
void ShaderTest::scale(const char* sourcefile)
{
	pImpl->scale(sourcefile);
}
void ShaderTest::raster(const char* sourcefile)
{
	pImpl->raster(sourcefile);
}
void ShaderTest::scramble(const char* sourcefile)
{
	pImpl->scramble(sourcefile);
}
void ShaderTest::unscramble(const char* sourcefile)
{
	pImpl->unscramble(sourcefile);
}
void ShaderTest::write_out_image()
{
	pImpl->write_out_image();
}
void ShaderTest::reload_input()
{
	pImpl->reload_input();
}
void ShaderTest::overscramble(int offset, const char* sourcefile)
{
	pImpl->overscramble(offset, sourcefile);
}
//unique_ptr<ShaderTestImpl> m_impl;

