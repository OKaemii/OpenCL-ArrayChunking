#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include<CL/cl.hpp>
#include <iostream>
#include <fstream>


enum Block {TOP,MIDDLE,BOTTOM};


// w = width
// h = height
// d = depth

bool doesIntersect(int x0, int y0, int z0, int w0, int h0, int d0, // Block
			    int x1, int y1, int z1, int w1, int h1, int d1) // Test Boundary
{
	// Calculate Halves
	int hw0 = w0 >> 1;
	int hh0 = h0 >> 1;
	int hd0 = d0 >> 1;

	int hw1 = w1 >> 1;
	int hh1 = h1 >> 1;
	int hd1 = d1 >> 1;

	// Calculate Middle of Block
	int mx = x0 + hw0;
	int my = y1 + hh0;
	int mz = z0 + hd0;

	// Calculate Middle of Boundary
	int bx = x1 + hw1;
	int by = y1 + hh1;
	int bz = z1 + hd1;

	// Intersection test
	// Calculate Deltas
	int dx = abs(mx - bx);
	int dy = abs(my - by);
	int dz = abs(mz - bz);

	return dx > hw0 + hw1 && dy > hh0 + hh1 && dz > hd0 + hd0;
}


inline void checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}


std::vector<int> chunkWork(cl::Context context, cl::Program program, cl::Device device, std::vector<int> * vec, int chunkSize, int err = 0)
{
	// initial point to start of our vector
	for (int * pVec = vec->data(); *pVec < vec->size(); pVec += chunkSize)
	{
		// create buffers, and kernel
		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * chunkSize, pVec);
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * chunkSize, nullptr);
		cl::Kernel kernel(program, "ProcArray", &err);
		checkErr(err, "kernelling...");

		// fill in args of the user made kernel func in .cl
		kernel.setArg(0, inBuf);
		kernel.setArg(1, outBuf);

		cl::CommandQueue queue(context, device);

		// sets up where we to read the finished GPU data
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(chunkSize));
		checkErr(err, "ND Range kernel execution");

		// reads from GPU data from where it was set to based on NDRangeK
		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * chunkSize, pVec); // get data from device, and work on buffer
		checkErr(err, "Reading buffer...");
	}

	return *vec;
}



int main()
{
	// create platform to accommodate for devices
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl::Platform platform = platforms.front();

	// get GPU devices
	std::vector<cl::Device> devices;
	auto err = platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	checkErr(err, "getting GPU devices");
	
	// select first available GPU
	auto& device = devices.front();

	// create context from device
	const cl::Context context(device);

	// read in kernel file as source
	std::ifstream kernelFile("ProcArray.cl");
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
	const cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.size() + 1));

	// create program from both context, and source
	cl::Program program(context, sources);
	err = program.build(devices, "-cl-std=CL1.2");
	checkErr(err, "building program");

	char buildLog[1024];
	size_t actualLogSize;
	clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, sizeof(char) * 1024, buildLog, &actualLogSize);

	if (actualLogSize > 2) // i.e. there was something to report
	{
		std::cout << "Build log: (clipped to 1024 chars, actual size: " << actualLogSize << ")" << std::endl << std::endl;
		std::cout << buildLog << std::endl;
	}

	// make 3D array
	const unsigned int array_width = 60;
	const unsigned int array_height = 60;
	const unsigned int array_depth = 30;
	const unsigned int array_size = array_width * array_depth * array_height;
	int* array_3d = new int[array_size]; // feeling cute, might delete later

	// initialise 3d array
	memset(array_3d, 1,array_size);

	// define boundary sizes in this test, all have same boundary size
	const unsigned int boundary_width = array_width;
	const unsigned int boundary_height = array_height / 3;
	const unsigned int boundary_depth = array_depth;

	// define boundaries
	const unsigned int TOP_x = 0;
	const unsigned int TOP_y = 0;
	const unsigned int TOP_z = 0;

	const unsigned int MID_x = 0;
	const unsigned int MID_y = TOP_y + boundary_height;
	const unsigned int MID_z = 0;

	const unsigned int BOT_x = 0;
	const unsigned int BOT_y = MID_y + boundary_height;
	const unsigned int BOT_z = 0;

	// function to tell us where the blocks belong
	for (int i = 0; i < array_size; i++)
	{

		// array_3d[i] 
	}
	// cl_code for each block
	// read back data
	

	std::vector<int> vec(1024);
	int chunkSize = 256;
	std::fill(vec.begin(), vec.end(), 1);

	// perform chunking function
	vec = chunkWork(context, program, device, &vec, chunkSize, err);

	// output array
	for (auto &i:vec)
	{
		printf("%d", i);
	}
	printf("\n");
	cl::finish();
}