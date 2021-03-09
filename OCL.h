#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>

class OCL
{
public:
	/*
	* default constructor for OpenCL initialisation 
	* platform, and device are automatically chosen
	*/
	OCL();

	/*
	* default constructor's destructor
	* frees OpenCL objects, and frees device memory
	*/
	~OCL();

	/*
	* initialise data for kernel
	*/
	void init(int platform_id = 0, int device_id = 0);

	/*
	* the bread and butter
	*/
	void run();

	// function to return our error info
	bool checkErr(cl_int err, const char* name);

	// entire array body to work with
	std::vector<int> arrMainBody;

	struct data_struct
	{
		// make 3D array
		int _WIDTH = 30;
		int _HEIGHT = 30;
		int _DEPTH = 30;

		// 3D chunky boii
		int chunk_x = 5;
		int chunk_y = 5;
		int chunk_z = 5;
	};

private:
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

	// list of available platforms
	std::vector<cl::Platform> platforms;
	cl::Platform platform; // platform to use

	// list of available devices
	std::vector<cl::Device> devices;
	cl::Device device; // device object to use

	// for our wait event
	cl::Event event;

	// for error log
	cl_int err;
};

