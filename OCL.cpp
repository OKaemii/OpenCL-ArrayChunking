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

struct data_struct
{
	// make 3D array
	int _WIDTH = 10;
	int _HEIGHT = 10;
	int _DEPTH = 10;
};

// struct to contain OpenCL kernels, program, contect, etc.
ocl_body oclBode;

// struct to contain dimension of main array
data_struct dataToUse;

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

	std::cout << "Build Status:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[device_id]) << std::endl;
	std::cout << "Build Options:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[device_id]) << std::endl;
	std::cout << "Build Log:\t " << oclBode.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[device_id]) << std::endl;

	cl::Kernel kernel(program, "doofus", &err);
	checkErr(err, "kernelling...");
	oclBode.kernel = kernel;
}

void OCL::run(int chunkSlice_x, int chunkSlice_y, int chunkSlice_z, int halo)
{
	std::vector<int> arrMainBody(dataToUse._WIDTH * dataToUse._HEIGHT * dataToUse._DEPTH);

	// fill array main body
	std::fill(arrMainBody.begin(), arrMainBody.end(), 1); // method 1
	// std::iota(std::begin(vec), std::end(vec), 0);	// method 2


	// halo defined from parameter
	int halo_size = halo * 2;

	// new vector to have original vec with halo
	std::vector<int> haloed_arrMainBody((dataToUse._WIDTH + halo_size) * (dataToUse._HEIGHT + halo_size) * (dataToUse._DEPTH + halo_size));

	// new dimensions with haloes
	int haloed_width = (dataToUse._WIDTH + halo_size);
	int haloed_height = (dataToUse._HEIGHT + halo_size);
	int haloed_depth = (dataToUse._DEPTH + halo_size);

	// index to map haloed vector with main array
	int original_index = 0;

	// contain main array into haloed vector to give it haloes
	for (int z = halo; z < haloed_depth - halo; z++)
	{
		for (int y = halo; y < haloed_height - halo; y++)
		{
			for (int x = halo; x < haloed_width - halo; x++)
			{
				// get the appropriate mapping for haloed vec, and main vec
				int index = x + haloed_width * (y + haloed_height * z);
				haloed_arrMainBody[index] = arrMainBody[original_index];
				original_index++;
			}
		}
	}

	// chunk main array not haloed, to give us cords for where starting haloed chunks will start
	int x_chunk = dataToUse._WIDTH;
	int y_chunk = dataToUse._HEIGHT;
	int z_chunk = dataToUse._DEPTH;

	// make sure all these chunks to be are whole numbers only, if not, do not chunk
	if ((dataToUse._WIDTH % chunkSlice_x == 0) && (dataToUse._HEIGHT % chunkSlice_y == 0) && (dataToUse._DEPTH % chunkSlice_z == 0))
	{
		x_chunk = dataToUse._WIDTH / chunkSlice_x;
		y_chunk = dataToUse._HEIGHT / chunkSlice_y;
		z_chunk = dataToUse._DEPTH / chunkSlice_z;

		chunkSlice_x = chunkSlice_y = chunkSlice_z = 1;
	}

	printf("using %d chunks.\n",(chunkSlice_x* chunkSlice_y* chunkSlice_z));

	// dimensions for chunked array
	int x_chunk_haloed = x_chunk + halo_size;
	int y_chunk_haloed = y_chunk + halo_size;
	int z_chunk_haloed = z_chunk + halo_size;

	// get the first co-ordinates of every chunk
	for (int z_offset = 0; z_offset < dataToUse._DEPTH; z_offset += z_chunk)
	{
		for (int y_offset = 0; y_offset < dataToUse._HEIGHT; y_offset += y_chunk)
		{
			for (int x_offset = 0; x_offset < dataToUse._WIDTH; x_offset += x_chunk)
			{
				// chunk main body array to send for device + haloed
				std::vector<int> chunkedBody(x_chunk_haloed * y_chunk_haloed * z_chunk_haloed);

				// host side index of our chunk
				int host_index = 0;
				
				// populate chunk with data + haloes
				for (int z = 0; z < z_chunk_haloed; z++)
				{
					for (int y = 0; y < y_chunk_haloed; y++)
					{
						for (int x = 0; x < x_chunk_haloed; x++)
						{
							// index of the haloed main body
							int index = (x + x_offset) + haloed_width * ((y + y_offset) + haloed_height * (z + z_offset));
							
							// assign from haloed main ved to chunk body
							// with coordinates where each chunk starts
							chunkedBody[host_index] = haloed_arrMainBody[index];
							host_index++;
						}
					}
				}
				
				// sending to device...
				int sizeofDataToPass = chunkedBody.size();
				int sizeofDataToPassinBytes = chunkedBody.size() * sizeof(int);

				// buffer to receive array from device
				std::vector<int> backBuff(chunkedBody.size());

				// map chunked array from host to the array for device
				// create buffers, and kernel
				cl::Buffer inBuf(oclBode.context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeofDataToPassinBytes, &chunkedBody[0]);
				cl::Buffer outBuf(oclBode.context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeofDataToPassinBytes, nullptr);

				// fill in args of the user made kernel func in .cl
				oclBode.kernel.setArg(0, inBuf);
				oclBode.kernel.setArg(1, outBuf);

				// haloed chunk dimensions
				oclBode.kernel.setArg(2, x_chunk_haloed);
				oclBode.kernel.setArg(3, y_chunk_haloed);

				// where we currently are in the haloed array
				oclBode.kernel.setArg(4, x_offset);
				oclBode.kernel.setArg(5, y_offset);
				oclBode.kernel.setArg(6, z_offset);

				// dimensions of our haloed main body array
				oclBode.kernel.setArg(7, haloed_width);
				oclBode.kernel.setArg(8, haloed_height);
				oclBode.kernel.setArg(9, haloed_depth);

				cl::Event event;
				cl_int status;

				// sets up where we to read the finished GPU data
				// cl::NDRange(sizeofDataToPass), not in bytes, defined in terms of every element and not bytes.
				err = oclBode.queue.enqueueNDRangeKernel(oclBode.kernel, cl::NullRange, cl::NDRange(sizeofDataToPass), cl::NullRange, nullptr, &event); // global, local range here
				checkErr(err, "ND Range kernel execution");

				// NDRange has no CL_True, so make wait event to block everything until done
				event.wait();

				// reads from GPU data from where it was set to based on NDRangeK
				// get data from device, and work on buffer
				// CL_TRUE block until everything is done
				err = oclBode.queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, sizeofDataToPassinBytes, &backBuff[0]);
				checkErr(err, "Reading buffer...");

				// map result from device back to host side main haloed body
				for (int z = halo; z < z_chunk_haloed - halo; z++)
				{
					for (int y = halo; y < y_chunk_haloed - halo; y++)
					{
						for (int x = halo; x < x_chunk_haloed - halo; x++)
						{
							// where in our main body haloed array is our chunk is
							int index = (x + x_offset) + haloed_width * ((y + y_offset) + haloed_height * (z + z_offset));

							// index for the device side chunk
							int device_index = x + x_chunk_haloed * (y + y_chunk_haloed * x);

							haloed_arrMainBody[index] = backBuff[device_index];
						}
					}
				}

			}
		}

	}

	// finished all chunking process
	// end cl use
	cl::finish();

	// output array in sliced fasion
	for (int z = 0; z < haloed_depth; z++)
	{
		for (int y = 0; y < haloed_height; y++)
		{
			for (int x = 0; x < haloed_width; x++)
			{
				int index = x + haloed_width * (y + haloed_height * z);
				printf("%d", haloed_arrMainBody[index]);
				index++;
			}
			printf("\n");
		}
		printf("\n");
	}
}