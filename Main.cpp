#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include "Main.h"

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>


int ocl_init()
{
	final int platformIndex = 0;
	final long deviceType = CL_DEVICE_TYPE_ALL;

}