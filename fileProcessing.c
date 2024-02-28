#include "fileProcessing.h"
#include <stdio.h>
#include <inttypes.h>

void fileOpener(struct fileManagement *fileManagement)
{
    fileManagement->seedRange = fopen("seedRange.txt", "r");
        if (fileManagement->seedRange == NULL) perror("Error opening seedRange.txt");
    fileManagement->fastionSeeds = fopen("Output/fastionSeeds.txt", "w");
        if (fileManagement->fastionSeeds == NULL) perror("Error opening fastionSeeds.txt");
    fileManagement->ssvFastionSeeds = fopen("Output/ssvFastionSeeds.txt", "w");
        if (fileManagement->ssvFastionSeeds == NULL) perror("Error opening ssvFastionSeeds.txt");

    fileManagement->fastionEndCitySeeds = fopen("Output/fastionEndCitySeeds.txt", "w");
        if (fileManagement->fastionEndCitySeeds == NULL) perror("Error opening fastionEndCitySeeds.txt");

    fileManagement->fastionEndCitySeedsWithCoords = fopen("Output/fastionEndCitySeedsWithCoords.txt", "w");
        if (fileManagement->fastionEndCitySeedsWithCoords == NULL) perror("Error opening fastionEndCitySeedsWithCoords.txt");

    fileManagement->ssvFastionEndCitySeeds = fopen("Output/ssvFastionEndCitySeeds.txt", "w");
        if (fileManagement->ssvFastionEndCitySeeds == NULL) perror("Error opening ssvFastionEndCitySeeds.txt");

    fileManagement->ssvFastionEndCitySeedsWithCoords = fopen("Output/ssvFastionEndCitySeedsWithCoords.txt", "w");
        if (fileManagement->ssvFastionEndCitySeedsWithCoords == NULL) perror("Error opening ssvFastionEndCitySeedsWithCoords.txt");

    fileManagement->fastionEndCityShipSeeds = fopen("Output/fastionEndCityShipSeeds.txt", "w");
        if (fileManagement->fastionEndCityShipSeeds == NULL) perror("Error opening fastionEndCityShipSeeds.txt");

    fileManagement->fastionEndCityShipSeedsWithCoords = fopen("Output/fastionEndCityShipSeedsWithCoords.txt", "w");
        if (fileManagement->fastionEndCityShipSeedsWithCoords == NULL) perror("Error opening fastionEndCityShipSeedsWithCoords.txt");

    fileManagement->ssvFastionEndCityShipSeeds = fopen("Output/ssvFastionEndCityShipSeeds.txt", "w");
        if (fileManagement->ssvFastionEndCityShipSeeds == NULL) perror("Error opening ssvFastionEndCityShipSeeds.txt");

    fileManagement->ssvFastionEndCityShipSeedsWithCoords = fopen("Output/ssvFastionEndCityShipSeedsWithCoords.txt", "w");
        if (fileManagement->ssvFastionEndCityShipSeedsWithCoords == NULL) perror("Error opening ssvFastionEndCityShipSeedsWithCoords.txt");
}

void fileCloser(struct fileManagement *fileManagement) 
{
    if (fileManagement->seedRange != NULL) fclose(fileManagement->seedRange);
    if (fileManagement->fastionSeeds != NULL) fclose(fileManagement->fastionSeeds);
    if (fileManagement->ssvFastionSeeds != NULL) fclose(fileManagement->ssvFastionSeeds);
    if (fileManagement->fastionEndCitySeeds != NULL) fclose(fileManagement->fastionEndCitySeeds);
    if (fileManagement->fastionEndCitySeedsWithCoords != NULL) fclose(fileManagement->fastionEndCitySeedsWithCoords);
    if (fileManagement->ssvFastionEndCitySeeds != NULL) fclose(fileManagement->ssvFastionEndCitySeeds);
    if (fileManagement->ssvFastionEndCitySeedsWithCoords != NULL) fclose(fileManagement->ssvFastionEndCitySeedsWithCoords);
    if (fileManagement->fastionEndCityShipSeeds != NULL) fclose(fileManagement->fastionEndCityShipSeeds);
    if (fileManagement->fastionEndCityShipSeedsWithCoords != NULL) fclose(fileManagement->fastionEndCityShipSeedsWithCoords);
    if (fileManagement->ssvFastionEndCityShipSeeds != NULL) fclose(fileManagement->ssvFastionEndCityShipSeeds);
    if (fileManagement->ssvFastionEndCityShipSeedsWithCoords != NULL) fclose(fileManagement->ssvFastionEndCityShipSeedsWithCoords);
}

void readSeedRange(struct fileManagement *fileManagement, uint64_t *startingStructureSeed, uint64_t *endingStructureSeed)
{
    int scanResult = fscanf(fileManagement->seedRange, "%" SCNu64 "%" SCNu64, startingStructureSeed, endingStructureSeed);
    if (scanResult != 2)
        printf("Error reading seeds from file.\n");
}