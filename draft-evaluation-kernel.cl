/* Assume an array of floats, p(0:ip+1,0:jp+1,0:kp+1) 
 So there is a 1-point halo in every direction
 
 i,j,k are calculated from global_id based on the range and offset in each direction using calc_loop_iters()

 */


/*
* based on location of chunk, if these calculations need to be computed is your task
* sideflows will always need to be done, and need values from farside of domain
* 
* also should consider implementing reduction to improve performance (later)
*/

/* First populate the inflow plane */
/* Iteration (Fortran convention)
i: 0,1
j: 1, jp
k: 1, kp
global range: 2*jp*kp
*/
void init_inflow_plane(float* p) {
    // ... - need to get i,j,k from global id to make this work
      p[F3D2C(ip+2, jp+2, 0,0,0, i,j,k)] = (k-1)/kp;
}

/* Then populate the entire domain with the same values */
/* Iteration (Fortran convention)
i: 1,ip
j: 1,jp
k: 1,kp
global range is ip*jp*kp
*/
void init_domain(float* p) {
    // ... - need to get i,j,k from global id to make this work

    p[F3D2C(ip+2, jp+2, 0,0,0, i,j,k)]  = 0.0;

}

/* Calculate the outflow values */
/* Iteration (Fortran convention)
i: no iteration, p(ip+1,j,k)=p(ip,j,k) // pressure at ip+1 same as ip
j: 1,jp
k: 1,kp
global range: jp*kp
*/
void init_outflow_halo(float* p) {
    // ...

p[F3D2C(ip+2, jp+2, 0,0,0, ip+1,j,k)] = p[F3D2C(ip+2, jp+2, 0,0,0, ip,j,k)];

}

/* Calculate the periodic conditions */
/* Iteration (Fortran convention)
i: 1,ip
j: no iteration. 
p(i,0,k) = p(i,jp,k)
p(i,jp+1,k) = p(i,1,k)
k: 1,kp
global range: ip*kp
*/
void init_sideflow_halos(float* p) {
    // ...

p[F3D2C(ip+2, jp+2, 0,0,0, i,0,k)] = p[F3D2C(ip+2, jp+2, 0,0,0, i,jp,k)];
p[F3D2C(ip+2, jp+2, 0,0,0, i,jp+1,k)] = p[F3D2C(ip+2, jp+2, 0,0,0, i,1,k)];


}

/*
* note: general case chunking does not work with periodic boundary cases with current implementation
* further work is to add further buffer for other side
* 
* currently only chunk from 1 dimension
*/


/* Calculate the top and bottom conditions */
/* Iteration (Fortran convention)
i: 1,ip
j: 1,jp
k: no iteration
p(i,j,0)=p(i,j,1)
p(i,j,kp+1)=p(i,j,k)
global range: jp*kp
*/

// F3D2C the macro

void init_top_bottom_halos(float* p) {
    // ...
p[F3D2C(ip+2, jp+2, 0,0,0, i,j,0)] = p[F3D2C(ip+2, jp+2, 0,0,0, i,j,1)];
p[F3D2C(ip+2, jp+2, 0,0,0, i,j,kp+1)] = p[F3D2C(ip+2, jp+2, 0,0,0, i,j,kp)];
}


/* Calculate the new core values */
/* Iteration (Fortran convention)
i: 1,ip
j: 1,jp
k: 1,kp
global range: ip*jp*kp

// need a macro to get the correct
// created a new array, and new values and return that back to host
// partial derivative of 3 dimensions
operation: p_new(i,j,k) = (
    p(i+1,j,k) + 
    p(i-1,j,k) +
    p(i,j+1,k) +
    p(i,j-1,k) +
    p(i,j,k+1) + 
    p(i,j,k-1)
    )/6

*/
void calc_new_core_values(float* p, float* p_out) {
// ...
}


// Superkernel code here



// the real world sim has a lot more constraints than the eval kernel, but not problem
// once machine to chunk, programmer can decide if done on a perfunction basis or a mixture or both
// depends on the programmer, not my problem, I am showing than chunking works
// if I do it on this basis, this would be faster, or if this slower
// if I do whole chunk at once is faster
// could bandwidth test - the time it takes to send a chunk; receive a chunk (depends on size of chunk)
// could test it takes a chunk to perform a function

// my work is to show that I can do this chunking, and ability to recognise where it is in the total grid of chunk
// and then decide to do the boundary condition or not