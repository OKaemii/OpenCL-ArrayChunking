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
	std::cout << "Build Status:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[device_id]) << std::endl;
	std::cout << "Build Options:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[device_id]) << std::endl;
	std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[device_id]) << std::endl;

	cl::Kernel kernel(program, "doofus", &err);
	checkErr(err, "kernelling...");
	oclBode.kernel = kernel;
}

void OCL::run()
{
	data_struct dataToUse;

	std::vector<int> arrMainBody(dataToUse._WIDTH * dataToUse._HEIGHT * dataToUse._DEPTH);
	// fill array main body
	std::fill(arrMainBody.begin(), arrMainBody.end(), 1);

	int halo = 0;
	int vecSize = arrMainBody.size();
	int chunkSize = dataToUse.chunk_x * dataToUse.chunk_y * dataToUse.chunk_z;

	// for halo subarrays
	std::vector<int>::const_iterator starting_index;
	std::vector<int>::const_iterator ending_index;

	printf("chunksize = %d\n", chunkSize);

	int progress = 0;
	printf("size of array to chunk: %d\n", arrMainBody.size());
	// initial point to start of our vector
	for (int i = 0; i < arrMainBody.size(); i += chunkSize)
	{
		// find start and end positions to send
		int halo_range_left = i - halo;
		int halo_range_right = (i + chunkSize) + halo;

		std::vector<int> left_attachment;
		std::vector<int> right_attachment;
		std::vector<int>::const_iterator starting_index;
		std::vector<int>::const_iterator ending_index;

		// it needs to get other boundary
		if (halo_range_left < 0)
		{
			left_attachment.insert(left_attachment.begin(), arrMainBody.end() + halo_range_left, arrMainBody.end());
			halo_range_left = 0;
		}
		if (halo_range_right > vecSize)
		{
			right_attachment.insert(right_attachment.begin(), arrMainBody.begin(), arrMainBody.begin() + (halo_range_right - vecSize));
			halo_range_right = vecSize;
		}
		starting_index = arrMainBody.begin() + halo_range_left;
		ending_index = arrMainBody.begin() + halo_range_right;
		std::vector<int> vectorToEvaluate(starting_index, ending_index);

		// append out of bound vectors if any
		vectorToEvaluate.insert(vectorToEvaluate.begin(), left_attachment.begin(), left_attachment.end());
		vectorToEvaluate.insert(vectorToEvaluate.end(), right_attachment.begin(), right_attachment.end());

		int sizeofDataToPass = sizeof(int) * vectorToEvaluate.size();

		// create buffers, and kernel
		cl::Buffer inBuf(oclBode.context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeofDataToPass, &vectorToEvaluate.data()[0]);
		cl::Buffer outBuf(oclBode.context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeofDataToPass, nullptr);

		int x = i % dataToUse._WIDTH;
		int temp_index = i / dataToUse._WIDTH;
		int y = temp_index % dataToUse._HEIGHT;
		int z = temp_index / dataToUse._HEIGHT;

		// fill in args of the user made kernel func in .cl
		oclBode.kernel.setArg(0, inBuf);
		oclBode.kernel.setArg(1, outBuf);

		oclBode.kernel.setArg(2, i);
		oclBode.kernel.setArg(3, chunkSize);
		oclBode.kernel.setArg(4, halo);

		cl_event wait;
		cl_int status;

#pragma warning(disable : 4996)
		status = clEnqueueMarker(oclBode.queue(), &wait);
		if (status != CL_SUCCESS)
		{
			printf("Enqueue marker failed...\n");
		}

		// sets up where we to read the finished GPU data
		err = oclBode.queue.enqueueNDRangeKernel(oclBode.kernel, cl::NullRange, cl::NDRange(sizeofDataToPass)); // global, local range here
		checkErr(err, "ND Range kernel execution");

		// block until everything is done
		status = clWaitForEvents(1, &wait);
		if (status != CL_SUCCESS)
		{
			printf("Wait failed?\n");
		}

		// reads from GPU data from where it was set to based on NDRangeK
		// get data from device, and work on buffer
		err = oclBode.queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeofDataToPass, &vectorToEvaluate.data()[0]);
		checkErr(err, "Reading buffer...");

		// we only want the non halo part
		std::copy(vectorToEvaluate.begin() + halo, vectorToEvaluate.begin() + halo + chunkSize, arrMainBody.begin() + i);


		progress += chunkSize;
	}

	for (auto& i : arrMainBody)
	{
		printf("%d", i);
	}

	printf("\n");
	cl::finish();
}