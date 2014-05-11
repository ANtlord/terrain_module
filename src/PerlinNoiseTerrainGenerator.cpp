#include "../include/PerlinNoiseTerrainGenerator.h"
#include <iostream>
void PerlinNoiseTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    Ogre::String filename = terrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(
                terrainGroup->getResourceGroup(), filename)) {
        terrainGroup->defineTerrain(x, y);
    }
    else {
        Ogre::Image img;
        img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        if (x % 2) img.flipAroundY();
        if (y % 2) img.flipAroundX();
        terrainGroup->defineTerrain(x, y, &img);
    }

    Ogre::TerrainGroup::TerrainIterator ti = terrainGroup->getTerrainIterator();
    while (ti.hasMoreElements()) {
        Ogre::Terrain* t = ti.getNext()->instance;
        if (t != NULL) {
            initBlendMaps(t);
        }
    }
}

void PerlinNoiseTerrainGenerator::initBlendMaps(Ogre::Terrain* terrain)
{
    std::cout<<"initBlendMaps\n";
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y) {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x) {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}
