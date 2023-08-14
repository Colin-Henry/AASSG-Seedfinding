#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "finders.c"
#include "generator.c"
#include "biome_tree.c"
#include "layers.c"
#include "noise.c"

typedef struct 
{
    Pos first, second;
} DoublePos;

typedef struct 
{
    DoublePos regionCoords;
    int dimension;
    Pos candidates[4];
    Pos positions[4];
    int candidatesCount;
    int positionsCount;
} StructData;

DoublePos origCoords = {{-96, -96}, {96, 96}};
int STRUCTS[] = {Bastion, Fortress};
const int MC = MC_1_16_1;
const int UPPER_BITS_TO_CHECK = 1;
int structureIndex = 0;

int structureChecker(int lower48, int STRUCTS[], int structureIndex, int MC, DoublePos origCoords, StructData data[], int result, Pos* bastionCoordinates, Pos* fortCoordinates);

int structureChecker(int lower48, int STRUCTS[], int structureIndex, int MC, DoublePos origCoords, StructData data[], int result, Pos* bastionCoordinates, Pos* fortCoordinates) 
{
    Pos p;
    int i = structureIndex; // Use the correct data element corresponding to the structure being checked
    data[i].candidatesCount = 0;
    int currentStructure = STRUCTS[structureIndex];

    for (int regX = data[i].regionCoords.first.x; regX <= data[i].regionCoords.second.x; ++regX) 
    {
        for (int regZ = data[i].regionCoords.first.z; regZ <= data[i].regionCoords.second.z; ++regZ) 
        {
            if (!getStructurePos(currentStructure, MC, lower48, regX, regZ, &p)) continue;

            if ((regX == data[i].regionCoords.first.x && p.x < origCoords.first.x) ||
                (regX == data[i].regionCoords.second.x && p.x > origCoords.second.x) ||
                (regZ == data[i].regionCoords.first.z && p.z < origCoords.first.z) ||
                (regZ == data[i].regionCoords.second.z && p.z > origCoords.second.z)) continue;

            data[i].candidates[data[i].candidatesCount] = p;
            data[i].candidatesCount++;
        }
    }

    if (!data[i].candidatesCount) 
    {
        result = 1;
    } 
    else if (structureIndex == 0) 
    {
        result = 2;
        for (int j = 0; j < data[i].candidatesCount; ++j) 
        {
            bastionCoordinates[j].x = data[i].candidates[j].x;
            bastionCoordinates[j].z = data[i].candidates[j].z;
        }
    } 
    else if (structureIndex == 1) 
    {
        result = 2;
        for (int j = 0; j < data[i].candidatesCount; ++j) 
        {
            fortCoordinates[j].x = data[i].candidates[j].x;
            fortCoordinates[j].z = data[i].candidates[j].z;
        }
    }
    return result;
}

void printBinary(uint64_t number, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        uint64_t mask = (uint64_t)1 << i;
        putchar((number & mask) ? '1' : '0');
    }
}

