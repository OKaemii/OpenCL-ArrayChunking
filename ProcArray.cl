__kernel void calcTop(__global int* data, __global int* outData);
__kernel void calcMid(__global int* data, __global int* outData);
__kernel void calcBot(__global int* data, __global int* outData);
bool doesIntersect(int aMinX, int aMinY, int aMinZ, int aMaxX, int aMaxY, int aMaxZ, int bMinX, int bMinY, int bMinZ, int bMaxX, int bMaxY, int bMaxZ);

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

bool doesIntersect(int aMinX, int aMinY, int aMinZ, int aMaxX, int aMaxY, int aMaxZ, int bMinX, int bMinY, int bMinZ, int bMaxX, int bMaxY, int bMaxZ)
{
	return (aMinX <= bMaxX && aMaxX >= bMinX) && (aMinY <= bMaxY && aMaxY >= bMinY) && (aMinZ <= bMaxZ && aMaxZ >= bMinZ);
}

// super kernel
//
// takes in boundaries of selected chunk, and points to the correct calculation
//
__kernel void doofus(__global int* data, __global int* outData, int x0, int y0, int z0, int w0, int h0, int d0)
{
	int _WIDTH = 30;
	int _HEIGHT = 30;
	int _DEPTH = 30;

	int x = 0;
	int z = 0;

	// top boundary?
	if (doesIntersect(x, _HEIGHT, z, _WIDTH, _HEIGHT * 2 / 3, _DEPTH, x0, y0, z0, w0, h0, d0))
	{
		calcTop(data, outData);
	}

	// mid boundary?
	if (doesIntersect(x, _HEIGHT * 2 / 3, z, _WIDTH, _HEIGHT * 1 / 3, _DEPTH, x0, y0, z0, w0, h0, d0))
	{
		calcMid(data, outData);
	}

	// bot boundary?
	if (doesIntersect(x, _HEIGHT * 1 / 3, z, _WIDTH, 0, _DEPTH, x0, y0, z0, w0, h0, d0))
	{
		calcBot(data, outData);
	}
}