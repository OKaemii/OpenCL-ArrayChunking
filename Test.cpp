#include "OCL.h"
#include <numeric>
#include <chrono>


int main(int argc, char** argv)
{
	OCL::data_struct dataToUse;

	// init OCL object
	// dimensions of cube
	printf("Enter the dimensions you want to use: \n");
	int dim = 0;
	std::cin >> dim;

	OCL ocl(dim, dim, dim);

	printf("Please Select your Platform: ");
	int platform_id=0;
	std::cin >> platform_id;
	printf("Please Select your Device ID: ");
	int device_id = 0;
	std::cin >> device_id;
	printf("Please select starting chunk size: ");
	int j = 0;
	std::cin >> j;
	// init kernel
	ocl.init(platform_id, device_id);

	// run chunking algo 3 times with different chunk sizes (j)
	for (int i = 0; i < 3; i++, j*=2)
	{
		// if j is not a factor of our dim, it will crash :(
		if (dim % j != 0)
		{
			printf("chunko no %d. :(\n", j);
			continue;
		}
		auto startTime = std::chrono::high_resolution_clock::now();

		// execute chunk?
		ocl.run(j, j, j, 0);

		auto endTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

		std::cout << "chunk_size(" << j << ")| elapsed time: " << elapsedTime << " (ms)" << std::endl;
	}

	return 0;
}

// 1333, 