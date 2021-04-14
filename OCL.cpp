#include "OCL.h"
#include <omp.h>

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

OCL::OCL(int width, int height, int depth, int halo)
{
	printf("OpenCL 1.2 Object.\n");
	// create platform to accommodate for devices
	err = cl::Platform::get(&platforms);
	checkErr(err, "trouble with acquring platform data. :(");

	// struct to contain dimension of main array
	dataToUse._WIDTH = width;
	dataToUse._HEIGHT = height;
	dataToUse._DEPTH = depth;
	dataToUse._HALO = halo;

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
	// free(arrMainBody);
	free(haloed_arrMainBody);
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
	std::ifstream kernelFile("EvalKernel.cl");
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

	cl::Kernel kernel(program, "super", &err);
	checkErr(err, "kernelling...");
	oclBode.kernel = kernel;

	printf("Building array...\n");

	//int arrMainBodySize = dataToUse._WIDTH * dataToUse._HEIGHT * dataToUse._DEPTH;
	//OCL::arrMainBody = (float*)malloc(sizeof(float) * arrMainBodySize);

	//// init_domain
	//// std::fill(arrMainBody[0], arrMainBody[arrMainBodySize-1], 1.f); // method 1
	//// std::iota(std::begin(vec), std::end(vec), 0.f);	// method 2
	//for (int i = 0; i < arrMainBodySize; i++)
	//{
	//	arrMainBody[i] = 0;
	//}
	//for (int i = 0; i < arrMainBodySize; i++)
	//{
	//	arrMainBody[i] = 1.f;
	//}

	// halo defined from parameter
	int halo = dataToUse._HALO;
	int halo_size = halo * 2;

	// new vector to have original vec with halo
	long haloed_arrMainBodySize = (dataToUse._WIDTH + halo_size) * (dataToUse._HEIGHT + halo_size) * (dataToUse._DEPTH + halo_size);
	OCL::haloed_arrMainBody = (float*)malloc(sizeof(float) * haloed_arrMainBodySize);
	memset(haloed_arrMainBody, 0, haloed_arrMainBodySize * sizeof(float));

	//// new dimensions with haloes
	//int haloed_width = (dataToUse._WIDTH + halo_size);
	//int haloed_height = (dataToUse._HEIGHT + halo_size);
	//int haloed_depth = (dataToUse._DEPTH + halo_size);
}

