/*
-----------------------------------------------------------------------------
Filename:    TerrainApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "../include/TerrainApplication.h"
#include "../include/CustomProfile.h"

#define TERRAIN_WORLD_SIZE 6000.0f
#define TERRAIN_SIZE 513
#define HOLD_LOD_DISTANCE 3000.0
#include <iostream>
//-------------------------------------------------------------------------------------
TerrainApplication::TerrainApplication(void) : BaseApplication()
{
	mTerrainPos = Vector3::ZERO;
	mTerrainsImported=false;
}
//-------------------------------------------------------------------------------------
TerrainApplication::~TerrainApplication(void)
{
}

//-------------------------------------------------------------------------------------
void TerrainApplication::createScene(void)
{
    mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
    mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(50000);
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z,
            TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"),
            Ogre::String("dat"));
    //mTerrainGroup->setOrigin(mTerrainPos);
	mTerrainGroup->setOrigin(mTerrainPos + Vector3(TERRAIN_WORLD_SIZE/2, 0,
                TERRAIN_WORLD_SIZE/2));
    mTerrainGroup->setAutoUpdateLod( TerrainAutoUpdateLodFactory::getAutoUpdateLod(BY_DISTANCE) );

    configureTerrainDefaults(light);

    // Paging setup
    mPageManager = OGRE_NEW PageManager();
    // Since we're not loading any pages from .page files, we need a way just 
    // to say we've loaded them without them actually being loaded
    mPageManager->setPageProvider(&mDummyPageProvider);
    mPageManager->addCamera(mCamera);
    mPageManager->setDebugDisplayLevel(0);
    mTerrainPaging = OGRE_NEW TerrainPaging(mPageManager);
    mPagedWorld = mPageManager->createWorld();
    //mTerrainPagedWorldSection = mTerrainPaging->createWorldSection(mPagedWorld,
            //mTerrainGroup, 400, 500, ENDLESS_PAGE_MIN_X, ENDLESS_PAGE_MIN_Y,
            //ENDLESS_PAGE_MAX_X, ENDLESS_PAGE_MAX_Y);
    mTerrainPagedWorldSection = mTerrainPaging->createWorldSection(mPagedWorld,
            mTerrainGroup, 400, 500,
            CustomTerrainGenerator::ENDLESS_PAGE_MIN_X,
            CustomTerrainGenerator::ENDLESS_PAGE_MIN_Y,
            CustomTerrainGenerator::ENDLESS_PAGE_MAX_X,
            CustomTerrainGenerator::ENDLESS_PAGE_MAX_Y
            );

    mCustomTerrainGenerator = OGRE_NEW CustomTerrainGenerator;
    mTerrainPagedWorldSection->setDefiner( mCustomTerrainGenerator );

    mTerrainGroup->freeTemporaryResources();
}

//-------------------------------------------------------------------------------------
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

//-------------------------------------------------------------------------------------
void TerrainApplication::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(
                mTerrainGroup->getResourceGroup(), filename)) {
        mTerrainGroup->defineTerrain(x, y);
    }
    else {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

//-------------------------------------------------------------------------------------
void TerrainApplication::initBlendMaps(Ogre::Terrain* terrain)
{
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
//-------------------------------------------------------------------------------------
void TerrainApplication::configureTerrainDefaults(Ogre::Light* light)
{
    mTerrainGlobals->setMaxPixelError(8);
    mTerrainGlobals->setCompositeMapDistance(3000);
    
    // Setup material generator.
    Ogre::TerrainMaterialGeneratorPtr generator;
    generator.bind(new Ogre::CustomMaterialGenetator());
    generator->setLightmapEnabled(false);
    mTerrainGlobals->setDefaultMaterialGenerator(generator);
    
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());

    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = TERRAIN_SIZE;
    defaultimp.worldSize = TERRAIN_WORLD_SIZE;
    defaultimp.inputScale = 600; // due terrain.png is 8 bpp
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

	// textures
    //defaultimp.layerList.resize(3);
    //defaultimp.layerList[0].worldSize = 100;
    //defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    //defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    //defaultimp.layerList[1].worldSize = 30;
    //defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    //defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    //defaultimp.layerList[2].worldSize = 200;
    //defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    //defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void TerrainApplication::destroyScene(void)
{
    if(mTerrainPaging) {
        OGRE_DELETE mTerrainPaging;
        mPageManager->destroyWorld( mPagedWorld );
        OGRE_DELETE mPageManager;
    }
    //OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}

void TerrainApplication::createFrameListener(void)
{
    BaseApplication::createFrameListener();
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}

//-------------------------------------------------------------------------------------
bool TerrainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool res = BaseApplication::frameRenderingQueued(evt);

    if (mTerrainGroup->isDerivedDataUpdateInProgress()) {
        mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
        mInfoLabel->show();
        mInfoLabel->setCaption("Building terrain...");
    }
    else {
        mTrayMgr->removeWidgetFromTray(mInfoLabel);
        mInfoLabel->hide();
    }

    mTerrainGroup->setAutoUpdateLod(TerrainAutoUpdateLodFactory::getAutoUpdateLod(BY_DISTANCE));
    return res;
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TerrainApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
