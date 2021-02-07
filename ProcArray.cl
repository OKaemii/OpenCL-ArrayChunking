void calcTop(__global int* data, __global int* outData);
void calcMid(__global int* data, __global int* outData);
void calcBot(__global int* data, __global int* outData);
bool doesIntersect(int aMinX, int aMinY, int aMinZ, int aMaxX, int aMaxY, int aMaxZ, int bMinX, int bMinY, int bMinZ, int bMaxX, int bMaxY, int bMaxZ);

void calcTop(__global int* data, __global int* outData)
{
	// removing this empty printf breaks the code
	printf(""); // the most important piece to make this work
	outData[get_global_id(0)] = data[get_global_id(0)] * 3;
}

void calcMid(__global int* data, __global int* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 0;
}

void calcBot(__global int* data, __global int* outData)
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

	// boundary conditions
	// top boundary
	int top_aMinX = 0;
	int top_aMinY = _HEIGHT;
	int top_aMinZ = 0;

	int top_aMaxX = _WIDTH;
	int top_aMaxY = _HEIGHT * 2 / 3;
	int top_aMaxZ = _DEPTH;

	// middle boundary
	int mid_aMinX = 0;
	int mid_aMinY = _HEIGHT * 2 / 3;
	int mid_aMinZ = 0;

	int mid_aMaxX = _WIDTH;
	int mid_aMaxY = _HEIGHT * 1 / 3;
	int mid_aMaxZ = _DEPTH;

	// bottom boundary
	int bot_aMinX = 0;
	int bot_aMinY = _HEIGHT * 1 / 3;
	int bot_aMinZ = 0;

	int bot_aMaxX = _WIDTH;
	int bot_aMaxY = 0;
	int bot_aMaxZ = _DEPTH;

	if (doesIntersect(top_aMinX, top_aMinY, top_aMinZ, top_aMaxX, top_aMaxY, top_aMaxZ, x0, y0, z0, w0, h0, d0))
	{
		calcTop(data, outData);
	}
	else if (doesIntersect(mid_aMinX, mid_aMinY, mid_aMinZ, mid_aMaxX, mid_aMaxY, mid_aMaxZ, x0, y0, z0, w0, h0, d0))
	{
		calcMid(data, outData);
	}
	else if (doesIntersect(bot_aMinX, bot_aMinY, bot_aMinZ, bot_aMaxX, bot_aMaxY, bot_aMaxZ, x0, y0, z0, w0, h0, d0))
	{
		calcBot(data, outData);
	}
}