bool doesIntersect(float x0, float y0, float z0, float w0, float h0, float d0, float x1, float y1, float z1, float w1, float h1, float d1);

/* Assume an array of floats, p(0:_WIDTH + halo, 0:_HEIGHT + halo, 0:_DEPTH + halo)
 So there is a variable-point halo in every direction

 x,y,z are calculated from global_id using the range and offset passed from host
 */


 /*
 * TODO:: based on location of chunk, if these calculations need to be computed
 * sideflows will always need to be done, and need values from farside of domain
 *
 * also should consider implementing reduction to improve performance (future work)
 */

 /*
 * note: general case chunking does not work with periodic boundary cases with current implementation
 * further work is to add further buffer for other side.
 *
 * All "init_..." functions should be done on the CPU or with 0 chunking, where the entire array is assessible.
 * Alternative solution is to have a new kernel to run init(), and the run() runs all non init functions.
 */

 /* First populate the inflow plane */
 /* Iteration (Fortran convention)
 x: 0, halo
 y: halo, _HEIGHT
 z: halo, _DEPTH

 global range: (halo)*_HEIGHT*_DEPTH
 */
//void init_inflow_plane(__global float* outData, int z, int _DEPTH, int halo)
//{
//	// data[F3D2C(_WIDTH + (halo * 2), _HEIGHT + (halo * 2), 0, 0, 0, x, y, z)] = (z - halo) / _DEPTH;
//	// data[(l, h, d, 0, 0, x, y, z)] = (z - halo) / _DEPTH;
//	outData[get_global_id(0)] = (z - halo) / _DEPTH;
//}

/* Calculate the outflow values */
/* Iteration (Fortran convention)
x: 1+1no iteration, data(_WIDTH + halo, y, z) = data(_WIDTH, y, z) // pressure at (_WIDTH + halo) same as _WIDTH passed from host
y: 1,_HEIGHT
z: 1,__DEPTH

global range: _HEIGHT*__DEPTH
*/
//void init_outflow_halo(float* data, int index)
//{
//	// data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, _WIDTH + 1, y, z)] = p[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, _WIDTH, y, z)];
//	// int index = (x+1) + max_x * (y + max_y * z);
//
//	data[index] = data[get_global_id(0)];
//}

/* Calculate the periodic conditions */
/* Iteration (Fortran convention)
x: 1, _WIDTH
y: no iteration.
z: 1, __DEPTH

data(x, 0, z) = data(x, _HEIGHT, z)
data(x, _HEIGHT + 1, z) = data(x, 1, z)

global range: _WIDTH * _DEPTH
*/
//void init_sideflow_halos(float* data, int index0, int index1, int index2, int index3)
//{
//	// data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, 0, z)] = data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, _HEIGHT, z)];
//	// data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, _HEIGHT + 1, z)] = data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, 1, z)];
//
//	// int index0 = (x) + max_x * ((0) + max_y * (z));
//	// int index1 = (x) + max_x * ((_HEIGHT) + max_y * (z));
//	// int index2 = (x) + max_x * ((_HEIGHT+1) + max_y * (z));
//	// int index3 = (x) + max_x * ((1) + max_y * (z));
//
//	data[index0] = data[index1];
//	data[index2] = data[index3];
//}


/* Calculate the top and bottom conditions */
/* Iteration (Fortran convention)
x: 1, _WIDTH
y: 1, _HEIGHT
z: no iteration

data(x, y, 0) = data(x, y, 1)
data(x, y, _DEPTH + 1) = data(x, y, z)

global range: _HEIGHT * _DEPTH
*/

//void init_top_bottom_halos(float* data, int index0, int index1, int index2, int index3)
//{
//	// data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, y, 0)] = data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, y, 1)];
//	// data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, y, _DEPTH + 1)] = data[F3D2C(_WIDTH + 2, _HEIGHT + 2, 0, 0, 0, x, y, _DEPTH)];
//
//	// int index0 = (x) + max_x * ((y) + max_y * (0));
//	// int index1 = (x) + max_x * ((y) + max_y * (1));
//	// int index2 = (x) + max_x * ((y) + max_y * (_DEPTH + 1));
//	// int index3 = (x) + max_x * ((y) + max_y * (_DEPTH));
//
//	data[index0] = data[index1];
//	data[index2] = data[index3];
//}

/* Then populate the entire domain with the same values */
/* Iteration (Fortran convention)
x: 1,_WIDTH
y: 1,_HEIGHT
z: 1,__DEPTH

global range is _WIDTH*_HEIGHT*__DEPTH
*/
//void init_domain(__global float* data, __global float* out_data)
//{
//	// data[F3D2C(_WIDTH + (halo * 2), _HEIGHT + (halo * 2), 0, 0, 0, x, y, z)] = 0.0;
//	out_data[get_global_id(0)] = 0.0;
//}

/* Calculate the new core values */
/* Iteration (Fortran convention)
x: 1, _WIDTH
y: 1, _HEIGHT
z: 1, _DEPTH

global range: _WIDTH * _HEIGHT * _DEPTH

// need a macro to get the correct
// created a new array, and new values and return that back to host
// partial derivative of 3 dimensions

operation: p_new(x,y,z) = (p(x+1,y,z) + p(x-1,y,z) + p(x,y+1,z) + p(x,y-1,z) + p(x,y,z+1) + p(x,y,z-1))/6

*/
void calc_new_core_values(__global float* data, __global float* out_data, int x, int y, int z, int max_x, int max_y)
{
	int index0 = (x + 1) + max_x * (y + max_y * z);
	int index1 = (x - 1) + max_x * (y + max_y * z);
	int index2 = x + max_x * ((y + 1) + max_y * z);
	int index3 = x + max_x * ((y - 1) + max_y * z);
	int index4 = x + max_x * (y + max_y * (z + 1));
	int index5 = x + max_x * (y + max_y * (z - 1));

	out_data[get_global_id(0)] = (data[index0] + data[index1] + data[index2] + data[index3] + data[index4] + data[index5]) / 6;
}

__kernel void super(__global float* data, __global float* out_data, int max_x, int max_y, int x_offset, int y_offset, int z_offset, int _WIDTH, int _DEPTH, int _HEIGHT, int halo)
{
	// co-ordinates of current index
	// map global array index to local array id
	int id = get_global_id(0);

	// find index of co-ordinates (x0, y0, z0)
	int loc_x = (id % max_x) + x_offset;
	int loc_index = id / max_x;
	int loc_y = (loc_index % max_y) + y_offset;
	int loc_z = (loc_index / max_y) + z_offset;
	
	// printf("max_x: %d, max_y: %d, x_offset: %d, y_offset: %d, z_offset: %d\n",max_x, max_y, x_offset, y_offset, z_offset);
	// (-------------------------BOUNDARY-------------------------)(--------------------------OBJECT--------------------------) 
	// (float x0, float y0, float z0, float w0, float h0, float d0, float x1, float y1, float z1, float w1, float h1, float d1)

	if ((loc_x >= halo && loc_y >= halo && loc_z >= halo) && (loc_x <= (_WIDTH - halo*2) && loc_y >= (_HEIGHT - halo * 2) && loc_z >= (_DEPTH - halo * 2)))
	{
		calc_new_core_values(data, out_data, loc_x - x_offset, loc_y - y_offset, loc_z - z_offset, max_x, max_y);
	}
}