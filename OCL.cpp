#include "OCL.h"

bool OCL::checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		printf("%s\n", name);
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		return true;
	}

	return false;
}

struct ocl_body
{
	// platform used
	cl::Platform platform;

	// device object used
	cl::Device device;

	// context menu
	cl::Context context;

	// command queue for something
	cl::CommandQueue queue;

	// something here
	cl::Program program;

	// our super kernel
	cl::Kernel kernel;

	// for our wait event
	cl::Event event;

	// for error log
	cl_int err;
};

ocl_body oclBode;


OCL::OCL()
{
	printf("OpenCL 1.2 Object.\n");
	// create platform to accommodate for devices
	err = cl::Platform::get(&platforms);
	checkErr(err, "trouble with acquring platform data. :(");

	// list platforms
	{
		int i = 0;
		for (auto& p : platforms)
		{
			printf("platform[%d]: %s\n", i, p.getInfo<CL_PLATFORM_NAME>().c_str());
			i++;

			auto err = p.getDevices(CL_DEVICE_TYPE_GPU, &devices);

			// list devices for each platform
			checkErr(err, "Error: Device probably unsupported by OpenCl.");
			{
				int j = 0;
				for (auto& d : devices)
				{
					printf("\t- devices[%d]: %s\n", j, d.getInfo<CL_DEVICE_NAME>().c_str());
					j++;
				}
			}
		}

	}
}

OCL::~OCL()
{
	// should probs do something
	// but cl.h seems to have something for it already?
	// another time perhaps
}

struct data_struct
{
	// make 3D array
	int _WIDTH = 30;
	int _HEIGHT = 30;
	int _DEPTH = 30;

	// 3D chunky boii
	int chunk_x = 10;
	int chunk_y = 10;
	int chunk_z = 10;
};

void OCL::init(int platform_id, int device_id)
{
	printf("\n\n");

	// create platform to accommodate for devices
	err = cl::Platform::get(&platforms);
	checkErr(err, "trouble with acquring platform data. :(");

	cl::Platform platform = platforms[platform_id];
	oclBode.platform = platform;

	// print the device selected
	printf("using platform: %s\n", platform.getInfo<CL_PLATFORM_NAME>().c_str());

	// get GPU devices; could also CPU with CL_DEVICE_TYPE_CPU
	auto err = platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	checkErr(err, "trouble with getting GPU devices. :(");

	cl::Device device = devices[device_id];
	oclBode.device = device;

	// print the device selected
	printf("using device: %s\n", device.getInfo<CL_DEVICE_NAME>().c_str());

	// create context from device
	cl::Context context = cl::Context(device);
	oclBode.context = context;

	// command queue to use for OpenCL command executions
	try
	{
		//queue = cl::CommandQueue(context, devices[device_id], 0, &err);
		cl::CommandQueue queue = cl::CommandQueue(context, device, 0, &err);
		oclBode.queue = queue;
	}
	catch (cl_int err)
	{
		checkErr(err, "error occured at command queue");
	}


	printf("initialising...\n");
	// read in kernel file as source
	std::ifstream kernelFile("ProcArray.cl");
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
	const cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.size() + 1));

	// create program from both context, and source
	cl::Program program(context, sources);
	err = program.build(devices, "-cl-std=CL1.2");
	checkErr(err, "building program...");

	oclBode.program = program;

	// new and improved build errors
	printf("%s\n", oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str());

	printf("Program built successfully!\n");
	std::cout << "Build Status:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[device_id]) << std::endl;
	std::cout << "Build Options:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[device_id]) << std::endl;
	std::cout << "Build Log:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[device_id]) << std::endl;

	cl::Kernel kernel(program, "doofus", &err);
	checkErr(err, "kernelling...");
	oclBode.kernel = kernel;
}

