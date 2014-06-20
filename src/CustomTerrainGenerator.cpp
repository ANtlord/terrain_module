#include "../include/CustomTerrainGenerator.h"
#include <iostream>
#include <string.h>
#include <sstream>
void CustomTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    const int MAX_VALUE = 1;
    Image img;
    std::stringstream qwe;
    int c = (x+y*4);
    qwe<<"nsmall-"<<c<<".png";
    //name += Ogre::StringConverter::toString(c)+".png";
    //std::cout<<"\nx: "<<x<<"\ny: "<<y<<std::endl;
    //std::cout<<"\nname: "<<name.c_str()<<std::endl;
    img.load(qwe.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //img.load("heightmap.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //if (x % 2)
        //img.flipAroundY();
    //if (y % 2)
        //img.flipAroundX();
    terrainGroup->defineTerrain(x, y, &img);
}
