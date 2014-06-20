#include "../include/CustomTerrainGenerator.h"
#include <iostream>
void CustomTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    const int MAX_VALUE = 1;
    Image img;
    Ogre::String name = "small-";
    name += Ogre::StringConverter::toString( MAX_VALUE - (x+y*4) )+".png";
    //std::cout<<"\nx: "<<x<<"\ny: "<<y<<std::endl;
    //std::cout<<"\nname: "<<name.c_str()<<std::endl;
    img.load(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //img.load("heightmap.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //if (x % 2)
        //img.flipAroundY();
    //if (y % 2)
        //img.flipAroundX();
    terrainGroup->defineTerrain(x, y, &img);
}
