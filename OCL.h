#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>

class OCL
{
public:
	// entire array body to work with
	std::vector<int> arrMainBody;

	int _WIDTH = 30;
	int _HEIGHT = 30;
	int _DEPTH = 30;

	int chunk_x = 15;
	int chunk_y = 15;
	int chunk_z = 15;

	// buffer for our main body
	cl::Buffer cl_arrMainBody_buffer;

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
	void init();

	/*
	* the bread and butter
	*/
	void run();

	// function to return our error info
	void checkErr(cl_int err, const char* name);

private:

	// list of available platforms
	std::vector<cl::Platform> platforms;
	cl::Platform platform; // platform to use
	int platform_id; // platform to use

	// list of available devices
	std::vector<cl::Device> devices;
	cl::Device device; // device object
	int device_id; // device to use

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

