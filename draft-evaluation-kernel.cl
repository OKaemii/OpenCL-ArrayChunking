/* Assume an array of floats, p(0:ip+1,0:jp+1,0:kp+1) 
 So there is a 1-point halo in every direction
 
 i,j,k are calculated from global_id based on the range and offset in each direction using calc_loop_iters()

 */

/* First populate the inflow plane */
/* Iteration (Fortran convention)
i: 0,1
j: 1, jp
k: 1, kp
global range: 2*jp*kp
*/
void init_inflow_plane(float* p) {
    // ...
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
    // ...

    p[F3D2C(ip+2, jp+2, 0,0,0, i,j,k)]  = 0.0;

}

/* Calculate the outflow values */
/* Iteration (Fortran convention)
i: no iteration, p(ip+1,j,k)=p(ip,j,k)
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

/* Calculate the top and bottom conditions */
/* Iteration (Fortran convention)
i: 1,ip
j: 1,jp
k: no iteration
p(i,j,0)=p(i,j,1)
p(i,j,kp+1)=p(i,j,k)
global range: jp*kp
*/
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


