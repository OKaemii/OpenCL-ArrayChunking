#include "OCL.h"
#include <numeric>
#include <chrono>


int main(int argc, char** argv)
{
	// init OCL object
	// dimensions of cube
	printf("Enter the dimensions you want to use: \n");
	int dim = 0;
	std::cin >> dim;
	printf("Please select halo size: ");
	int halo = 1;
	std::cin >> halo;

	OCL ocl(dim, dim, dim, halo);

	//printf("Please Select your Platform: ");
	int platform_id=1;
	//std::cin >> platform_id;
	//printf("Please Select your Device ID: ");
	int device_id = 0;
	//std::cin >> device_id;
	printf("Please select starting chunk size: ");
	int j = 1;
	std::cin >> j;
	// init kernel
	ocl.init(platform_id, device_id);

	// if j is not a factor of our dim, it will crash :(
	printf("using %d chunks | chunk dim: (%d, %d, %d)\n", (j * j * j), dim / j + halo, dim / j + halo, dim / j + halo);
	for (int i = 0; i < 5; i++/*, j *= 2*/)
	{
		if ((dim + halo) % j != 0)
		{
			printf("chunko no %d. :(\n", j);
			continue;
		}
		auto startTime = std::chrono::high_resolution_clock::now();

		// consider in opencl vhost pointer or copy host pointer

		// execute chunk?
		ocl.run(j, j, j);
		ocl.run(j, j, j);
		ocl.run(j, j, j);
		ocl.run(j, j, j);
		ocl.run(j, j, j);
		ocl.run(j, j, j);

		auto endTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		std::cout << elapsedTime << ", ";
	}

	return 0;
}