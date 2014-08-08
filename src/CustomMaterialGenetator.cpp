#include "../include/CustomMaterialGenetator.h"

using namespace Ogre;

#include <OgreTerrain.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreGpuProgramManager.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include "../include/CustomTerrainGenerator.h"
#include <math.h>
#include <OgreTextureManager.h>
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
    //_material = MaterialManager::getSingleton().getByName("Study/HeightBasedMaterial");
}

CustomMaterialGenetator::CustomProfile::~CustomProfile()
{

}

bool CustomMaterialGenetator::CustomProfile::isVertexCompressionSupported() const
{
    return false;
}		
/// Generate / reuse a material for the terrain.
MaterialPtr CustomMaterialGenetator::CustomProfile::generate(const Terrain* terrain)
{
    MaterialPtr mat = terrain->_getMaterial();
    if (mat.isNull()) {
        MaterialManager& matMgr = MaterialManager::getSingleton();
        const String& matName = terrain->getMaterialName();
        mat = matMgr.getByName(matName);
        if (mat.isNull()) {
            //const int X = terrain->getPosition().x / CustomTerrainGenerator::TERRAIN_WORLD_SIZE;
            //const int Y = terrain->getPosition().z / CustomTerrainGenerator::TERRAIN_WORLD_SIZE;
            //std::cout << "X: "<<X <<"Y: "<<Y << std::endl;


            // Program must not create material and mustn't get it from material script.
            mat = matMgr.create("qweasd_material", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            //Material::LodValueList lodValues;
            //lodValues.push_back(TerrainGlobalOptions::getSingleton().getCompositeMapDistance());
            //mat->setLodLevels(lodValues);

            Pass * pass = mat->getTechnique(0)->getPass(0); // Technique for high LOD.
            //Technique * tech = 0;   // Technique for low LOD.
            //if (mat->getNumTechniques() == 1) {
                //tech = mat->createTechnique();
                //tech->setLodIndex(1);
            //}
            //else tech = mat->getTechnique(1);
            //Pass * pass2 = 0;
            //if (tech != 0) {
                //if (tech->getNumPasses() == 0) pass2 = tech->createPass();
                //else pass2 = tech->getPass(0);
            //}
            //else {
                //std::cout << "pass hasn't been created" << std::endl;
            //}

            HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
            HighLevelGpuProgramPtr vprog = mgr.createProgram("tvprog",
                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "cg",
                    GPT_VERTEX_PROGRAM);
            HighLevelGpuProgramPtr fprog = mgr.createProgram("tfprog",
                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "cg",
                    GPT_FRAGMENT_PROGRAM);
            
            //std::string textureNames[3] = {"blending_map.png", "tusk.jpg", "GreenSkin.jpg"};
            //for (std::string item : textureNames) {
                //TexturePtr tex = TextureManager::getSingleton().load(
                        //item, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
                    //);
                
                //// Create indentificator of unit state for checking.
                //std::string UNIT_STATE_NAME = item.substr(0, item.size()-4);
                //if (pass->getTextureUnitState(UNIT_STATE_NAME) == 0) {
                    //pass->createTextureUnitState(UNIT_STATE_NAME)->setTexture(tex);
                //}
            //}
            //std::stringstream strStream;
            vprog->setParameter("entry_point", "qwe");
            fprog->setParameter("entry_point", "asd");
            vprog->setParameter("profiles", "ps_1_1 arbvp1");
            fprog->setParameter("profiles", "ps_1_1 arbfp1");
            //vprog->setSource(" void qwe(float4 position : POSITION, out float4 oPosition : POSITION, out float4 dum : TEXCOORD0, uniform float4x4 worldViewMatrix) { oPosition =  mul(worldViewMatrix, position); dum = position; } ");

            //strStream <<
            //fprog->setSource( "void asd(in float4 oPosition : TEXCOORD0, out float4 color: COLOR){ float value = oPosition.y/300; color = float4(value, value, value, 1); }");
            vprog->setSource(" void qwe(float4 position : POSITION, out float4 oPosition : POSITION, out float4 dum : TEXCOORD0, uniform float4x4 worldViewMatrix) { oPosition =  mul(worldViewMatrix, position); dum = position; } ");
            fprog->setSource(" void asd(in float4 oPosition : TEXCOORD0, out float4 color: COLOR) { float value = oPosition.y/300; color = float4(value, value, value, 1); } ");
            vprog->load();
            fprog->load();

            GpuProgramParametersSharedPtr params = vprog->getDefaultParameters();
            params->setIgnoreMissingParams(true);
            params->setNamedAutoConstant("worldMatrix", GpuProgramParameters::ACT_WORLD_MATRIX);
            params->setNamedAutoConstant("viewMatrix", GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
            params->setNamedAutoConstant("viewProjMatrix", GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
            params->setNamedAutoConstant("lodMorph", GpuProgramParameters::ACT_CUSTOM, 
                Terrain::LOD_MORPH_CUSTOM_PARAM);
            params->setNamedAutoConstant("fogParams", GpuProgramParameters::ACT_FOG_PARAMS);

            //if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP)
            //{
                //Matrix4 posIndexToObjectSpace;
                //terrain->getPointTransform(&posIndexToObjectSpace);
                //params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
            //}

            params = fprog->getDefaultParameters();
            params->setIgnoreMissingParams(true);
            params->setNamedAutoConstant("worldMatrix", GpuProgramParameters::ACT_WORLD_MATRIX);
            params->setNamedAutoConstant("viewMatrix", GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
            params->setNamedAutoConstant("viewProjMatrix", GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
            params->setNamedAutoConstant("lodMorph", GpuProgramParameters::ACT_CUSTOM, 
                Terrain::LOD_MORPH_CUSTOM_PARAM);
            params->setNamedAutoConstant("fogParams", GpuProgramParameters::ACT_FOG_PARAMS);

            //if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP)
            //{
                //Matrix4 posIndexToObjectSpace;
                //terrain->getPointTransform(&posIndexToObjectSpace);
                //params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
            //}



            pass->setVertexProgram(vprog->getName());
            pass->setFragmentProgram(fprog->getName());
        }
    }
    return mat;
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
