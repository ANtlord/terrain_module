#include "../include/CustomTerrainGenerator.h"
#include <iostream>
#include <string.h>
#include <sstream>

const uint8 CustomTerrainGenerator::ENDLESS_PAGE_MIN_X = 0;
const uint8 CustomTerrainGenerator::ENDLESS_PAGE_MIN_Y = 0;
const uint8 CustomTerrainGenerator::ENDLESS_PAGE_MAX_X = 3;
const uint8 CustomTerrainGenerator::ENDLESS_PAGE_MAX_Y = 3;

void CustomTerrainGenerator::define(TerrainGroup* terrainGroup, long x, long y)
{
    //if (x < ENDLESS_PAGE_MIN_X) x = ENDLESS_PAGE_MIN_X;
    //if (x > ENDLESS_PAGE_MAX_X) x = ENDLESS_PAGE_MAX_X;
    //if (y < ENDLESS_PAGE_MIN_Y) y = ENDLESS_PAGE_MIN_Y;
    //if (y > ENDLESS_PAGE_MAX_Y) y = ENDLESS_PAGE_MAX_Y;
    if (x >= ENDLESS_PAGE_MIN_X && x <= ENDLESS_PAGE_MAX_X
            && y >= ENDLESS_PAGE_MIN_Y && y <= ENDLESS_PAGE_MAX_Y)
    {
        Image img;
        std::stringstream qwe;
        int c = (x+y*4);
        qwe<<"nsmall-"<<c<<".png";
        //img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        img.load(qwe.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        terrainGroup->defineTerrain(x, y, &img);
    }
    else {
        terrainGroup->defineTerrain(x, y);
    }

    //name += Ogre::StringConverter::toString(c)+".png";
    //std::cout<<"\nx: "<<x<<"\ny: "<<y<<std::endl;
    //std::cout<<"\nname: "<<name.c_str()<<std::endl;
    //img.load(qwe.str(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //if (x % 2)
        //img.flipAroundY();
    //if (y % 2)
        //img.flipAroundX();
}
