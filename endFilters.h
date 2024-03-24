#pragma once

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include "cubiomes.h"


// ----------
// macros
// ----------

#define ref(X) const X* const
#define NOISE_REGION_SIZE 6

// ----------
// data types
// ----------

typedef struct {
    double x;
    double z;
} Vec;

typedef struct {
    int minX;
    int minZ;
    int maxX;
    int maxZ;
} BlockArea;

typedef struct {
    Pos3 pos;
    double r;
} EndIsland;

typedef struct {
    int size;
    EndIsland first;
    EndIsland second;
} EndIslandPair;

// this struct is a holder for all the noise column values used in terrain generation
// for a single chunk, only the first 3x3 noise columns are used
// for getTallestBlock, all 6x6 cols. get sampled and stored
typedef struct {
    double noisecolumn[NOISE_REGION_SIZE][NOISE_REGION_SIZE][33];
} NoiseRegion;

typedef struct {
    SurfaceNoise surfaceNoise;
    EndNoise endNoise;
    NoiseRegion noiseRegion;
    uint64_t seed;
} EndTerrainNoise;

// -----------------------
// inline functs, helpers
// -----------------------

inline int min(const int a, const int b) 
{ return a < b ? a : b; }

inline int max(const int a, const int b) 
{ return a > b ? a : b; }

inline void setPos(Pos3* const pos, const int x, const int y, const int z) 
{
    pos->x = x;
    pos->y = y;
    pos->z = z;
}

inline uint64_t getPosAsLong(Pos3* const pos) 
{
    // this encodes the x,y,z coordinates directly as a Java long
    const int PACKED_XZ_LENGTH = 26;
    const int PACKED_Y_LENGTH = 64 - 2*PACKED_XZ_LENGTH;
    const uint64_t PACKED_XZ_MASK = (1ULL << PACKED_XZ_LENGTH) - 1;
    const uint64_t PACKED_Y_MASK = (1ULL << PACKED_Y_LENGTH) - 1;

    uint64_t l = 0ULL;
    l |= ((uint64_t)pos->x & PACKED_XZ_MASK) << (PACKED_XZ_LENGTH + PACKED_Y_LENGTH);
    l |= ((uint64_t)pos->y & PACKED_Y_MASK) << 0;
    return l |= ((uint64_t)pos->z & PACKED_XZ_MASK) << PACKED_Y_LENGTH;
}


// -----------------------
// function headers
// -----------------------

bool blockIsWithinIsland(ref(EndIsland) island, const int x, const int z);
bool islandIntersects(ref(EndIsland) island, ref(BlockArea) area);
bool anyIslandIntersectsChunk(ref(EndIslandPair) islandPair, int chunkX, int chunkZ);
void addIntersectingIslands(EndIsland *arr, int *arrSize, ref(EndIslandPair) islandPair, ref(BlockArea) area, int minHeight);

uint64_t getPopSeed(uint64_t ws, int x, int z);
EndIslandPair generateIslands(Generator *endBiomeGenerator, uint64_t lower48, int xCoordinate, int zCoordinate);
Pos3 getTopPriorityBlock(ref(EndIsland) island, ref(BlockArea) area);
Pos3 getIslandTallestBlock(Generator *endBiomeGenerator, uint64_t lower48, ref(Pos3) center, const int maxSurfaceHeight);
bool chunkHasIslandBlocks(ref(Pos) chunkPos, Generator *endBiomeGenerator, uint64_t lower48);

void initEndTerrainNoise(EndTerrainNoise *const etn, uint64_t lower48);
void sampleColumn(EndTerrainNoise *const etn, int cellX, int cellZ, const int minX, const int minZ);
void sampleNoiseColumnsCached(EndTerrainNoise *const etn, bool noiseCalculated[][NOISE_REGION_SIZE], int cellX, int cellZ, const int minX, const int minZ);
int getHeightAt(EndTerrainNoise *const etn, int arrayCellX, int arrayCellZ, int x, int z);
bool chunkHasTerrainBlocks(ref(Pos) chunkPos, EndTerrainNoise *const etn, bool minHeight30);
Pos3 getTerrainTallestBlock(ref(Pos3) center, EndTerrainNoise *const etn);

Pos getMainGateway(uint64_t lower48);
Pos3 linkedGateway(uint64_t lower48);

void sampleNoiseColumnEnd(double column[], const SurfaceNoise *sn, const EndNoise *en, int x, int z, int colymin, int colymax);
int getSurfaceHeight(const double ncol00[], const double ncol01[], const double ncol10[], const double ncol11[], int colymin, int colymax, int blockspercell, double dx, double dz);

bool isEndCityNearby(uint64_t lower48, Generator* endBiomeSource, SurfaceNoise* endSurfaceNoise);
bool findEndCities(uint64_t lower48, Pos* endCityCoords, Pos3* gatewayCoords);
bool checkForShip(uint64_t lower48, Pos endCityCoords);