void OCL::run(int chunkSlice_x, int chunkSlice_y, int chunkSlice_z)
{
	int halo = dataToUse._HALO;
	// halo defined from parameter
	int halo_size = dataToUse._HALO * 2;
	// new dimensions with haloes
	int haloed_width = (dataToUse._WIDTH + halo_size);
	int haloed_height = (dataToUse._HEIGHT + halo_size);
	int haloed_depth = (dataToUse._DEPTH + halo_size);

	int x_chunk = dataToUse._WIDTH / chunkSlice_x;
	int y_chunk = dataToUse._HEIGHT / chunkSlice_y;
	int z_chunk = dataToUse._DEPTH / chunkSlice_z;

	// printf("using %d chunks | chunk dim: (%d, %d, %d)\n", (chunkSlice_x * chunkSlice_y * chunkSlice_z), dataToUse._WIDTH / chunkSlice_x + dataToUse._HALO, dataToUse._HEIGHT / chunkSlice_y + dataToUse._HALO, dataToUse._DEPTH / chunkSlice_z + dataToUse._HALO);
	// printf("validation(%d-%d-%d)==(0,0,0)\n", dataToUse._WIDTH % x_chunk, dataToUse._HEIGHT % y_chunk, dataToUse._DEPTH % z_chunk);

	// contain main array into haloed vector to give it haloes
	#pragma omp parallel for num_threads(32) collapse(3)
	for (int z = 0; z < haloed_depth; z++)
	{
		for (int y = 0; y < haloed_height; y++)
		{
			for (int x = 0; x < haloed_width; x++)
			{
				// index to map haloed vector
				int haloed_arr_id = x + haloed_width * (y + haloed_height * z);

				// domain value
				if ((x >= halo && x < dataToUse._WIDTH) && (y >= halo && y < dataToUse._HEIGHT) && (z >= halo && z < dataToUse._DEPTH))
				{
					haloed_arrMainBody[haloed_arr_id] = 0;
				}

				// init_inflow_plane and init_outflow_plane
				if ((x >= 0 && x < halo) && (y >= halo && y < dataToUse._HEIGHT) && (z >= halo && z < dataToUse._DEPTH))
				{
					haloed_arrMainBody[haloed_arr_id] = (float) (z - halo) / dataToUse._DEPTH;

					// outflow
					int from_index = (x + 1) + dataToUse._WIDTH * (y + dataToUse._HEIGHT * z);
					haloed_arrMainBody[from_index] = haloed_arrMainBody[haloed_arr_id];
				}

				// init_sideflow_halos
				if ((x >= halo && x < dataToUse._WIDTH) && (y >= 0 && y < halo) && (z >= halo && z < dataToUse._DEPTH))
				{
					int from_index = x + dataToUse._WIDTH * (dataToUse._HEIGHT + dataToUse._HEIGHT * z);
					haloed_arrMainBody[haloed_arr_id] = haloed_arrMainBody[from_index];
				}

				if ((x >= halo && x < dataToUse._WIDTH) && (y >= dataToUse._HEIGHT + halo) && (z >= halo && z < dataToUse._DEPTH))
				{
					int from_index = x + dataToUse._WIDTH * (halo + dataToUse._HEIGHT * z);
					haloed_arrMainBody[haloed_arr_id] = haloed_arrMainBody[from_index];
				}

				// init_top_bottom_halos
				if ((x >= halo && x < dataToUse._WIDTH) && (y >= halo && y < dataToUse._HEIGHT) && (z >= 0 && z < halo))
				{
					int from_index = x + dataToUse._WIDTH * (y + dataToUse._HEIGHT * halo);
					haloed_arrMainBody[haloed_arr_id] = haloed_arrMainBody[from_index];
				}

				if ((x >= halo && x < dataToUse._WIDTH) && (y >= halo && y < dataToUse._HEIGHT) && (z >= dataToUse._DEPTH + halo))
				{
					int from_index = x + dataToUse._WIDTH * (y + dataToUse._HEIGHT * (dataToUse._DEPTH + halo));
					haloed_arrMainBody[haloed_arr_id] = haloed_arrMainBody[from_index];
				}
			}
		}
	}

	// dimensions for chunked array
	int x_chunk_haloed = x_chunk + halo_size;
	int y_chunk_haloed = y_chunk + halo_size;
	int z_chunk_haloed = z_chunk + halo_size;

	// get the first co-ordinates of every chunk
	int ddd = dataToUse._WIDTH % x_chunk;
	for (int z_offset = 0; z_offset + ddd < dataToUse._DEPTH; z_offset += z_chunk)
	{
		for (int y_offset = 0; y_offset + ddd < dataToUse._HEIGHT; y_offset += y_chunk)
		{
			for (int x_offset = 0; x_offset + ddd < dataToUse._WIDTH; x_offset += x_chunk)
			{
				// chunk main body array to send for device + haloed
				int chunkedBodySize = x_chunk_haloed * y_chunk_haloed * z_chunk_haloed;
				float* chunkedBody = (float*)malloc(sizeof(float) * chunkedBodySize);

				// populate chunk with data + haloes
				#pragma omp parallel for num_threads(32) collapse(3)
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
							// host side index of our chunk
							int host_index = x + x_chunk_haloed * (y + y_chunk_haloed * z);

							chunkedBody[host_index] = haloed_arrMainBody[index];
						}
					}
				}

				// sending to device...
				int sizeofDataToPass = chunkedBodySize;

				// buffer to receive array from device
				float* backBuff = new float[chunkedBodySize * sizeof(float)];

				// map chunked array from host to the array for device
				// create buffers, and kernel
				//																			 CL_MEM_COPY_HOST_PTR
				cl::Buffer inBuf(oclBode.context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, chunkedBodySize * sizeof(float), chunkedBody);
				cl::Buffer outBuf(oclBode.context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, chunkedBodySize * sizeof(float), nullptr);

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

				oclBode.kernel.setArg(10, dataToUse._HALO);

				cl::Event event;

				// sets up where we to read the finished GPU data
				// cl::NDRange(sizeofDataToPass), not in bytes, defined in terms of every element and not bytes.
				err = oclBode.queue.enqueueNDRangeKernel(oclBode.kernel, cl::NullRange, cl::NDRange(sizeofDataToPass), cl::NullRange, nullptr, &event); // global, local range here
				checkErr(err, "ND Range kernel execution");

				// NDRange has no CL_True, so make wait event to block everything until done
				event.wait();

				// reads from GPU data from where it was set to based on NDRangeK
				// get data from device, and work on buffer
				// CL_TRUE block until everything is done
				err = oclBode.queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, chunkedBodySize * sizeof(float), backBuff);

				// no longer need chunkedbody
				// free chunked body from its suffgering
				free(chunkedBody);

				checkErr(err, "Reading buffer...");

				#pragma omp parallel for num_threads(32) collapse(3)
				for (int z = halo; z < z_chunk_haloed - halo; z++)
				{
					for (int y = halo; y < y_chunk_haloed - halo; y++)
					{
						for (int x = halo; x < x_chunk_haloed - halo; x++)
						{
							// where in our main body haloed array is our chunk is
							int index = (x + x_offset) + haloed_width * ((y + y_offset) + haloed_height * (z + z_offset));

							// index for the device side chunk
							int device_index = x + x_chunk_haloed * (y + y_chunk_haloed * z);

							haloed_arrMainBody[index] = backBuff[device_index];
						}
					}
				}

				// future work
				// current done naively, done point by point
				// we know in general problem not continguos
				// large chunks large contiguous sub parts, could do block copies
				// no longer need buffer
				// direct mem access (DMA), thus faster

				// if memcopy no work, could use void pointers and then cast it back to float
				delete[] backBuff;
			}
		}

	}

	// finished all chunking process
	// end cl use
	cl::finish();

	//// output array in sliced fasion
	//for (int z = 0; z < haloed_depth; z++)
	//{
	//	for (int y = 0; y < haloed_height; y++)
	//	{
	//		for (int x = 0; x < haloed_width; x++)
	//		{
	//			int index = x + haloed_width * (y + haloed_height * z);
	//			printf(" %.2f ", haloed_arrMainBody[index]);
	//			index++;
	//		}
	//		printf("\n");
	//	}
	//	printf("\n");
	//}
}