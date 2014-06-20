#include "../include/CustomTerrainGenerator.h"
#include <iostream>
void CustomTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    std::cout<<"\nx: "<<x<<"\ny: "<<y<<"\n";
    int num = (x)+y*4;
    Image img;
    Ogre::String name = "small-";
    name += Ogre::StringConverter::toString(num)+".png";
    std::cout<<"\nname: "<<name.c_str()<<std::endl;
    img.load(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //if (x % 2)
        //img.flipAroundY();
    //if (y % 2)
        //img.flipAroundX();
    terrainGroup->defineTerrain(x, y, &img);
}
