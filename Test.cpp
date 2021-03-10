//#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
//
//#include <CL/cl.hpp>
//#include <iostream>
//#include <fstream>
//#include <string>

//typedef const unsigned int cuint;
//
//cuint _WIDTH;
//cuint _HEIGHT;
//cuint _DEPTH;
//
//inline void checkErr(cl_int err, const char *name)
//{
//	if (err != CL_SUCCESS)
//	{
//		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
//		//exit(EXIT_FAILURE);
//	}
//}
//
//std::vector<int> chunkWork1D(cl::Context context, cl::Program program, cl::Device device, std::vector<int> *vec, int chunkSize, int err = 0)
//{
//	// initial point to start of our vector
//	cl::Event event;
//	for (int *pVec = vec->data(); *pVec < vec->size(); pVec += chunkSize)
//	{
//		// create buffers, and kernel
//		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * chunkSize, pVec);
//		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * chunkSize, nullptr);
//		cl::Kernel kernel(program, "ProcArray", &err);
//		checkErr(err, "kernelling...");
//
//		// fill in args of the user made kernel func in .cl
//		kernel.setArg(0, inBuf);
//		kernel.setArg(1, outBuf);
//
//		cl::CommandQueue queue(context, device);
//
//		// sets up where we to read the finished GPU data
//		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(chunkSize), cl::NDRange(1), NULL, &event);
//		checkErr(err, "ND Range kernel execution");
//		event.wait();
//		// reads from GPU data from where it was set to based on NDRangeK
//		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * chunkSize, pVec); // get data from device, and work on buffer
//		checkErr(err, "Reading buffer...");
//	}
//
//	return *vec;
//}
//
//std::vector<int> chunkWork3D_old(cl::Context context, cl::Program program, cl::Device device, std::vector<int> *vec, cuint a_size,
//								 cuint chunk_x, cuint chunk_y, cuint chunk_z, int err = 0)
//{
//	char calcToPerform[] = "calcNon";
//	int chunkSize = chunk_x * chunk_y * chunk_z;
//	int x = 0;
//	int y = 0;
//	int z = 0;
//
//	// initial point to start of our vector
//	for (int *pVec = vec->data(); *pVec < vec->size(); pVec += chunkSize)
//	{
//		// create buffers, and kernel
//		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * chunkSize, pVec);
//		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * chunkSize, nullptr);
//
//		if (x >= _WIDTH)
//		{
//			x = 0;
//			z += chunk_z;
//		}
//
//		if (z >= _DEPTH)
//		{
//			z = 0;
//			y += chunk_y;
//		}
//
//		if (y >= _HEIGHT)
//		{
//			y = 0;
//		}
//
//		x += chunk_x;
//
//		cl::Kernel kernel(program, calcToPerform, &err);
//		checkErr(err, "kernelling...");
//
//		// fill in args of the user made kernel func in .cl
//		kernel.setArg(0, inBuf);
//		kernel.setArg(1, outBuf);
//
//		cl::CommandQueue queue(context, device);
//
//		// sets up where we to read the finished GPU data
//		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(chunkSize));
//		checkErr(err, "ND Range kernel execution");
//
//		// reads from GPU data from where it was set to based on NDRangeK
//		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * chunkSize, pVec); // get data from device, and work on buffer
//		checkErr(err, "Reading buffer...");
//	}
//
//	return *vec;
//}
//
//std::vector<int> vecToFortran3D(std::vector<int> *vec, cuint halo, cuint x0, cuint x1, cuint y0, cuint y1, cuint z0, cuint z1)
//{
//	// take only what we need from our vector
//	// C is row-major order
//	// convert array to column order
//	cuint arr_size = ((x1 - x0) + halo) * ((y1 - y0) + halo) * ((z1 - z0));
//	std::vector<int> dump(arr_size);
//
//	cuint x_begin = x0 - halo > 0 ? x0 - halo : 0;
//	cuint x_end = x1 + halo < _WIDTH ? x1 + halo : _WIDTH;
//
//	cuint y_begin = y0 - halo > 0 ? y0 - halo : 0;
//	cuint y_end = y1 + halo < _HEIGHT ? y1 + halo : _HEIGHT;
//
//	cuint z_begin = z0 > 0 ? z0 - halo : 0;
//	cuint z_end = z1 < _DEPTH ? z1 : _DEPTH;
//
//	// e.g. 3D array:
//	/*
//	* |1 2 3 4|, |11 12 13 14|
//	* |5 6 7 8|  |15 16 17 18|
//	*/
//
//	// F: '1  11  2  12  3  13  4  14  5  15  6  16  7  17  8  18'
//	// C: '1  5  2  6  3  7  4  8  11  15  12  16  13  17  14  18'
//
//	/*
//	*	// row major order
//		index = ((x * max_y + y) * max_z) + z;
//		// column major order
//		index = ((z * max_y + y) * max_x) + x;
//
//		// for row major order
//		z = index % max_z;
//		y = (index / max_z) % max_y;
//		x = index / (max_z * max_y);
//
//		// for column major order
//		x = index % max_x;
//		y = (index / max_x) % max_y;
//		z = index / (max_x * max_y);
//	*/
//	int i = 0;
//	for (int y = y_begin; y < y_end; y++)
//	{
//		for (int x = x_begin; x < x_end; x++)
//		{
//			for (int z = z_begin; z < z_end; z++)
//			{
//				dump[i] = vec->data()[((z * _HEIGHT + y) * _WIDTH) + x];
//				i++;
//			}
//		}
//	}
//
//	return dump;
//}
//
//std::vector<int> chunkWork3D(cl::Context context, cl::Program program, cl::Device device, std::vector<int> *vec, int halo,
//							 int chunk_x, int chunk_y, int chunk_z, int err = 0)
//{
//	int vecSize = vec->size();
//	int chunkSize = chunk_x * chunk_y * chunk_z;
//
//	// for halo subarrays
//	std::vector<int>::const_iterator starting_index;
//	std::vector<int>::const_iterator ending_index;
//
//	printf("chunksize = %d\n", chunkSize);
//
//	int x = 0;
//	int y = 0;
//	int z = 0;
//
//	int progress = 0;
//	printf("size of array to chunk: %d\n", vec->size());
//	// initial point to start of our vector
//	for (int i = 0; i < vec->size(); i += chunkSize)
//	{
//		// find start and end positions to send
//		int halo_range_left = i - halo;
//		int halo_range_right = (i + chunkSize) + halo;
//
//		std::vector<int> left_attachment;
//		std::vector<int> right_attachment;
//		std::vector<int>::const_iterator starting_index;
//		std::vector<int>::const_iterator ending_index;
//
//		// it needs to get other boundary
//		if (halo_range_left < 0)
//		{
//			left_attachment.insert(left_attachment.begin(), vec->end() + halo_range_left, vec->end());
//			halo_range_left = 0;
//		}
//		if (halo_range_right > vecSize)
//		{
//			right_attachment.insert(right_attachment.begin(), vec->begin(), vec->begin() + (halo_range_right - vecSize));
//			halo_range_right = vecSize;
//		}
//		starting_index = vec->begin() + halo_range_left;
//		ending_index = vec->begin() + halo_range_right;
//		std::vector<int> vectorToEvaluate(starting_index, ending_index);
//
//		// append out of bound vectors if any
//		vectorToEvaluate.insert(vectorToEvaluate.begin(), left_attachment.begin(), left_attachment.end());
//		vectorToEvaluate.insert(vectorToEvaluate.end(), right_attachment.begin(), right_attachment.end());
//
//		int sizeofDataToPass = sizeof(int) * vectorToEvaluate.size();
//
//		// create buffers, and kernel
//		cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeofDataToPass, &vectorToEvaluate.data()[0]);
//		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeofDataToPass, nullptr);
//
//		int x = i % _WIDTH;
//		int temp_index = i / _WIDTH;
//		int y = temp_index % _HEIGHT;
//		int z = temp_index / _HEIGHT;
//
//		int i2 = i + chunkSize;
//
//		int x1 = i2 % _WIDTH;
//		int temp_index2 = i2 / _WIDTH;
//		int y1 = temp_index2 % _HEIGHT;
//		int z1 = temp_index2 / _HEIGHT;
//
//		cl::Kernel kernel(program, "doofus", &err);
//		checkErr(err, "kernelling...");
//		printf("i: %d | (%d, %d, %d) -> (%d, %d, %d)\n", i2, x, y, z, x1, y1, z1);
//		// fill in args of the user made kernel func in .cl
//		kernel.setArg(0, inBuf);
//		kernel.setArg(1, outBuf);
//
//		kernel.setArg(2, i);
//		kernel.setArg(3, chunkSize);
//		kernel.setArg(4, halo);
//
//		cl_event wait;
//		cl_int status;
//
//		cl::CommandQueue queue(context, device);
//
//#pragma warning(disable : 4996)
//		status = clEnqueueMarker(queue(), &wait);
//		if (status != CL_SUCCESS)
//		{
//			printf("Enqueue marker failed...\n");
//		}
//
//		// sets up where we to read the finished GPU data
//		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(sizeofDataToPass)); // global, local range here
//		checkErr(err, "ND Range kernel execution");
//
//		// block until everything is done
//		status = clWaitForEvents(1, &wait);
//		if (status != CL_SUCCESS)
//		{
//			printf("Wait failed?\n");
//		}
//
//		// reads from GPU data from where it was set to based on NDRangeK
//		// get data from device, and work on buffer
//		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeofDataToPass, &vectorToEvaluate.data()[0]);
//		checkErr(err, "Reading buffer...");
//
//		// we only want the non halo part
//		std::copy(vectorToEvaluate.begin() + halo, vectorToEvaluate.begin() + halo + chunkSize, vec->begin() + i);
//
//		
//		progress += chunkSize;
//	}
//
//	return *vec;
//}
//
///*
//* two functions: one for initialisation, and one to run again and again