int main(int argc, char **argv) 
{
    FILE *fp;
    char *inputPath = "seedRange.txt";

    for (int i = 1; i < argc; i++) 
    {
        if (strcmp(argv[i], "--input") == 0) 
        {
            inputPath = argv[i + 1];
        }
    }

    if (inputPath == NULL) 
    {
        printf("Error: --input argument is required.\n");
        return 1;
    }

    fp = fopen(inputPath, "r");
    if (fp == NULL) 
    {
        printf("Error: Unable to open input file.\n");
        return 1;
    }

    uint64_t START_STRUCTURE_SEED, STRUCTURE_SEEDS_TO_CHECK;
    if (fscanf(fp, "%" SCNu64 "%" SCNu64, &START_STRUCTURE_SEED, &STRUCTURE_SEEDS_TO_CHECK) != 2) 
    {
        printf("Error: Unable to read seeds from file.\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);
    
    const int numberOfStructs = sizeof(STRUCTS) / sizeof(*STRUCTS);
    StructData data[numberOfStructs];

    int i = 0;
    data[i].candidatesCount = 0;
    data[i].positionsCount = 0;

    Pos bastionCoordinates[4];
    Pos fortressCoordinates;
    Pos fortCoordinates[4];

    for (int i = 0; i < numberOfStructs; ++i) 
    {
        StructureConfig currentStructureConfig;
        if (!getStructureConfig(STRUCTS[i], MC, &currentStructureConfig)) 
        {
            printf("ERROR: Structure #%d in the STRUCTS array cannot exist in the specified version.\n", i);
            exit(1);
        }

        switch (currentStructureConfig.regionSize) 
        {
            case 32:
                data[i].regionCoords.first.x = origCoords.first.x >> 9;
                data[i].regionCoords.first.z = origCoords.first.z >> 9;
                data[i].regionCoords.second.x = origCoords.second.x >> 9;
                data[i].regionCoords.second.z = origCoords.second.z >> 9;
                break;
            case 1:
                data[i].regionCoords.first.x = origCoords.first.x >> 4;
                data[i].regionCoords.first.z = origCoords.first.z >> 4;
                data[i].regionCoords.second.x = origCoords.second.x >> 4;
                data[i].regionCoords.second.z = origCoords.second.z >> 4;
                break;
            default:
                data[i].regionCoords.first.x = (origCoords.first.x / (currentStructureConfig.regionSize << 4)) - (origCoords.first.x < 0);
                data[i].regionCoords.first.z = (origCoords.first.z / (currentStructureConfig.regionSize << 4)) - (origCoords.first.z < 0);
                data[i].regionCoords.second.x = (origCoords.second.x / (currentStructureConfig.regionSize << 4)) - (origCoords.second.x < 0);
                data[i].regionCoords.second.z = (origCoords.second.z / (currentStructureConfig.regionSize << 4)) - (origCoords.second.z < 0);
                break;
        }

        data[i].dimension = currentStructureConfig.properties & STRUCT_NETHER ? DIM_NETHER :
                            currentStructureConfig.properties & STRUCT_END    ? DIM_END :
                                                                                DIM_OVERWORLD;
    }

    Generator g;
    setupGenerator(&g, MC, 0);

    int biome = 0;
    int result = 0;
    uint64_t seed;

    for (uint64_t lower48 = START_STRUCTURE_SEED; lower48 < STRUCTURE_SEEDS_TO_CHECK; ++lower48) 
    {
        DoublePos bastionCoords = {{-96, -96}, {96, 96}};
        int structureIndex = 0;

        result = structureChecker(lower48, STRUCTS, 0, MC, bastionCoords, data, result, bastionCoordinates, fortCoordinates);
        if (result == 1) 
        {
            continue;
        } 
        else 
        {
            for (int bastionIdx = 0; bastionIdx < data[0].candidatesCount; ++bastionIdx) 
            {
                DoublePos fortressCoords = {{-96 + bastionCoordinates[bastionIdx].x, -96 + bastionCoordinates[bastionIdx].z}, {96 + bastionCoordinates[bastionIdx].x, 96 + bastionCoordinates[bastionIdx].z}};
                int structureIndex = 1;
                result = structureChecker(lower48, STRUCTS, 1, MC, fortressCoords, data, result, &fortressCoordinates, fortCoordinates);
                if (result == 1) 
                {
                    continue;
                } 
                else 
                {
                    int allChecksFailed = 1;
                    for (uint64_t upper16 = 0; upper16 < UPPER_BITS_TO_CHECK; ++upper16) 
                    {
                        seed = lower48 | (upper16 << 48);
                        for (int i = 0; i < numberOfStructs; ++i) 
                        {
                            data[i].positionsCount = 0;
                            if (g.seed != seed || g.dim != data[i].dimension) applySeed(&g, data[i].dimension, seed);
                            for (int j = 0; j < data[i].candidatesCount; ++j) 
                            {
                                if (!isViableStructurePos(STRUCTS[i], &g, data[i].candidates[j].x, data[i].candidates[j].z, 0) ||
                                    !isViableStructureTerrain(STRUCTS[i], &g, data[i].candidates[j].x, data[i].candidates[j].z))
                                    continue;
                                data[i].positions[data[i].positionsCount] = data[i].candidates[j];
                                data[i].positionsCount++;
                            }
                            if (data[i].positionsCount > 0) 
                            {
                                allChecksFailed = 0;
                                break;
                            }
                        }
                        if (!allChecksFailed) 
                            break;
                    }

                    if (allChecksFailed) 
                        continue;

                    biome = getBiomeAt(&g, 1, bastionCoordinates[bastionIdx].x, 64, bastionCoordinates[bastionIdx].z);
                    if (biome == basalt_deltas) 
                    {
                        goto nextStructureSeed;
                    }
                    printf("A");
                    printBinary(lower48, 48);
                    printf("\n");

                    biome = getBiomeAt(&g, 1, fortCoordinates[bastionIdx].x, 64, fortCoordinates[bastionIdx].z); 
                    if (biome != soul_sand_valley) 
                    {    
                        goto nextStructureSeed;
                    }
                    printf("B");
                    printBinary(lower48, 48);
                    printf("\n");

                    goto nextStructureSeed;
                }
            }
        }

        nextStructureSeed:

        for (int i = 0; i < numberOfStructs; ++i) 
        {
            data[i].candidatesCount = 0;
            data[i].positionsCount = 0;
        }
        continue;
    }
    fclose(fp);
    return 0;
}
