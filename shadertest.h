#ifndef SHADERTEST_H

#define SHADERTEST_H

#include <memory>
//#include "impl.h"

class ShaderTest
{
	public:
		ShaderTest(const char* inputfile,
				const char* outputfile,
				const char* maskfile);
		~ShaderTest();
		void run(const char* sourcefile);

		void scale(const char* sourcefile);
		void raster(const char* sourcefile);
		void scramble(const char* sourcefile);
		void unscramble(const char* sourcefile);
		void overscramble(int offset, const char* sourcefile);
		void write_out_image();
		void reload_input();
	private:
		//std::unique_ptr<Impl> m_impl;
		struct impl;
		struct ImplDeleter
		{
  			void operator()(impl *p);
		};
		std::unique_ptr<impl, ImplDeleter> pImpl;
};
/*
// widget.h (interface)
class widget {
    // public members
private:
    struct impl;  // forward declaration of the implementation class
    // One implementation example: see below for other design options and trade-offs
    std::experimental::propagate_const< // const-forwarding pointer wrapper
        std::unique_ptr<                // unique-ownership opaque pointer
            impl>> pImpl;               // to the forward-declared implementation class
};
 
// widget.cpp (implementation)
struct widget::impl {
   // implementation details
};
*/
#endif /* end of include guard: SHADERTEST_H */
