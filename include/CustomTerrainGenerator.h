#ifndef CUSTOMTERRAINGENERATOR_H
#define CUSTOMTERRAINGENERATOR_H

#include <Terrain/OgreTerrainPagedWorldSection.h>

#define B 0x100
using namespace Ogre;

///Use PerlinNoise algorithm to generate terrain height map
class CustomTerrainGenerator : public TerrainPagedWorldSection::TerrainDefiner
{
    public:
        virtual void define(TerrainGroup* terrainGroup, long x, long y);
        static const uint8 ENDLESS_PAGE_MIN_X;
        static const uint8 ENDLESS_PAGE_MIN_Y;
        static const uint8 ENDLESS_PAGE_MAX_X;
        static const uint8 ENDLESS_PAGE_MAX_Y;
    
};
#endif //CUSTOMTERRAINGENERATOR_H
