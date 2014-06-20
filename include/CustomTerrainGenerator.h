#ifndef CUSTOMTERRAINGENERATOR_H
#define CUSTOMTERRAINGENERATOR_H

#include <OGRE/Terrain/OgreTerrainPagedWorldSection.h>

#define B 0x100
using namespace Ogre;

///Use PerlinNoise algorithm to generate terrain height map
class CustomTerrainGenerator : public TerrainPagedWorldSection::TerrainDefiner
{
    public:
        virtual void define(TerrainGroup* terrainGroup, long x, long y);
};
#endif //CUSTOMTERRAINGENERATOR_H
