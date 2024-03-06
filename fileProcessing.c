#include "fileProcessing.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

void threadFileOpener(struct threadFileManagement *threadFileManagement, int rank)
{
    char fastionSeedsFilename[256]; // Arbitrary size, but is big enough to fit all names and takes very little memory
    snprintf(fastionSeedsFilename, sizeof(fastionSeedsFilename), "Output/Temp/fastionSeeds/fastionSeeds_temp_%d.txt", rank); // Generate unique filename for each process
    threadFileManagement->fastionSeeds = fopen(fastionSeedsFilename, "w");

    char ssvFastionSeedsFilename[256];
    snprintf(ssvFastionSeedsFilename, sizeof(ssvFastionSeedsFilename), "Output/Temp/ssvFastionSeeds/ssvFastionSeeds_temp_%d.txt", rank);
    threadFileManagement->ssvFastionSeeds = fopen(ssvFastionSeedsFilename, "w");

    char fastionEndCitySeedsFilename[256];
    snprintf(fastionEndCitySeedsFilename, sizeof(fastionEndCitySeedsFilename), "Output/Temp/fastionEndCitySeeds/fastionEndCitySeeds_temp_%d.txt", rank);
    threadFileManagement->fastionEndCitySeeds = fopen(fastionEndCitySeedsFilename, "w");

    char fastionEndCitySeedsWithCoordsFilename[256];
    snprintf(fastionEndCitySeedsWithCoordsFilename, sizeof(fastionEndCitySeedsWithCoordsFilename), "Output/Temp/fastionEndCitySeedsWithCoords/fastionEndCitySeedsWithCoords_temp_%d.txt", rank);
    threadFileManagement->fastionEndCitySeedsWithCoords = fopen(fastionEndCitySeedsWithCoordsFilename, "w");

    char ssvFastionEndCitySeedsFilename[256];
    snprintf(ssvFastionEndCitySeedsFilename, sizeof(ssvFastionEndCitySeedsFilename), "Output/Temp/ssvFastionEndCitySeeds/ssvFastionEndCitySeeds_temp_%d.txt", rank);
    threadFileManagement->ssvFastionEndCitySeeds = fopen(ssvFastionEndCitySeedsFilename, "w");

    char ssvFastionEndCitySeedsWithCoordsFilename[256];
    snprintf(ssvFastionEndCitySeedsWithCoordsFilename, sizeof(ssvFastionEndCitySeedsWithCoordsFilename), "Output/Temp/ssvFastionEndCitySeedsWithCoords/ssvFastionEndCitySeedsWithCoords_temp_%d.txt", rank);
    threadFileManagement->ssvFastionEndCitySeedsWithCoords = fopen(ssvFastionEndCitySeedsWithCoordsFilename, "w");

    char fastionEndCityShipSeedsFilename[256];
    snprintf(fastionEndCityShipSeedsFilename, sizeof(fastionEndCityShipSeedsFilename), "Output/Temp/fastionEndCityShipSeeds/fastionEndCityShipSeeds_temp_%d.txt", rank);
    threadFileManagement->fastionEndCityShipSeeds = fopen(fastionEndCityShipSeedsFilename, "w");

    char fastionEndCityShipSeedsWithCoordsFilename[256];
    snprintf(fastionEndCityShipSeedsWithCoordsFilename, sizeof(fastionEndCityShipSeedsWithCoordsFilename), "Output/Temp/fastionEndCityShipSeedsWithCoords/fastionEndCityShipSeedsWithCoords_temp_%d.txt", rank);
    threadFileManagement->fastionEndCityShipSeedsWithCoords = fopen(fastionEndCityShipSeedsWithCoordsFilename, "w");

    char ssvFastionEndCityShipSeedsFilename[256];
    snprintf(ssvFastionEndCityShipSeedsFilename, sizeof(ssvFastionEndCityShipSeedsFilename), "Output/Temp/ssvFastionEndCityShipSeeds/ssvFastionEndCityShipSeeds_temp_%d.txt", rank);
    threadFileManagement->ssvFastionEndCityShipSeeds = fopen(ssvFastionEndCityShipSeedsFilename, "w");

    char ssvFastionEndCityShipSeedsWithCoordsFilename[256];
    snprintf(ssvFastionEndCityShipSeedsWithCoordsFilename, sizeof(ssvFastionEndCityShipSeedsWithCoordsFilename), "Output/Temp/ssvFastionEndCityShipSeedsWithCoords/ssvFastionEndCityShipSeedsWithCoords_temp_%d.txt", rank);
    threadFileManagement->ssvFastionEndCityShipSeedsWithCoords = fopen(ssvFastionEndCityShipSeedsWithCoordsFilename, "w");
}

void mainFileWriterAndDeleter(int size)
{
    for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for fastions
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/fastionSeeds/fastionSeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            // Copying over everything from the temp file to the main file
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.fastionSeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for ssv fastions
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/ssvFastionSeeds/ssvFastionSeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.ssvFastionSeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for fastion end cities
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/fastionEndCitySeeds/fastionEndCitySeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.fastionEndCitySeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for fastion end cities with coords
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/fastionEndCitySeedsWithCoords/fastionEndCitySeedsWithCoords_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.fastionEndCitySeedsWithCoords);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for ssv fastion end cities
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/ssvFastionEndCitySeeds/ssvFastionEndCitySeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.ssvFastionEndCitySeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for ssv fastion end cities with coords
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/ssvFastionEndCitySeedsWithCoords/ssvFastionEndCitySeedsWithCoords_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.ssvFastionEndCitySeedsWithCoords);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for fastion end city ships
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/fastionEndCityShipSeeds/fastionEndCityShipSeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.fastionEndCityShipSeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for fastion end city ships with coords
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/fastionEndCityShipSeedsWithCoords/fastionEndCityShipSeedsWithCoords_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.fastionEndCityShipSeedsWithCoords);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for ssv fastion end city ships
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/ssvFastionEndCityShipSeeds/ssvFastionEndCityShipSeeds_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.ssvFastionEndCityShipSeeds);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }

        for (int currentTempFile = 1; currentTempFile <= (size - 1); currentTempFile++) // for ssv fastion end city ships with coords
        {
            char tempFile[256];
            snprintf(tempFile, sizeof(tempFile), "Output/Temp/ssvFastionEndCityShipSeedsWithCoords/ssvFastionEndCityShipSeedsWithCoords_temp_%d.txt", currentTempFile);
        
            FILE *tempInputFile = fopen(tempFile, "r");
            if (tempInputFile == NULL) 
                perror("Error opening input file");

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempInputFile)) > 0) 
                fwrite(buffer, 1, bytesRead, fileManagement.ssvFastionEndCityShipSeedsWithCoords);

            fclose(tempInputFile);

            if (remove(tempFile) != 0) 
                perror("Error deleting temporary file");
        }
}

void threadFileCloser(struct threadFileManagement *threadFileManagement)
{
    fclose(threadFileManagement->fastionSeeds);
    fclose(threadFileManagement->ssvFastionSeeds);
    fclose(threadFileManagement->fastionEndCitySeeds);
    fclose(threadFileManagement->fastionEndCitySeedsWithCoords);
    fclose(threadFileManagement->ssvFastionEndCitySeeds);
    fclose(threadFileManagement->ssvFastionEndCitySeedsWithCoords);
    fclose(threadFileManagement->fastionEndCityShipSeeds);
    fclose(threadFileManagement->fastionEndCityShipSeedsWithCoords);
    fclose(threadFileManagement->ssvFastionEndCityShipSeeds);
    fclose(threadFileManagement->ssvFastionEndCityShipSeedsWithCoords);
}