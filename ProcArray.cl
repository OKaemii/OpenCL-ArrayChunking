#define _WIDTH	30
#define _HEIGHT	30
#define _DEPTH	30



void calcTop(__global int* data, __global int* outData)
{
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

bool doesIntersect(int x0, int y0, int z0, int w0, int h0, int d0, int x1, int y1, int z1, int w1, int h1, int d1)
{
	/*
	* w,h,z: dimensions of object
	* x,y,z: co-ordinates of object
	*/

	// Calculate Halves
	int hw0 = w0 >> 1;
	int hh0 = h0 >> 1;
	int hd0 = d0 >> 1;

	int hw1 = w1 >> 1;
	int hh1 = h1 >> 1;
	int hd1 = d1 >> 1;

	// Calculate Middle of Boundary
	int mx = x0 + hw0;
	int my = y0 + hh0;
	int mz = z0 + hd0;

	// Calculate Middle of Block
	int bx = x1 + hw1;
	int by = y1 + hh1;
	int bz = z1 + hd1;

	// Intersection test
	// Calculate Deltas (centre displacement)
	int dx = abs(mx - bx);
	int dy = abs(my - by);
	int dz = abs(mz - bz);

	return (hw0 + hw1) > dx && (hh0 + hh1) > dy && (hd0 + hd1) > dz;
}

// super kernel
//
// takes in boundaries of selected chunk, and points to the correct calculation
//
__kernel void doofus(__global int* data, __global int* outData, int x0, int y0, int z0, int w0, int h0, int d0)
{
	// boundary conditions
	// top boundary
	int top_aMinX = 0;
	int top_aMinY = 0;
	int top_aMinZ = 0;

	int top_aMaxX = _WIDTH;
	int top_aMaxY = (_HEIGHT * 1) / 3;
	int top_aMaxZ = _DEPTH;

	// middle boundary
	int mid_aMinX = 0;
	int mid_aMinY = (_HEIGHT * 1) / 3;
	int mid_aMinZ = 0;

	int mid_aMaxX = _WIDTH;
	int mid_aMaxY = (_HEIGHT * 2) / 3;
	int mid_aMaxZ = _DEPTH;

	// bottom boundary
	int bot_aMinX = 0;
	int bot_aMinY = (_HEIGHT * 2) / 3;
	int bot_aMinZ = 0;

	int bot_aMaxX = _WIDTH;
	int bot_aMaxY = _HEIGHT;
	int bot_aMaxZ = _DEPTH;

	/*
	* TODO:
	* identify which function needs to be called, case statement
	* for each of those calls, work out if they need to be computed; work out which boundary it is to see if there is work to be done
	* for any given state where you'd normally call a function, check if you need to call it
	* for a function on the boundary, need to test if chunk is on that boundary or no operation
	* for a function that calls regardless of boundary, need no check
	*/
	// co-ordinates of current index
	int id = get_global_id(0);

	int max_x = _WIDTH;  //x0 + w0;
	int max_y = _HEIGHT; //y0 + h0;
	int max_z = _DEPTH;  //z0 + d0;

	// find index of co-ordinates (x0, y0, z0)
	// dimensions w0, h0, d0
	int index = x0 + w0 * (y0 + h0 * z0) + id;

	int loc_x = index % _WIDTH;
	int temp_index = index / _WIDTH;
	int loc_y = temp_index % _HEIGHT;
	int loc_z = temp_index / _HEIGHT;
	
	// printf("@id: %d <-> %d | should start (%d, %d, %d)| real start (%d, %d, %d) -> (%d, %d, %d) | current (%d, %d, %d)\n", id, index, x,y,z,x0,y0,z0, x0 + w0, y0 + h0, z0 + d0, loc_x, loc_y, loc_z);
	// printf("@%d | D(%d, %d, %d)->(%d, %d, %d) @(%d, %d, %d)\n", id, x0, y0, z0, x0+w0, y0+h0, z0+d0, loc_x, loc_y, loc_z);
	if (doesIntersect(top_aMinX, top_aMinY, top_aMinZ, top_aMaxX, top_aMaxY, top_aMaxZ, loc_x, loc_y, loc_z, 2, 2, 2))
	{
		calcTop(data, outData);
	}
	if (doesIntersect(mid_aMinX, mid_aMinY, mid_aMinZ, mid_aMaxX, mid_aMaxY, mid_aMaxZ, loc_x, loc_y, loc_z, 2, 2, 2))
	{
		calcMid(data, outData);
	}
	if (doesIntersect(bot_aMinX, bot_aMinY, bot_aMinZ, bot_aMaxX, bot_aMaxY, bot_aMaxZ, loc_x, loc_y, loc_z, 2, 2, 2))
	{
		calcBot(data, outData);
	}
}