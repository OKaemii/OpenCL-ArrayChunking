#include "OCL.h"

bool OCL::checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		std::vector<std::string> possibleErrors = {
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE",
		"CL_INVALID_VALUE",
		"CL_INVALID_DEVICE_TYPE",
		"CL_INVALID_PLATFORM",
		"CL_INVALID_DEVICE",
		"CL_INVALID_CONTEXT",
		"CL_INVALID_QUEUE_PROPERTIES",
		"CL_INVALID_COMMAND_QUEUE",
		"CL_INVALID_HOST_PTR",
		"CL_INVALID_MEM_OBJECT",
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
		"CL_INVALID_IMAGE_SIZE",
		"CL_INVALID_SAMPLER",
		"CL_INVALID_BINARY",
		"CL_INVALID_BUILD_OPTIONS",
		"CL_INVALID_PROGRAM",
		"CL_INVALID_PROGRAM_EXECUTABLE",
		"CL_INVALID_KERNEL_NAME",
		"CL_INVALID_KERNEL_DEFINITION",
		"CL_INVALID_KERNEL",
		"CL_INVALID_ARG_INDEX",
		"CL_INVALID_ARG_VALUE",
		"CL_INVALID_ARG_SIZE",
		"CL_INVALID_KERNEL_ARGS",
		"CL_INVALID_WORK_DIMENSION",
		"CL_INVALID_WORK_GROUP_SIZE",
		"CL_INVALID_WORK_ITEM_SIZE",
		"CL_INVALID_GLOBAL_OFFSET",
		"CL_INVALID_EVENT_WAIT_LIST",
		"CL_INVALID_EVENT",
		"CL_INVALID_OPERATION",
		"CL_INVALID_GL_OBJECT",
		"CL_INVALID_BUFFER_SIZE",
		"CL_INVALID_MIP_LEVEL",
		"CL_INVALID_GLOBAL_WORK_SIZE",
		"CL_VERSION_1_0",
		"CL_FALSE",
		"CL_TRUE",
		"CL_PLATFORM_PROFILE",
		"CL_PLATFORM_VERSION",
		"CL_PLATFORM_NAME",
		"CL_PLATFORM_VENDOR",
		"CL_PLATFORM_EXTENSIONS",
		"CL_DEVICE_TYPE_DEFAULT",
		"CL_DEVICE_TYPE_CPU",
		"CL_DEVICE_TYPE_GPU",
		"CL_DEVICE_TYPE_ACCELERATOR",
		"CL_DEVICE_TYPE_ALL",
		"CL_DEVICE_TYPE",
		"CL_DEVICE_VENDOR_ID",
		"CL_DEVICE_MAX_COMPUTE_UNITS",
		"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",
		"CL_DEVICE_MAX_WORK_GROUP_SIZE",
		"CL_DEVICE_MAX_WORK_ITEM_SIZES",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHOR",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOA",
		"CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE",
		"CL_DEVICE_MAX_CLOCK_FREQUENCY",
		"CL_DEVICE_ADDRESS_BITS",
		"CL_DEVICE_MAX_READ_IMAGE_ARGS",
		"CL_DEVICE_MAX_WRITE_IMAGE_ARGS",
		"CL_DEVICE_MAX_MEM_ALLOC_SIZE",
		"CL_DEVICE_IMAGE2D_MAX_WIDTH",
		"CL_DEVICE_IMAGE2D_MAX_HEIGHT",
		"CL_DEVICE_IMAGE3D_MAX_WIDTH",
		"CL_DEVICE_IMAGE3D_MAX_HEIGHT",
		"CL_DEVICE_IMAGE3D_MAX_DEPTH",
		"CL_DEVICE_IMAGE_SUPPORT",
		"CL_DEVICE_MAX_PARAMETER_SIZE",
		"CL_DEVICE_MAX_SAMPLERS",
		"CL_DEVICE_MEM_BASE_ADDR_ALIGN",
		"CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE",
		"CL_DEVICE_SINGLE_FP_CONFIG",
		"CL_DEVICE_GLOBAL_MEM_CACHE_TYPE",
		"CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZ",
		"CL_DEVICE_GLOBAL_MEM_CACHE_SIZE",
		"CL_DEVICE_GLOBAL_MEM_SIZE",
		"CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE",
		"CL_DEVICE_MAX_CONSTANT_ARGS",
		"CL_DEVICE_LOCAL_MEM_TYPE",
		"CL_DEVICE_LOCAL_MEM_SIZE",
		"CL_DEVICE_ERROR_CORRECTION_SUPPORT",
		"CL_DEVICE_PROFILING_TIMER_RESOLUTION",
		"CL_DEVICE_ENDIAN_LITTLE",
		"CL_DEVICE_AVAILABLE",
		"CL_DEVICE_COMPILER_AVAILABLE",
		"CL_DEVICE_EXECUTION_CAPABILITIES",
		"CL_DEVICE_QUEUE_PROPERTIES",
		"CL_DEVICE_NAME",
		"CL_DEVICE_VENDOR",
		"CL_DRIVER_VERSION",
		"CL_DEVICE_PROFILE",
		"CL_DEVICE_VERSION",
		"CL_DEVICE_EXTENSIONS",
		"CL_DEVICE_PLATFORM",
		"CL_FP_DENORM",
		"CL_FP_INF_NAN",
		"CL_FP_ROUND_TO_NEAREST",
		"CL_FP_ROUND_TO_ZERO",
		"CL_FP_ROUND_TO_INF",
		"CL_FP_FMA",
		"CL_NONE",
		"CL_READ_ONLY_CACHE",
		"CL_READ_WRITE_CACHE",
		"CL_LOCAL",
		"CL_GLOBAL",
		"CL_EXEC_KERNEL",
		"CL_EXEC_NATIVE_KERNEL",
		"CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE",
		"CL_QUEUE_PROFILING_ENABLE",
		"CL_CONTEXT_REFERENCE_COUNT",
		"CL_CONTEXT_DEVICES",
		"CL_CONTEXT_PROPERTIES",
		"CL_CONTEXT_PLATFORM",
		"CL_QUEUE_CONTEXT",
		"CL_QUEUE_DEVICE",
		"CL_QUEUE_REFERENCE_COUNT",
		"CL_QUEUE_PROPERTIES",
		"CL_MEM_READ_WRITE,0)",
		"CL_MEM_WRITE_ONLY,1)",
		"CL_MEM_READ_ONLY,2)",
		"CL_MEM_USE_HOST_PTR,3)",
		"CL_MEM_ALLOC_HOST_PTR4)",
		"CL_MEM_COPY_HOST_PTR,5)",
		"CL_R",
		"CL_A",
		"CL_RG",
		"CL_RA",
		"CL_RGB",
		"CL_RGBA",
		"CL_BGRA",
		"CL_ARGB",
		"CL_INTENSITY",
		"CL_LUMINANCE",
		"CL_SNORM_INT8",
		"CL_SNORM_INT16",
		"CL_UNORM_INT8",
		"CL_UNORM_INT16",
		"CL_UNORM_SHORT_565",
		"CL_UNORM_SHORT_555",
		"CL_UNORM_INT_101010",
		"CL_SIGNED_INT8",
		"CL_SIGNED_INT16",
		"CL_SIGNED_INT32",
		"CL_UNSIGNED_INT8",
		"CL_UNSIGNED_INT16",
		"CL_UNSIGNED_INT32",
		"CL_HALF_FLOAT",
		"CL_FLOAT",
		"CL_MEM_OBJECT_BUFFER",
		"CL_MEM_OBJECT_IMAGE2D",
		"CL_MEM_OBJECT_IMAGE3D",
		"CL_MEM_TYPE",
		"CL_MEM_FLAGS",
		"CL_MEM_SIZE",
		"CL_MEM_HOST_PTR",
		"CL_MEM_MAP_COUNT",
		"CL_MEM_REFERENCE_COUNT",
		"CL_MEM_CONTEXT",
		"CL_IMAGE_FORMAT",
		"CL_IMAGE_ELEMENT_SIZE",
		"CL_IMAGE_ROW_PITCH",
		"CL_IMAGE_SLICE_PITCH",
		"CL_IMAGE_WIDTH",
		"CL_IMAGE_HEIGHT",
		"CL_IMAGE_DEPTH",
		"CL_ADDRESS_NONE",
		"CL_ADDRESS_CLAMP_TO_EDGE",
		"CL_ADDRESS_CLAMP",
		"CL_ADDRESS_REPEAT",
		"CL_FILTER_NEAREST",
		"CL_FILTER_LINEAR",
		"CL_SAMPLER_REFERENCE_COUNT",
		"CL_SAMPLER_CONTEXT",
		"CL_SAMPLER_NORMALIZED_COORDS",
		"CL_SAMPLER_ADDRESSING_MODE",
		"CL_SAMPLER_FILTER_MODE",
		"CL_MAP_READ",
		"CL_MAP_WRITE",
		"CL_PROGRAM_REFERENCE_COUNT",
		"CL_PROGRAM_CONTEXT",
		"CL_PROGRAM_NUM_DEVICES",
		"CL_PROGRAM_DEVICES",
		"CL_PROGRAM_SOURCE",
		"CL_PROGRAM_BINARY_SIZES",
		"CL_PROGRAM_BINARIES",
		"CL_PROGRAM_BUILD_STATUS",
		"CL_PROGRAM_BUILD_OPTIONS",
		"CL_PROGRAM_BUILD_LOG",
		"CL_BUILD_SUCCESS",
		"CL_BUILD_NONE",
		"CL_BUILD_ERROR",
		"CL_BUILD_IN_PROGRESS",
		"CL_KERNEL_FUNCTION_NAME",
		"CL_KERNEL_NUM_ARGS",
		"CL_KERNEL_REFERENCE_COUNT",
		"CL_KERNEL_CONTEXT",
		"CL_KERNEL_PROGRAM",
		"CL_KERNEL_WORK_GROUP_SIZE",
		"CL_KERNEL_COMPILE_WORK_GROUP_SIZE",
		"CL_KERNEL_LOCAL_MEM_SIZE",
		"CL_EVENT_COMMAND_QUEUE",
		"CL_EVENT_COMMAND_TYPE",
		"CL_EVENT_REFERENCE_COUNT",
		"CL_EVENT_COMMAND_EXECUTION_STATUS",
		"CL_COMMAND_NDRANGE_KERNEL",
		"CL_COMMAND_TASK",
		"CL_COMMAND_NATIVE_KERNEL",
		"CL_COMMAND_READ_BUFFER",
		"CL_COMMAND_WRITE_BUFFE",
		"CL_COMMAND_COPY_BUFFER",
		"CL_COMMAND_READ_IMAGE",
		"CL_COMMAND_WRITE_IMAGE",
		"CL_COMMAND_COPY_IMAGE",
		"CL_COMMAND_COPY_IMAGE_TO_BUFFER",
		"CL_COMMAND_COPY_BUFFER_TO_IMAGE",
		"CL_COMMAND_MAP_BUFFER",
		"CL_COMMAND_MAP_IMAGE",
		"CL_COMMAND_UNMAP_MEM_OBJECT",
		"CL_COMMAND_MARKER",
		"CL_COMMAND_ACQUIRE_GL_OBJECTS",
		"CL_COMMAND_RELEASE_GL_OBJECTS",
		"CL_COMPLETE",
		"CL_RUNNING",
		"CL_SUBMITTED",
		"CL_QUEUED",
		"CL_PROFILING_COMMAND_QUEUED",
		"CL_PROFILING_COMMAND_SUBMIT",
		"CL_PROFILING_COMMAND_START",
		"CL_PROFILING_COMMAND_EN",
		"UNKNOWN ERROR OCCURED, UPDATE LIST?"
		};

		int error = (-err >= 0 && -err < possibleErrors.size() - 1) ? -err: possibleErrors.size();

		printf("%s, \n %s\n", name, possibleErrors.data()[error]);
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

		//int i2 = i + chunkSize;

		//int x1 = i2 % dataToUse._WIDTH;
		//int temp_index2 = i2 / dataToUse._WIDTH;
		//int y1 = temp_index2 % dataToUse._HEIGHT;
		//int z1 = temp_index2 / dataToUse._HEIGHT;

		// printf("i: %d | (%d, %d, %d) -> (%d, %d, %d)\n", i2, x, y, z, x1, y1, z1);

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