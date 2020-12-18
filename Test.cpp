#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include<CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>

typedef const unsigned int cuint;
enum Block {TOP,MIDDLE,BOTTOM};

cuint _WIDTH = 30;
cuint _HEIGHT = 30;
cuint _DEPTH = 30;


// w = width
// h = height
// d = depth

bool doesIntersect(int aMinX, int aMinY, int aMinZ, int aMaxX, int aMaxY, int aMaxZ, // Block
			       int bMinX, int bMinY, int bMinZ, int bMaxX, int bMaxY, int bMaxZ) // Test Boundary
{
	return (aMinX <= bMaxX && aMaxX >= bMinX) &&
		   (aMinY <= bMaxY && aMaxY >= bMinY) &&
		   (aMinZ <= bMaxZ && aMaxZ >= bMinZ);
}

bool isTop(int x0, int y0, int z0, int w0, int h0, int d0)
{
	cuint x = 0;
	cuint y = _HEIGHT;
	cuint z = 0;

	return doesIntersect(x, y,z,_WIDTH, y - _HEIGHT * 1 / 3,_DEPTH,x0,y0,z0,w0,h0,d0);
}

bool isMid(int x0, int y0, int z0, int w0, int h0, int d0)
{
	cuint x = 0;
	cuint y = _HEIGHT * 2 / 3;
	cuint z = 0;

	return doesIntersect(x, y, z, _WIDTH, y - _HEIGHT * 1 / 3, _DEPTH, x0, y0, z0, w0, h0, d0);
}

bool isBot(int x0, int y0, int z0, int w0, int h0, int d0)
{
	cuint x = 0;
	cuint y = _HEIGHT * 1 / 3;
	cuint z = 0;

	return doesIntersect(x, y, z, _WIDTH, y - _HEIGHT * 1 / 3, _DEPTH, x0, y0, z0, w0, h0, d0);
}

inline void checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}


std::vector<int> chunkWork1D(cl::Context context, cl::Program program, cl::Device device, std::vector<int> * vec, int chunkSize, int err = 0)
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

std::vector<int> chunkWork3D(cl::Context context, cl::Program program, cl::Device device, std::vector<int> * vec, cuint a_size,
	cuint chunk_x, cuint chunk_y, cuint chunk_z, int err = 0)
{
	char calcToPerform[] = "calcNon";
	int chunkSize = chunk_x * chunk_y * chunk_z;
	int x = 0;
	int y = 0;
	int z = 0;

	// initial point to start of our vector
	for (int* pVec = vec->data(); *pVec < vec->size(); pVec += chunkSize)
	{
		// create buffers, and kernel
		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * chunkSize, pVec);
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * chunkSize, nullptr);

		if (isTop(x,y,z, x+chunk_x,y+chunk_y,z+chunk_z))
		{
			printf("TOP: %d %d %d\n", x, y, z);
			strcpy_s(calcToPerform, "calcTop");
		}
		if (isMid(x, y, z, x+chunk_x, y+chunk_y, z+chunk_z))
		{
			printf("MID: %d %d %d\n", x, y, z);
			strcpy_s(calcToPerform, "calcMid");
		}
		if (isBot(x, y, z, x+chunk_x, y+chunk_y, z+chunk_z))
		{
			printf("BOT: %d %d %d\n", x, y, z);
			strcpy_s(calcToPerform, "calcBot");
		}

		// TODO: overlap check
		// Friday: unit testing
		// no need to consider Z direction
		// TODO: create a sub array for fortran convention on host
		// TODO: coordinate check on the kernel side
		// TODO: super-kernel case for every other kernel, but they are subroutines for other kernels
		// TODO: consider three possible ways of chunking, chunk x, y or z dimension (chunk 1,2 or 3 dimensions)

		if (x >= _WIDTH)
		{
			x = 0;
			z += chunk_z;
		}

		if (z >= _DEPTH)
		{
			z = 0;
			y += chunk_y;
		}

		if (y >= _HEIGHT)
		{
			y = 0;
		}

		x += chunk_x;

		cl::Kernel kernel(program, calcToPerform, &err);
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
	cuint array_width = _WIDTH;
	cuint array_height = _HEIGHT;
	cuint array_depth = _DEPTH;
	cuint array_size = array_width * array_depth * array_height;
	// int* array_3d = new int[array_size]; // feeling cute, might delete later

	// initialise 3d array
	// memset(array_3d, 1,array_size);

	std::vector<int> vec(array_size);
	std::fill(vec.begin(), vec.end(), 1);
	
	// 3D chunky boii
	cuint chunkSize_width = 10;
	cuint chunkSize_height = 10;
	cuint chunkSize_depth = 10;
	cuint chunkSize = chunkSize_width * chunkSize_height * chunkSize_depth;

	// perform chunking function
	vec = chunkWork3D(context, program, device, &vec, array_size, chunkSize_width, chunkSize_height, chunkSize_depth, err);

	// TODO: Unit Tests
	
	for (auto &i:vec)
	{
		printf("%d", i);
	}

	printf("\n");
	cl::finish();
}