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
	if ((dim + halo) % j != 0)
	{
		printf("chunko no %d. :(\n", j);
		return 0;
	}
	auto startTime = std::chrono::high_resolution_clock::now();

	// consider in opencl vhost pointer or copy host pointer

	// execute chunk?
	ocl.run(j, j, j);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "chunk_size(" << j << ")| elapsed time: " << elapsedTime << " (ms)" << std::endl;

	//// run chunking algo 3 times with different chunk sizes (j)
	//for (int i = 0; i < 3; i++, j*=2)
	//{
	//	// if j is not a factor of our dim, it will crash :(
	//	if ((dim+halo) % j != 0)
	//	{
	//		printf("chunko no %d. :(\n", j);
	//		continue;
	//	}
	//	auto startTime = std::chrono::high_resolution_clock::now();

	//	// consider in opencl vhost pointer or copy host pointer

	//	// execute chunk?
	//	ocl.run(j, j, j, halo);

	//	auto endTime = std::chrono::high_resolution_clock::now();
	//	auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	//	std::cout << "chunk_size(" << j << ")| elapsed time: " << elapsedTime << " (ms)" << std::endl;
	//}

	return 0;
}