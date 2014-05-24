#include "../include/CustomProfile.h"

using namespace Ogre;
#include <OgreTerrain.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>

CustomProfile::CustomProfile(TerrainMaterialGenerator* parent, const String& name,
        const String& desc) : TerrainMaterialGenerator::Profile(parent, name,
            desc)
{

}

CustomProfile::~CustomProfile ()
{

}

bool CustomProfile::isVertexCompressionSupported() const
{
    return false;
}		
/// Generate / reuse a material for the terrain
MaterialPtr CustomProfile::generate(const Terrain* terrain)
{
    const Ogre::String matName = terrain->getMaterialName();        
    
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
    if (!mat.isNull())
        MaterialManager::getSingleton().remove(matName);
     
    // Set Ogre material 
    mat = MaterialManager::getSingleton().getByName("Ogre/Skin");
    //mat = Ogre::MaterialManager::getSingleton().getByName(((Ogre::TerrainMaterialGenerator*)getParent())->mMaterialName);

    // Get default pass
    Ogre::Pass *p = mat->getTechnique(0)->getPass(0);

    // Add terrain's global normalmap to renderpass so the fragment program can find it.
    Ogre::TextureUnitState *tu = p->createTextureUnitState(matName+"/nm");

    Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
    tu->setTexturePtr(nmtx);
    
    return mat;
}
/// Generate / reuse a material for the terrain
MaterialPtr CustomProfile::generateForCompositeMap(const Terrain* terrain)
{
    return terrain->_getCompositeMapMaterial();
}
/// Whether to support a light map over the terrain in the shader, if it's present (default true)
void CustomProfile::setLightmapEnabled(bool enabled)
{
}
/// Get the number of layers supported
Ogre::uint8 CustomProfile::getMaxLayers(const Terrain* terrain) const
{
    return 0;
}

void CustomProfile::updateParams(const MaterialPtr& mat, const Terrain* terrain)
{
}
/// Update params for a terrain
void CustomProfile::updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain)
{
}

/// Request the options needed from the terrain
void CustomProfile::requestOptions(Terrain* terrain)
{
    terrain->_setMorphRequired(true);
    terrain->_setNormalMapRequired(true); // enable global normal map
    terrain->_setLightMapRequired(false);
    terrain->_setCompositeMapRequired(false);
}


