#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "finders.c"
#include "generator.c"
#include "biome_tree.c"
#include "layers.c"
#include "noise.c"

typedef struct {
    Pos first, second;
} DoublePos;

typedef struct ListNode {
    Pos position;
    struct ListNode* next;
} ListNode;

typedef struct {
    DoublePos regionCoords;
    int dimension;
    ListNode* candidatesHead;
    ListNode* positionsHead;
    int candidatesCount;
    int positionsCount;
} StructData;

const int r = 0;
DoublePos origCoords = {{-96, -96}, {96, 96}};
const char* FILEPATH = "seeds.txt";
int STRUCTS[] = {Bastion, Fortress};
const int MC = MC_1_16_1;
const uint64_t START_STRUCTURE_SEED = 0;
const uint64_t STRUCTURE_SEEDS_TO_CHECK = 6252; //Program dies between 6252 and 6253
const int UPPER_BITS_TO_CHECK = 1;
int structureIndex = 0;

int structureChecker(int lower48, int STRUCTS[], int structureIndex, int MC, DoublePos origCoords, StructData data[], int dataSize, int result, Pos* bastionCoordinates);

void addCandidate(ListNode** head, Pos p) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    newNode->position = p;
    newNode->next = *head;
    *head = newNode;
}

void addPosition(ListNode** head, Pos p) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    newNode->position = p;
    newNode->next = *head;
    *head = newNode;
}

int structureChecker(int lower48, int STRUCTS[], int structureIndex, int MC, DoublePos origCoords, StructData data[], int dataSize, int result, Pos* bastionCoordinates) {
    Pos p;
    int i = 0;
    data[i].candidatesCount = 0;
    int currentStructure = STRUCTS[structureIndex];
    for (int regX = data[i].regionCoords.first.x; regX <= data[i].regionCoords.second.x; ++regX) {
        for (int regZ = data[i].regionCoords.first.z; regZ <= data[i].regionCoords.second.z; ++regZ) {
            if (!getStructurePos(currentStructure, MC, lower48, regX, regZ, &p)) continue;

            if ((regX == data[i].regionCoords.first.x && p.x < origCoords.first.x) ||
                (regX == data[i].regionCoords.second.x && p.x > origCoords.second.x) ||
                (regZ == data[i].regionCoords.first.z && p.z < origCoords.first.z) ||
                (regZ == data[i].regionCoords.second.z && p.z > origCoords.second.z)) continue;

            addCandidate(&data[i].candidatesHead, p);
            ++data[i].candidatesCount;
        }
    }

    if (!data[i].candidatesCount) {
        result = 1;
    } else if (structureIndex == 0) {
        result = 2;
        bastionCoordinates->x = data[i].candidatesHead->position.x;
        bastionCoordinates->z = data[i].candidatesHead->position.z;
    } else {
        result = 2;
    }
    return result;
}

int main() {
    const int numberOfStructs = sizeof(STRUCTS) / sizeof(*STRUCTS);
    StructData data[numberOfStructs];

    struct Node* head = NULL;

    int i = 0;
    data[i].candidatesHead = NULL;
    data[i].positionsHead = NULL;

    Pos bastionCoordinates;
    Pos fortressCoordinates;

    for (int i = 0; i < numberOfStructs; ++i) {
        StructureConfig currentStructureConfig;
        if (!getStructureConfig(STRUCTS[i], MC, &currentStructureConfig)) {
            printf("ERROR: Structure #%d in the STRUCTS array cannot exist in the specified version.\n", i);
            exit(1);
        }

        switch (currentStructureConfig.regionSize) {
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
                            currentStructureConfig.properties & STRUCT_END ? DIM_END :
                            DIM_OVERWORLD;
    }

    FILE* fp = fopen(FILEPATH, "a");
    if (!fp) exit(1);

    Generator g;
    setupGenerator(&g, MC, 0);

    int biome = 0;

    int result = 0;

    for (uint64_t lower48 = START_STRUCTURE_SEED; lower48 < STRUCTURE_SEEDS_TO_CHECK; ++lower48) {
        DoublePos bastionCoords = {{-96, -96}, {96, 96}};
        int structureIndex = 0;
        result = structureChecker(lower48, STRUCTS, 0, MC, bastionCoords, data, numberOfStructs, result, &bastionCoordinates);        
        if (result == 1) {
            continue;
            // Didn't find a candidate and continuing to the next structure seed
        } else {
            DoublePos fortressCoords = {{-96 + bastionCoordinates.x, -96 + bastionCoordinates.z}, {96 + bastionCoordinates.x, 96 + bastionCoordinates.z}};
            int structureIndex = 1;
            result = structureChecker(lower48, STRUCTS, 1, MC, fortressCoords, data, numberOfStructs, result, &fortressCoordinates);        
            if (result == 1) {
                continue;
                // Didn't find a candidate and continuing to the next structure seed
            } else {
                for (uint64_t upper16 = 0; upper16 < UPPER_BITS_TO_CHECK; ++upper16) {
                    uint64_t seed = lower48 | (upper16 << 48);
                    for (int i = 0; i < numberOfStructs; ++i) {
                        data[i].positionsCount = 0;
                        if (g.seed != seed || g.dim != data[i].dimension) applySeed(&g, data[i].dimension, seed);
                        ListNode* candidateNode = data[i].candidatesHead;
                        while (candidateNode != NULL) {
                            if (!isViableStructurePos(STRUCTS[i], &g, candidateNode->position.x, candidateNode->position.z, 0) ||
                                !isViableStructureTerrain(STRUCTS[i], &g, candidateNode->position.x, candidateNode->position.z))
                                continue;
                            addPosition(&data[i].positionsHead, candidateNode->position);
                            ++data[i].positionsCount;
                            candidateNode = candidateNode->next;
                        }
                        if (!data[i].positionsCount) continue;
                    }
                    biome = getBiomeAt(&g, 1, bastionCoordinates.x, 64, bastionCoordinates.z);
                    if (biome == basalt_deltas) {
                        goto nextStructureSeed;
                    }
                    fprintf(fp, "%" PRId64 "\n", seed);
                    goto nextStructureSeed;
                }
            }
        }
        nextStructureSeed:

        for (int i = 0; i < numberOfStructs; ++i) {
            ListNode* candidateNode = data[i].candidatesHead;
            while (candidateNode != NULL) {
                ListNode* temp = candidateNode;
                candidateNode = candidateNode->next;
                free(temp);
            }
            data[i].candidatesHead = NULL;

            ListNode* positionNode = data[i].positionsHead;
            while (positionNode != NULL) {
                ListNode* temp = positionNode;
                positionNode = positionNode->next;
                free(temp);
            }
            data[i].positionsHead = NULL;
        }

        continue;
    }

    fprintf(fp, "Done\n");
    fclose(fp);

    for (int i = 0; i < numberOfStructs; ++i) {
        ListNode* candidateNode = data[i].candidatesHead;
        while (candidateNode != NULL) {
            ListNode* temp = candidateNode;
            candidateNode = candidateNode->next;
            free(temp);
        }
        data[i].candidatesHead = NULL;

        ListNode* positionNode = data[i].positionsHead;
        while (positionNode != NULL) {
            ListNode* temp = positionNode;
            positionNode = positionNode->next;
            free(temp);
        }
        data[i].positionsHead = NULL;
    }

    return 0;
}
