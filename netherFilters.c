#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "cubiomes.h"
#include "fileProcessing.h"

int getRegionPos(Pos* structurePos, uint64_t structureSeed, int rx, int rz)
{
    uint64_t* rng = &structureSeed;
    setSeed(rng, rx*341873128712ULL + rz*132897987541ULL + *rng + 30084232ULL);
    structurePos->x = (rx * 27 + nextInt(rng, 23)) << 4;
    structurePos->z = (rz * 27 + nextInt(rng, 23)) << 4;
    return nextInt(rng, 5);
}

void getNetherStructs(uint64_t structureSeed, Pos bastions[], int* bastionCount, Pos forts[], int* fortCount)
{
    Pos structurePos;
    *bastionCount = 0;
    *fortCount = 0;
    
    for (int rx = -1; rx < 1; rx++) 
    {
        for (int rz = -1; rz < 1; rz++)
        {
            int structureType = getRegionPos(&structurePos, structureSeed, rx, rz);
            if ((structureType >= 2) && (abs(structurePos.x) <= 96 && abs(structurePos.z) <= 96)) // bastion position filtering
                bastions[(*bastionCount)++] = structurePos; // it's a bastion
            if ((structureType < 2) && (abs(structurePos.x) <= 192 && abs(structurePos.z) <= 192)) // fortress position filtering
                forts[(*fortCount)++] = structurePos; // it's a fortress
        }
    }
}

bool checkStructureDistances(Pos* bast, Pos* fort)
{
    int dX = abs(bast->x - fort->x);
    int dZ = abs(bast->z - fort->z);
    if (dX > 96 || dZ > 96)
        return false;
    else return true;
}

bool checkStructureReqs(const Generator* biomeSource, Pos* bast, Pos* fort)
{
    if (!checkStructureDistances(bast, fort))
        return false;
    else

    applySeed((Generator *)biomeSource, DIM_NETHER, currentStructureSeed);

    if ((getBiomeAt(biomeSource, 4, (bast->x >> 2) + 2, 2, (bast->z >> 2) + 2)) == basalt_deltas)
        return false; // check if bastion spawns

    fprintf(fileManagement.fastionSeeds, "%" PRId64 "\n", currentStructureSeed);

    if (getBiomeAt(biomeSource, 4, (fort->x >> 2), 2, (fort->z >> 2)) == soul_sand_valley) return true; // Center
    if (getBiomeAt(biomeSource, 4, (fort->x >> 2) + 8, 2, (fort->z >> 2)) == soul_sand_valley) return true; // Right
    if (getBiomeAt(biomeSource, 4, (fort->x >> 2) - 8, 2, (fort->z >> 2)) == soul_sand_valley) return true; // Left
    if (getBiomeAt(biomeSource, 4, (fort->x >> 2), 2, (fort->z >> 2) + 8) == soul_sand_valley) return true; // Top
    if (getBiomeAt(biomeSource, 4, (fort->x >> 2), 2, (fort->z >> 2) - 8) == soul_sand_valley) return true; // Bottom
    else return false; // none of them had ssv 
}

void findSSVFastions(uint64_t currentStructureSeed, Pos* bastions, int bastCount, Pos* forts, int fortCount, const Generator* biomeSource)
{
    getNetherStructs(currentStructureSeed, bastions, &bastCount, forts, &fortCount);
    if (bastCount == 0 || fortCount == 0) // skip seeds that don't have a bastion or don't have a fort
        return;

    // check the requirements for each bastion-fortress pair
    for (int bastID = 0; bastID < bastCount; bastID++)
    {
        for (int fortID = 0; fortID < fortCount; fortID++)
        {
            if (checkStructureReqs(biomeSource, &(bastions[bastID]), &(forts[fortID])))
            {
                fprintf(fileManagement.ssvFastionSeeds, "%" PRId64 "\n", currentStructureSeed);
                return;
            }
        }
    }
}