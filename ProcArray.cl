__kernel void calcTop(__global int* data, __global int* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 3;
}

__kernel void calcMid(__global int* data, __global int* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 1;
}

__kernel void calcBot(__global int* data, __global int* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 7;
}

__kernel void calcNon(__global int* data, __global int* outData)
{
	outData[get_global_id(0)] = 0;
}