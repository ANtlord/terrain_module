/*
-----------------------------------------------------------------------------
Filename:    TerrainApplication.h
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
#ifndef __TerrainApplication_h_
#define __TerrainApplication_h_

#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainPagedWorldSection.h>
#include <Terrain/OgreTerrainPaging.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "BaseApplication.h"
#include "CustomTerrainGenerator.h"
#include <OgreVector3.h>

class TerrainApplication : public BaseApplication
{
public:
    TerrainApplication(void);
    virtual ~TerrainApplication(void);

protected:
    virtual void createScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual void createFrameListener(void);
    virtual void destroyScene(void);

private:
    Ogre::TerrainPaging* mTerrainPaging;
    Ogre::TerrainPagedWorldSection* mTerrainPagedWorldSection;
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    Ogre::PageManager* mPageManager;
    Ogre::PagedWorld* mPagedWorld;

	CustomTerrainGenerator* mCustomTerrainGenerator;
	bool mLodStatus;
	bool mAutoLod;
    bool mTerrainsImported;
    Ogre::Vector3 mTerrainPos;

	/// This class just pretends to provide procedural page content to avoid page loading
	class DummyPageProvider : public PageProvider
	{
	public:
		bool prepareProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool loadProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool unloadProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool unprepareProceduralPage(Page* page, PagedWorldSection* section) { return true; }
	};
	DummyPageProvider mDummyPageProvider;


 
    //void defineTerrain(long x, long y);
    //void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);
    OgreBites::Label* mInfoLabel;
};

#endif // #ifndef __TerrainApplication_h_
