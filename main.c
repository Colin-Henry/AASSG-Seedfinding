#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "cubiomes.h"
#include "fileProcessing.h"
#include "netherFilters.h"
#include "endFilters.h"

int main(int argc, char *argv[])
{
    double totalTime; // Variable to store total time

    MPI_Init(&argc, &argv); // Initialize MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    double startTime = MPI_Wtime(); // Start time of the computation

    fileOpener(&fileManagement);
    readSeedRange(&fileManagement, &startingStructureSeed, &endingStructureSeed);

    if (rank != 0) // Dedicating rank 0 as the "file writing process"
    {
        uint64_t totalIterations = endingStructureSeed - startingStructureSeed + 1;
        uint64_t iterationsPerProcess = totalIterations / (size - 1);
        uint64_t extraIterations = totalIterations % (size - 1);

        uint64_t startIteration = (rank - 1) * iterationsPerProcess + startingStructureSeed;
        uint64_t endIteration = rank * iterationsPerProcess - 1 + startingStructureSeed;
        if (rank == size - 1) 
        {
            endIteration += extraIterations; // Add extra seeds to the last process
        }

        Pos bastions[4], forts[4];
        int bastCount = 0;
        int fortCount = 0;
        int bastID = 0; 
        int fortID = 0;
        Pos3 gatewayCoords = {0, 0, 0};
        Pos endCityCoords = {0, 0};

        Generator biomeSource;
        setupGenerator(&biomeSource, MC_1_16_1, 0);
    
        threadFileOpener(&threadFileManagement, rank);

        for (currentStructureSeed = startIteration; currentStructureSeed <= endIteration; currentStructureSeed++) 
        {
            bool isFastion = findFastions(currentStructureSeed, bastions, bastCount, forts, fortCount, &biomeSource, bastID, fortID);

            if (isFastion)
            {
                fprintf(threadFileManagement.fastionSeeds, "%" PRId64 "\n", currentStructureSeed);

                bool isSSV = checkForSSV(&forts[fortID], &biomeSource);
                if (isSSV)
                    fprintf(threadFileManagement.ssvFastionSeeds, "%" PRId64 "\n", currentStructureSeed);
                
                if (!isEndCityNearby(currentStructureSeed))
                    continue;
                
                bool isEndCity = findEndCities(currentStructureSeed, &endCityCoords, &gatewayCoords);
                if (isEndCity)
                {
                    fprintf(threadFileManagement.fastionEndCitySeeds, "%" PRId64 "\n", currentStructureSeed);
                    fprintf(threadFileManagement.fastionEndCitySeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);

                    if (isSSV)
                    {
                        fprintf(threadFileManagement.ssvFastionEndCitySeeds, "%" PRId64 "\n", currentStructureSeed);
                        fprintf(threadFileManagement.ssvFastionEndCitySeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);
                    }
                    
                    bool isEndCityShip = checkForShip(currentStructureSeed, &endCityCoords);
                    if (isEndCityShip)
                    {
                        fprintf(threadFileManagement.fastionEndCityShipSeeds, "%" PRId64 "\n", currentStructureSeed);
                        fprintf(threadFileManagement.fastionEndCityShipSeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);

                        if (isSSV)
                        {
                            fprintf(threadFileManagement.ssvFastionEndCityShipSeeds, "%" PRId64 "\n", currentStructureSeed);
                            fprintf(threadFileManagement.ssvFastionEndCityShipSeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);
                        }
                    }
                }
            }
        }

        threadFileCloser(&threadFileManagement);
    }
    

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes have completed before closing files

    if (rank == 0) // Writing everything to the main file and deleting the temp files as it goes
        mainFileWriterAndDeleter(size);
  
    double endTime = MPI_Wtime();

    MPI_Reduce(&endTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {        
        printf("Total execution time (s): %lf\n", totalTime - startTime);
        printf("Done\n");
    }

    fileCloser(&fileManagement);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    
    return 0;
}

// All seed amounts are just fastion seeds, all times are single-threaded
// 2^16 = 65536 (.005 s) (CBViewer has 62 results)
// 2^20 = 1048576 (.068 s) (CBViewer has 760 results)
// 2^24 = 16777216 (1.088 s) (CBViewer has 12,118 results)
// 2^32 = 4294967296 (280.162 s) (CBViewer has 3,127,350 results)
// Est. 2^48 time: 19.92 hr (Based on 2^32 time, 256 threads)
// Est. structure seeds: 204,954,009,600 seeds (Based on 2^32 seedcount)
// Est. 64-bit seeds: 13,431,866,000,000,000 seeds (Based on 2^32 seedcount)

// For SSV Fastion End City Ship Seeds
// Est. structure seeds: 721,420,289 seeds (Based on 43 results for 16777216 seeds)
// Est. time: 722.909992 hr (Based on cluster using 512 cores)

// Fastion results match cubiomesViewer (tested from 0-2^32)

// TODO:
// Add a seed counter for all types and print data to console
// Update github accordingly