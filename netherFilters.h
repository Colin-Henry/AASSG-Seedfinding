#pragma once

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "cubiomes.h"

int getRegionPos(Pos* structurePos, uint64_t structureSeed, int rx, int rz);
void getNetherStructs(uint64_t structureSeed, Pos bastions[], int* bastionCount, Pos forts[], int* fortCount);
bool checkStructureDistances(Pos* bast, Pos* fort);
bool checkStructureReqs(const Generator* biomeSource, Pos* bast, Pos* fort);
void findSSVFastions(uint64_t currentStructureSeed, Pos* bastions, int bastCount, Pos* forts, int fortCount, const Generator* biomeSource);