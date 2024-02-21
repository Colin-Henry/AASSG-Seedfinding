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

    uint64_t totalIterations = endingStructureSeed - startingStructureSeed + 1;
    uint64_t iterationsPerProcess = totalIterations / size;
    uint64_t extraIterations = totalIterations % size;

    uint64_t startIteration = rank * iterationsPerProcess + startingStructureSeed;
    uint64_t endIteration = (rank + 1) * iterationsPerProcess - 1 + startingStructureSeed;
    if (rank == size - 1) 
    {
        endIteration += extraIterations; // Add extra iterations to the last process
    }

    Pos bastions[4], forts[4];
    int bastCount = 0;
    int fortCount = 0;
    int bastID = 0; 
    int fortID = 0;

    Generator biomeSource;
    setupGenerator(&biomeSource, MC_1_16_1, 0);

    for (currentStructureSeed = startIteration; currentStructureSeed <= endIteration; currentStructureSeed++) 
    {
        bool isFastion = findFastions(currentStructureSeed, bastions, bastCount, forts, fortCount, &biomeSource, bastID, fortID);
        if (isFastion)
        {
            fprintf(fileManagement.fastionSeeds, "%" PRId64 "\n", currentStructureSeed);
            bool isSSV = checkForSSV(&forts[fortID], &biomeSource);
            if (isSSV)
            {
                fprintf(fileManagement.ssvFastionSeeds, "%" PRId64 "\n", currentStructureSeed);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes have completed before closing files
    
    double endTime = MPI_Wtime(); // End time of the computation

    // Calculate total time on rank 0
    MPI_Reduce(&endTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) 
    {        
        printf("Total execution time (s): %lf\n", totalTime - startTime); // Print total execution time
        printf("Done\n"); // Print "done" message once
    }

    fileCloser(&fileManagement);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize(); // Finalize MPI
    
    return 0;
}

// All seed amounts are just fastion seeds, all times are single-threaded
// 2^16 = 65536 (.005 s) (CBViewer has 62 results)
// 2^20 = 1048576 (.068 s) (CBViewer has 760 results)
// 2^24 = 16777216 (1.088 s) (CBViewer has 12,118 results)
// 2^32 = 4294967296 (280.162) (CBViewer has 3,127,350 results)
// Est. 2^48 time: 19.92 hr (Based on 2^32 time, 256 threads)
// Est. structure seeds: 204,954,009,600 seeds (Based on 2^32 seedcount)
// Est. 64-bit seeds: 13,431,866,000,000,000 seeds (Based on 2^32 seedcount)

// Results match cubiomesViewer (tested from 0-2^32)

// TODO:
// Add thread protection for printing
// Figure out why the cluster is only printing for 1 node
// Sort data A-Z
// Move ssv detection to another function
// Create if statements after each seedsift requirement is passed
// Add end city detection to the end filter
// Add ship detection to the end filter
// Add print statements to the end filter
// Add coordinate print statements to to the end filter
// Additional testing if needed
// Optimization if needed
// Update github accordingly

// Check once for ssv. If it's ssv, create a flag saying so. Then, at all other print statements, add a check for that flag. If its an ssv seed, also print that seed to the accompanying ssv list