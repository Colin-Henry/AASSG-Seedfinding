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
            endIteration += extraIterations; // Add extra iterations to the last process
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

    
        char fastionSeedsFilename[1000]; // Arbitrary size, but is big enough to fit all names and takes very little memory
        snprintf(fastionSeedsFilename, sizeof(fastionSeedsFilename), "Output/Temp/fastionSeeds/fastionSeeds_temp_%d.txt", rank); // Generate unique filename for each process
        FILE *fastionSeeds = fopen(fastionSeedsFilename, "w");

        char ssvFastionSeedsFilename[1000];
        snprintf(ssvFastionSeedsFilename, sizeof(ssvFastionSeedsFilename), "Output/Temp/ssvFastionSeeds/ssvFastionSeeds_temp_%d.txt", rank);
        FILE *ssvFastionSeeds = fopen(ssvFastionSeedsFilename, "w");

        char fastionEndCitySeedsFilename[1000];
        snprintf(fastionEndCitySeedsFilename, sizeof(fastionEndCitySeedsFilename), "Output/Temp/fastionEndCitySeeds/fastionEndCitySeeds_temp_%d.txt", rank);
        FILE *fastionEndCitySeeds = fopen(fastionEndCitySeedsFilename, "w");

        char fastionEndCitySeedsWithCoordsFilename[1000];
        snprintf(fastionEndCitySeedsWithCoordsFilename, sizeof(fastionEndCitySeedsWithCoordsFilename), "Output/Temp/fastionEndCitySeedsWithCoords/fastionEndCitySeedsWithCoords_temp_%d.txt", rank);
        FILE *fastionEndCitySeedsWithCoords = fopen(fastionEndCitySeedsWithCoordsFilename, "w");

        char ssvFastionEndCitySeedsFilename[1000];
        snprintf(ssvFastionEndCitySeedsFilename, sizeof(ssvFastionEndCitySeedsFilename), "Output/Temp/ssvFastionEndCitySeeds/ssvFastionEndCitySeeds_temp_%d.txt", rank);
        FILE *ssvFastionEndCitySeeds = fopen(ssvFastionEndCitySeedsFilename, "w");

        char ssvFastionEndCitySeedsWithCoordsFilename[1000];
        snprintf(ssvFastionEndCitySeedsWithCoordsFilename, sizeof(ssvFastionEndCitySeedsWithCoordsFilename), "Output/Temp/ssvFastionEndCitySeedsWithCoords/ssvFastionEndCitySeedsWithCoords_temp_%d.txt", rank);
        FILE *ssvFastionEndCitySeedsWithCoords = fopen(ssvFastionEndCitySeedsWithCoordsFilename, "w");

        char fastionEndCityShipSeedsFilename[1000];
        snprintf(fastionEndCityShipSeedsFilename, sizeof(fastionEndCityShipSeedsFilename), "Output/Temp/fastionEndCityShipSeeds/fastionEndCityShipSeeds_temp_%d.txt", rank);
        FILE *fastionEndCityShipSeeds = fopen(fastionEndCityShipSeedsFilename, "w");

        char fastionEndCityShipSeedsWithCoordsFilename[1000];
        snprintf(fastionEndCityShipSeedsWithCoordsFilename, sizeof(fastionEndCityShipSeedsWithCoordsFilename), "Output/Temp/fastionEndCityShipSeedsWithCoords/fastionEndCityShipSeedsWithCoords_temp_%d.txt", rank);
        FILE *fastionEndCityShipSeedsWithCoords = fopen(fastionEndCityShipSeedsWithCoordsFilename, "w");

        char ssvFastionEndCityShipSeedsFilename[1000];
        snprintf(ssvFastionEndCityShipSeedsFilename, sizeof(ssvFastionEndCityShipSeedsFilename), "Output/Temp/ssvFastionEndCityShipSeeds/ssvFastionEndCityShipSeeds_temp_%d.txt", rank);
        FILE *ssvFastionEndCityShipSeeds = fopen(ssvFastionEndCityShipSeedsFilename, "w");

        char ssvFastionEndCityShipSeedsWithCoordsFilename[1000];
        snprintf(ssvFastionEndCityShipSeedsWithCoordsFilename, sizeof(ssvFastionEndCityShipSeedsWithCoordsFilename), "Output/Temp/ssvFastionEndCityShipSeedsWithCoords/ssvFastionEndCityShipSeedsWithCoords_temp_%d.txt", rank);
        FILE *ssvFastionEndCityShipSeedsWithCoords = fopen(ssvFastionEndCityShipSeedsWithCoordsFilename, "w");


        for (currentStructureSeed = startIteration; currentStructureSeed <= endIteration; currentStructureSeed++) 
        {
            bool isFastion = findFastions(currentStructureSeed, bastions, bastCount, forts, fortCount, &biomeSource, bastID, fortID);

            if (isFastion)
            {
                fprintf(fastionSeeds, "%" PRId64 "\n", currentStructureSeed);

                bool isSSV = checkForSSV(&forts[fortID], &biomeSource);
                if (isSSV)
                    fprintf(ssvFastionSeeds, "%" PRId64 "\n", currentStructureSeed);
                
                
                bool isEndCity = findEndCities(currentStructureSeed, &endCityCoords, &gatewayCoords); // Need to add in coord returns for coord printing
                if (isEndCity)
                {
                    fprintf(fastionEndCitySeeds, "%" PRId64 "\n", currentStructureSeed);
                    fprintf(fastionEndCitySeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);

                    if (isSSV)
                    {
                        fprintf(ssvFastionEndCitySeeds, "%" PRId64 "\n", currentStructureSeed);
                        fprintf(ssvFastionEndCitySeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);
                    }
                    
                    bool isEndCityShip = checkForShip(currentStructureSeed, endCityCoords); // Need to add in coord returns for coord printing
                    if (isEndCityShip)
                    {
                        fprintf(fastionEndCityShipSeeds, "%" PRId64 "\n", currentStructureSeed);
                        fprintf(fastionEndCityShipSeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);

                        if (isSSV)
                        {
                            fprintf(ssvFastionEndCityShipSeeds, "%" PRId64 "\n", currentStructureSeed);
                            fprintf(ssvFastionEndCityShipSeedsWithCoords, "%" PRId64 " %d %d\n", currentStructureSeed, gatewayCoords.x, gatewayCoords.z);
                        }
                    }
                }
            }
        }

        fclose(fastionSeeds);
        fclose(ssvFastionSeeds);
        fclose(fastionEndCitySeeds);
        fclose(fastionEndCitySeedsWithCoords);
        fclose(ssvFastionEndCitySeeds);
        fclose(ssvFastionEndCitySeedsWithCoords);
        fclose(fastionEndCityShipSeeds);
        fclose(fastionEndCityShipSeedsWithCoords);
        fclose(ssvFastionEndCityShipSeeds);
        fclose(ssvFastionEndCityShipSeedsWithCoords);
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
// 2^32 = 4294967296 (280.162 s) (CBViewer has 3,127,350 results)
// Est. 2^48 time: 19.92 hr (Based on 2^32 time, 256 threads)
// Est. structure seeds: 204,954,009,600 seeds (Based on 2^32 seedcount)
// Est. 64-bit seeds: 13,431,866,000,000,000 seeds (Based on 2^32 seedcount)

// For SSV Fastion, End Ship City Seeds
// Est. structure seeds: 352,321,536 seeds (Based on 21 results for 16777216 seeds)

// Fastion results match cubiomesViewer (tested from 0-2^32)

// TODO:
// Add thread protection for printing
// Figure out why the cluster is only printing for 1 node
// Sort data A-Z
// Additional testing if needed
// Optimization if needed
// Update github accordingly