void OCL::run()
{
	data_struct dataToUse;

	std::vector<int> arrMainBody(dataToUse._WIDTH * dataToUse._HEIGHT * dataToUse._DEPTH);
	std::vector<int> chunkedBody(dataToUse.chunk_x * dataToUse.chunk_y * dataToUse.chunk_z);

	// fill array main body
	std::fill(arrMainBody.begin(), arrMainBody.end(), 1);

	// assumuing shape to be square (big assumption)
	// just doing first chunk for now!!!					!IMPORTANT!
	int current_chunk = 0;

	// offset to get correct coordinate mapping from chunk to main bode
	int x_offset = current_chunk;
	int y_offset = current_chunk;
	int z_offset = current_chunk;

	// index for chunked body
	int i = 0;

	// go through depth data
	for (int z = current_chunk; z < dataToUse.chunk_z + current_chunk; z++)
	{
		// go through height
		for (int y = current_chunk; y < dataToUse.chunk_y + current_chunk; y++)
		{
			// go through width
			for (int x = current_chunk; x < dataToUse.chunk_x + current_chunk; x++)
			{
				// get starting index of data location
				int index = x + dataToUse._WIDTH * (y + dataToUse._HEIGHT * z);
				// map chunked location to main body
				chunkedBody.data()[i] = arrMainBody.data()[index];
				i++;
			}
		}
	}

	int sizeofDataToPass = chunkedBody.size();
	printf("size: %d\n", chunkedBody.size());

	// create buffers, and kernel
	cl::Buffer inBuf(oclBode.context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeofDataToPass, &chunkedBody[0]);
	cl::Buffer outBuf(oclBode.context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeofDataToPass, nullptr);

	// fill in args of the user made kernel func in .cl
	oclBode.kernel.setArg(0, inBuf);
	oclBode.kernel.setArg(1, outBuf);

	oclBode.kernel.setArg(2, dataToUse.chunk_x);
	oclBode.kernel.setArg(3, dataToUse.chunk_y);
	oclBode.kernel.setArg(4, dataToUse.chunk_z);

	oclBode.kernel.setArg(5, x_offset);
	oclBode.kernel.setArg(6, y_offset);
	oclBode.kernel.setArg(7, z_offset);

	cl::Event event;
	cl_int status;

	// sets up where we to read the finished GPU data
	err = oclBode.queue.enqueueNDRangeKernel(oclBode.kernel, cl::NullRange, cl::NDRange(sizeofDataToPass), cl::NullRange, nullptr, &event); // global, local range here
	checkErr(err, "ND Range kernel execution");

//#pragma warning(disable : 4996)
//	status = clEnqueueMarker(oclBode.queue(), &wait);
//	if (status != CL_SUCCESS)
//	{
//		printf("Enqueue marker failed...\n");
//	}

	// NDRange has no CL_True, so make wait event to block everything until done
	event.wait();

	// reads from GPU data from where it was set to based on NDRangeK
	// get data from device, and work on buffer
	// CL_TRUE block until everything is done
	err = oclBode.queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, sizeofDataToPass, &chunkedBody[0]);
	checkErr(err, "Reading buffer...");

	
	//status = clWaitForEvents(1, wait.wait());
	//if (status != CL_SUCCESS)
	//{
	//	printf("Wait failed?\n");
	//}

	printf("received from buffer\n");
	for (auto& i : chunkedBody)
	{
		printf("%d", i);
	}
	printf("\nend of stream\n\n");


	// we want to take this 3d array, and put it back to where it belongs in our main body
	// go through depth data
	i = 0;
	for (int z = 0; z < dataToUse.chunk_z; z++)
	{
		// go through height
		for (int y = 0; y < dataToUse.chunk_y; y++)
		{
			// go through width
			for (int x = 0; x < dataToUse.chunk_x; x++)
			{
				// get starting index of data location
				int index = (x + x_offset) + dataToUse._WIDTH * ((y + y_offset) + dataToUse._HEIGHT * (z + z_offset));
				// map chunked location to main body
				arrMainBody.data()[index] = chunkedBody.data()[i];
				i++;
			}
		}
	}

	printf("output of entire array\n");
	for (auto& i : arrMainBody)
	{
		printf("%d", i);
	}

	printf("\n");
	cl::finish();

}