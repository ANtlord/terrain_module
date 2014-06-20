#include "../include/CustomProfile.h"

using namespace Ogre;
#include <OgreTerrain.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>

CustomMaterialGenetator::CustomMaterialGenetator()
{
    // define the layers
    // We expect terrain textures to have no alpha, so we use the alpha channel
    // in the albedo texture to store specular reflection
    // similarly we double-up the normal and height (for parallax)
    mLayerDecl.samplers.push_back(TerrainLayerSampler("albedo_specular", PF_BYTE_RGBA));
    mLayerDecl.samplers.push_back(TerrainLayerSampler("normal_height", PF_BYTE_RGBA));
    
    mLayerDecl.elements.push_back(
        TerrainLayerSamplerElement(0, TLSS_ALBEDO, 0, 3));
    mLayerDecl.elements.push_back(
        TerrainLayerSamplerElement(0, TLSS_SPECULAR, 3, 1));
    mLayerDecl.elements.push_back(
        TerrainLayerSamplerElement(1, TLSS_NORMAL, 0, 3));
    mLayerDecl.elements.push_back(
        TerrainLayerSamplerElement(1, TLSS_HEIGHT, 3, 1));

    mProfiles.push_back(OGRE_NEW CustomProfile(this, "qwe",
                "Profile for rendering on Shader Model 2 capable cards"));
    // TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
    setActiveProfile("qwe");
}

CustomMaterialGenetator::~CustomMaterialGenetator()
{

}

CustomMaterialGenetator::CustomProfile::CustomProfile(
        TerrainMaterialGenerator* parent, const String& name, const String& desc
        ) : TerrainMaterialGenerator::Profile(parent, name,
            desc)
{
    _material = MaterialManager::getSingleton().getByName("Study/HeightBasedMaterial");
}

CustomMaterialGenetator::CustomProfile::~CustomProfile()
{

}

bool CustomMaterialGenetator::CustomProfile::isVertexCompressionSupported() const
{
    return false;
}		
/// Generate / reuse a material for the terrain
MaterialPtr CustomMaterialGenetator::CustomProfile::generate(const Terrain* terrain)
{
    //const Ogre::String matName = terrain->getMaterialName();        
    
    //Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
    //if (!mat.isNull())
        //MaterialManager::getSingleton().remove(matName);
     
    // Set Ogre material 
    //mat = Ogre::MaterialManager::getSingleton().getByName(((Ogre::TerrainMaterialGenerator*)getParent())->mMaterialName);

    // Get default pass
//    Ogre::Pass *p = mat->getTechnique(0)->getPass(0);
//    std::cout<<"CustomProfile_DevLog::generate\n";

    // Add terrain's global normalmap to renderpass so the fragment program can find it.
    //Ogre::TextureUnitState *tu = p->createTextureUnitState(matName+"/nm");

    //Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
    //tu->setTexturePtr(nmtx);
    return _material;
}
/// Generate / reuse a material for the terrain
MaterialPtr CustomMaterialGenetator::CustomProfile::generateForCompositeMap(
        const Terrain* terrain)
{
    return terrain->_getCompositeMapMaterial();
}
/// Whether to support a light map over the terrain in the shader, if it's present (default true)
void CustomMaterialGenetator::CustomProfile::setLightmapEnabled(bool enabled)
{
}
/// Get the number of layers supported
Ogre::uint8 CustomMaterialGenetator::CustomProfile::getMaxLayers(const Terrain* terrain) const
{
    return 0;
}

void CustomMaterialGenetator::CustomProfile::updateParams(const MaterialPtr& mat, const Terrain* terrain)
{
}
/// Update params for a terrain
void CustomMaterialGenetator::CustomProfile::updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain)
{
}

/// Request the options needed from the terrain
void CustomMaterialGenetator::CustomProfile::requestOptions(Terrain* terrain)
{
    terrain->_setMorphRequired(true);
    terrain->_setNormalMapRequired(true); // enable global normal map
    terrain->_setLightMapRequired(false);
    terrain->_setCompositeMapRequired(false);
}
