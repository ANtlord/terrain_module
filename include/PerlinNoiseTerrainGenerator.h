#pragma once

#include <OGRE/Terrain/OgreTerrainPagedWorldSection.h>

#define B 0x100
using namespace Ogre;

///Use PerlinNoise algorithm to generate terrain height map
class PerlinNoiseTerrainGenerator : public TerrainPagedWorldSection::TerrainDefiner
{
    public:
        virtual void define(TerrainGroup* terrainGroup, long x, long y);
        void initBlendMaps(Ogre::Terrain* terrain);
};
