#include "../include/PerlinNoiseTerrainGenerator.h"
void PerlinNoiseTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    Image img;
    img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (x % 2)
        img.flipAroundY();
    if (y % 2)
        img.flipAroundX();
    terrainGroup->defineTerrain(x, y, &img);
}
