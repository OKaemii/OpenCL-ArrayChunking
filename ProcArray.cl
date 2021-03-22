void calcTop(__global float* data, __global float* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 3;
}

void calcMid(__global float* data, __global float* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 1;
}

void calcBot(__global float* data, __global float* outData)
{
	outData[get_global_id(0)] = data[get_global_id(0)] * 7;
}

bool doesIntersect(float x0, float y0, float z0, float w0, float h0, float d0, float x1, float y1, float z1, float w1, float h1, float d1)
{
	/*
	* w,h,z: dimensions of object
	* x,y,z: co-ordinates of object
	*/

	// Calculate Halves
	float hw0 = w0 * 0.5;
	float hh0 = h0 * 0.5;
	float hd0 = d0 * 0.5;

	float hw1 = w1 * 0.5;
	float hh1 = h1 * 0.5;
	float hd1 = d1 * 0.5;

	// Calculate Middle of Boundary
	float mx = x0 + hw0;
	float my = y0 + hh0;
	float mz = z0 + hd0;

	// Calculate Middle of Block
	float bx = x1 + hw1;
	float by = y1 + hh1;
	float bz = z1 + hd1;

	// Intersection test
	// Calculate Deltas (centre displacement)
	float dx = fabs(mx - bx);
	float dy = fabs(my - by);
	float dz = fabs(mz - bz);

	return (hw0 + hw1) > dx && (hh0 + hh1) > dy && (hd0 + hd1) > dz;
}

// super kernel
//
// takes in boundaries of selected chunk, and points to the correct calculation
//
__kernel void doofus(__global float* data, __global float* outData, int max_x, int max_y, int x_offset, int y_offset, int z_offset, int _WIDTH, int _DEPTH, int _HEIGHT)
{
	// boundary conditions
	// top boundary
	float top_aMinX = 0.f;
	float top_aMinY = 0.f;
	float top_aMinZ = 0.f;

	float top_aMaxX = (float) _WIDTH;
	float top_aMaxY = 0.333f * (float)_HEIGHT;
	float top_aMaxZ = (float) _DEPTH;

	// middle boundary
	float mid_aMinX = 0.f;
	float mid_aMinY = top_aMaxY;
	float mid_aMinZ = 0.f;

	float mid_aMaxX = (float) _WIDTH;
	float mid_aMaxY = 0.666f * (float)_HEIGHT;
	float mid_aMaxZ = (float) _DEPTH;

	// bottom boundary
	float bot_aMinX = 0.f;
	float bot_aMinY = mid_aMaxY;
	float bot_aMinZ = 0.f;

	float bot_aMaxX = (float)_WIDTH;
	float bot_aMaxY = (float)_HEIGHT;
	float bot_aMaxZ = (float)_DEPTH;

	/*
	* TODO:
	* identify which function needs to be called, case statement
	* for each of those calls, work out if they need to be computed; work out which boundary it is to see if there is work to be done
	* for any given state where you'd normally call a function, check if you need to call it
	* for a function on the boundary, need to test if chunk is on that boundary or no operation
	* for a function that calls regardless of boundary, need no check
	*/

	// co-ordinates of current index
	// map global array index to local array id
	int id = get_global_id(0);

	// find index of co-ordinates (x0, y0, z0)
	float loc_x = (id % max_x) + x_offset;
	int loc_index = id / max_x;
	float loc_y = (loc_index % max_y) + y_offset;
	float loc_z = (loc_index / max_y) + z_offset;

	
	if (doesIntersect(top_aMinX, top_aMinY, top_aMinZ, top_aMaxX, top_aMaxY, top_aMaxZ, loc_x, loc_y, loc_z, 0.9f, 0.9f, 0.9f))
	{
		//printf("top coordinates: (%f, %f, %f) @%d\n", loc_x, loc_y, loc_z, id);
		calcTop(data, outData);
	}
	if (doesIntersect(mid_aMinX, mid_aMinY, mid_aMinZ, mid_aMaxX, mid_aMaxY, mid_aMaxZ, loc_x, loc_y, loc_z, 0.9f, 0.9f, 0.9f))
	{
		//printf("mid coordinates: (%f, %f, %f) @%d\n", loc_x, loc_y, loc_z, id);
		calcMid(data, outData);
	}
	if (doesIntersect(bot_aMinX, bot_aMinY, bot_aMinZ, bot_aMaxX, bot_aMaxY, bot_aMaxZ, loc_x, loc_y, loc_z, 0.9f, 0.9f, 0.9f))
	{
		//printf("bot coordinates: (%f, %f, %f) @%d\n", loc_x, loc_y, loc_z, id);
		calcBot(data, outData);
	}

	// something went wrong error
	//outData[id] = 9;
}