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

std::vector<int> chunkWork3D_old(cl::Context context, cl::Program program, cl::Device device, std::vector<int> * vec, cuint a_size,
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

		//if (isTop(x,y,z, x+chunk_x,y+chunk_y,z+chunk_z))
		//{
		//	printf("TOP: %d %d %d\n", x, y, z);
		//	strcpy_s(calcToPerform, "calcTop");
		//}
		//if (isMid(x, y, z, x+chunk_x, y+chunk_y, z+chunk_z))
		//{
		//	printf("MID: %d %d %d\n", x, y, z);
		//	strcpy_s(calcToPerform, "calcMid");
		//}
		//if (isBot(x, y, z, x+chunk_x, y+chunk_y, z+chunk_z))
		//{
		//	printf("BOT: %d %d %d\n", x, y, z);
		//	strcpy_s(calcToPerform, "calcBot");
		//}

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

std::vector<int> vecToFortran3D(std::vector<int>* vec, cuint halo, cuint x0, cuint x1, cuint y0, cuint y1, cuint z0, cuint z1)
{
	// take only what we need from our vector
	// C is row-major order
	// convert array to column order
	cuint arr_size = ((x1 - x0) + halo) * ((y1 - y0) + halo) * ((z1 - z0));
	std::vector<int> dump(arr_size);

	cuint x_begin = x0 - halo > 0 ? x0 - halo : 0;
	cuint x_end = x1 + halo < _WIDTH ? x1 + halo : _WIDTH;

	cuint y_begin = y0 - halo > 0 ? y0 - halo : 0;
	cuint y_end = y1 + halo < _HEIGHT ? y1 + halo : _HEIGHT;

	cuint z_begin = z0 > 0 ? z0 - halo : 0;
	cuint z_end = z1 < _DEPTH ? z1 : _DEPTH;

	// e.g. 3D array:
	/*
	* |1 2 3 4|, |11 12 13 14|
	* |5 6 7 8|  |15 16 17 18|
	*/

	// F: '1  11  2  12  3  13  4  14  5  15  6  16  7  17  8  18'
	// C: '1  5  2  6  3  7  4  8  11  15  12  16  13  17  14  18'

	/*
	*	// row major order
		index = ((x * max_y + y) * max_z) + z;
		// column major order
		index = ((z * max_y + y) * max_x) + x;

		// for row major order
		z = index % max_z;
		y = (index / max_z) % max_y;
		x = index / (max_z * max_y);

		// for column major order
		x = index % max_x;
		y = (index / max_x) % max_y;
		z = index / (max_x * max_y);
	*/
	int i = 0;
	for (int y = y_begin; y < y_end; y++)
	{
		for (int x = x_begin; x < x_end; x++)
		{
			for (int z = z_begin; z < z_end; z++)
			{
				dump[i] = vec->data()[((z * _HEIGHT + y) * _WIDTH) + x];
				i++;
			}
		}
	}

	return dump;
}

std::vector<int> chunkWork3D(cl::Context context, cl::Program program, cl::Device device, std::vector<int>* vec, cuint halo,
	cuint chunk_x, cuint chunk_y, cuint chunk_z, int err = 0)
{
	int chunkSize = (chunk_x + halo) * (chunk_y + halo) * (chunk_z + halo);

	int x = 0;
	int y = 0;
	int z = 0;

	// initial point to start of our vector
	for (int* pVec = vec->data(); *pVec < vec->size(); pVec += chunkSize)
	{
		// create buffers, and kernel
		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * chunkSize, pVec);
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * chunkSize, nullptr);

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

		cl::Kernel kernel(program, "doofus", &err);
		checkErr(err, "kernelling...");

		// fill in args of the user made kernel func in .cl
		kernel.setArg(0, inBuf);
		kernel.setArg(1, outBuf);

		kernel.setArg(2, x); // x0
		kernel.setArg(3, y); // y0
		kernel.setArg(4, z); // z0

		kernel.setArg(5, x + chunk_x); // w0
		kernel.setArg(6, y + chunk_y); // h0
		kernel.setArg(7, z + chunk_z); // d0

		cl::CommandQueue queue(context, device);

		// sets up where we to read the finished GPU data
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(chunkSize));
		checkErr(err, "ND Range kernel execution");

		// get array to send
		std::vector<int> dump = vecToFortran3D(vec, halo, x, x + chunk_x, y, y + chunk_y, z, z + chunk_z);

		// reads from GPU data from where it was set to based on NDRangeK
		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * chunkSize, dump.data()); // get data from device, and work on buffer
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
	// int* array_3d = new int[array_size]; // delete later

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
	
	for (auto &i:vec)
	{
		printf("%d", i);
	}

	printf("\n");
	cl::finish();
}