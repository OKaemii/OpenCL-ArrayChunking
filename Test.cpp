#include "OCL.h"
#include <numeric>
#include <chrono>


int main(int argc, char** argv)
{
	// init OCL object
	// dimensions of cube
		int platform_id = 0;
		int device_id = 0;

	if (argc < 2)
	{
		OCL ocl(0, 0, 0, 0);
		printf("Please Select your Platform: ");
		std::cin >> platform_id;
		printf("Please Select your Device ID: ");
		std::cin >> device_id;
	}
	else
	{
		platform_id = atoi(argv[1]);
		device_id = atoi(argv[2]);
	}


	std::vector<int> dimensions = {16, 32, 64, 512};
	std::vector<int> haloes = {1, 2};
	std::vector<int> chunkSizes = {1, 2, 4};

	for (auto dim : dimensions)
	{
		for (auto halo : haloes)
		{
			for (auto chunk: chunkSizes)
			{
				if ((dim == 512 || dim == 511 || dim == 510) && (chunk == 2 || chunk == 4))
				{
					continue;
				}

				std::vector<long long> fullRuns;

				int j = chunk;
				dim -= halo;
				OCL ocl(dim, dim, dim, halo, false);

				ocl.init(platform_id, device_id, false);
				printf("using %d chunks | chunk dim: (%d, %d, %d) | halo: %d \n", (j * j * j), (dim / j) + halo, (dim / j) + halo, (dim / j) + halo, halo);
				
				// if j is not a factor of our dim, it will crash :(
				//if ((dim + halo) % j != 0)
				//{
				//	printf("chunko no %d. :(\n", j);
				//	continue;
				//}

				// output test array
				for (int i = 0; i < 5; i++)
				{
					std::vector<long long> gpuRuns;

					auto startTime = std::chrono::high_resolution_clock::now();

					// consider in opencl vhost pointer or copy host pointer

					// execute chunk?
					for (int t = 0; t < 6; t++)
					{
						ocl.run(j, j, j);
						gpuRuns.push_back(ocl.dataToUse._GPUTIME);
					}

					printf("\ngpu time: [");
					for (auto i : gpuRuns)
					{
						std::cout << i << ", ";
					}
					printf("]\n");

					auto endTime = std::chrono::high_resolution_clock::now();
					auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
					fullRuns.push_back(elapsedTime);

				}

				printf("\nfull time: [");
				for (auto i : fullRuns)
				{
					std::cout << i << ", ";
				}
				printf("]\n");

			}
			
		}
	}

	

	return 0;
}