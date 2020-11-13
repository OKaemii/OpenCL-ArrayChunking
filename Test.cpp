#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include<CL/cl.hpp>
#include <iostream>
#include <fstream>


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