//	/*
//	* TODO:
//	* 
//	* could make all the platform and device into a struct
//	* so we have a single pointer instead of many
//	*/

//	// force chunksize to fit in memory
//	// if the selected chunk is not a factor of our array
//	// there will not be enough free memory in device.
//	// force chunk size to be cube and remainder

///*
//	* TODO:
//	*
//	* allow user to specify platform and device index, and cl context, if not specified
//	* use default/optional parameters during the initialisation
//	* as long as some way for user to make their choice should they want to
//	* platform info to find id for device, and pass that in for choice in device selection
//	*/
//{

//int main()
//{
	//// create platform to accommodate for devices
	//std::vector<cl::Platform> platforms;
	//cl::Platform::get(&platforms);
	//// select GPU here I think?
	//cl::Platform platform = platforms.back();

	//// get GPU devices
	//std::vector<cl::Device> devices;
	//auto err = platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	//checkErr(err, "getting GPU devices");

	//auto &device = devices.front();
	//printf("%s\n", device.getInfo<CL_DEVICE_NAME>().c_str());

	//// create context from device
	//const cl::Context context(device);

	//// read in kernel file as source
	//std::ifstream kernelFile("ProcArray.cl");
	//std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
	//const cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.size() + 1));

	//// create program from both context, and source
	//cl::Program program(context, sources);
	//err = program.build(devices, "-cl-std=CL1.2");
	//checkErr(err, "building program");

	//// new and improved build errors lmao
	//printf("%s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str());

	//// make 3D array
	//cuint array_width = _WIDTH;
	//cuint array_height = _HEIGHT;
	//cuint array_depth = _DEPTH;
	//cuint array_size = array_width * array_depth * array_height;

	//std::vector<int> vec(array_size);
	//std::fill(vec.begin(), vec.end(), 1);

	//// 3D chunky boii
	//int chunkSize_width = 15;
	//int chunkSize_height = 15;
	//int chunkSize_depth = 15;

	//// force chunksize to fit in memory
	//// if the selected chunk is not a factor of our array
	//// there will not be enough free memory in device.
	//// force chunk size to be cube and remainder
	////int factored_chunk = (_WIDTH * _HEIGHT * _DEPTH) % (chunkSize_width * chunkSize_height * chunkSize_depth);
	////if (factored_chunk != 0)
	////{
	////	chunkSize_width = chunkSize_depth = chunkSize_height = factored_chunk;
	////}

	//// perform chunking function
	//vec = chunkWork3D(context, program, device, &vec, 8, chunkSize_width, chunkSize_height, chunkSize_depth, err);

	//for (auto &i : vec)
	//{
	//	printf("%d", i);
	//}

	//printf("\n");
	//cl::finish();


//}

#include "OCL.h"
// #include <numeric>

int main(int argc, char** argv)
{
	OCL::data_struct dataToUse;

	// init OCL object
	OCL ocl;

	int platform_id=1;
	int device_id=0;

	// init kernel
	ocl.init(platform_id, device_id);

	// execute chunk?
	ocl.run();

	// std::vector<int> vec(dataToUse._WIDTH * dataToUse._HEIGHT * dataToUse._DEPTH);
	// std::iota(std::begin(vec), std::end(vec), 0);

	return 0;
}