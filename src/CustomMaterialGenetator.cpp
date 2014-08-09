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
}

CustomMaterialGenetator::CustomProfile::~CustomProfile()
{

}

bool CustomMaterialGenetator::CustomProfile::isVertexCompressionSupported() const
{
    return false;
}
HighLevelGpuProgramPtr initShader(const std::string name, GpuProgramType gptype)
{
    HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
    HighLevelGpuProgramPtr vprog = mgr.getByName(name);
    if (vprog.isNull()) {
        vprog = mgr.createProgram(name,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "cg", gptype);
    }
    else {
        vprog->unload();
    }
    return vprog;
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
            const std::string VERTEX_SHADER_NAME = "tvprog";
            const std::string FRAGMENT_SHADER_NAME = "tfprog";

            HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
            HighLevelGpuProgramPtr vprog = initShader(VERTEX_SHADER_NAME,
                    GPT_VERTEX_PROGRAM);
            HighLevelGpuProgramPtr fprog = initShader(FRAGMENT_SHADER_NAME,
                    GPT_FRAGMENT_PROGRAM);
            
            //std::string textureNames[3] = {"blending_map.png", "tusk.jpg", "GreenSkin.jpg"};
            std::string textureNames[1] = {"grass_mini.jpg"};
            for (std::string item : textureNames) {
                TexturePtr tex = TextureManager::getSingleton().load(
                        item, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
                    );
                
                // Create indentificator of unit state for checking.
                std::string UNIT_STATE_NAME = item.substr(0, item.size()-4);
                if (pass->getTextureUnitState(UNIT_STATE_NAME) == 0) {
                    pass->createTextureUnitState(UNIT_STATE_NAME)->setTexture(tex);
                }
            }

            const std::string VERTEX_SHADER_ENTRY_NAME = "qwe";
            const std::string FRAGMENT_SHADER_ENTRY_NAME = "asd";

            vprog->setParameter("entry_point", VERTEX_SHADER_ENTRY_NAME);
            fprog->setParameter("entry_point", FRAGMENT_SHADER_ENTRY_NAME);
            vprog->setParameter("profiles", "ps_1_1 arbvp1");
            fprog->setParameter("profiles", "ps_1_1 arbfp1");

            const std::string WORLDVIEWPROJ_MATRIX_NAME = "worldViewMatrix";
            //strStream <<
            std::stringstream ss;
            ss<<"void "<<VERTEX_SHADER_ENTRY_NAME<<"(float4 position : POSITION,"
                "out float4 oPosition : POSITION, out float4 texCoord : TEXCOORD0,"
                "uniform float4x4 "<<WORLDVIEWPROJ_MATRIX_NAME<<")"
                //" out float4 texCoord : TEXCOORD0)"
                "{"
                    "oPosition =  mul("<<WORLDVIEWPROJ_MATRIX_NAME<<", position);"
                    //"texCoord = position;"
                    "texCoord.x = position.x/513;"
                    "texCoord.y = position.z/513;"
                "}";
            vprog->setSource(ss.str());
            ss.clear();
            ss<<"void "<<FRAGMENT_SHADER_ENTRY_NAME<<
                "(in float4 texCoord : TEXCOORD0, out float4 color: COLOR,"
                "uniform sampler2D tex1 : register(s0))"
                //;
                "{"
                    //"color = float4(1,1,1,1); "
                    //"float value = texCoord.y/300;"
                    //"color = float4(value, value, value, 1);"
                    "color = tex2D(tex1, float2(texCoord.x, texCoord.y));"
                "}";
            fprog->setSource(ss.str());
            vprog->load();
            fprog->load();

            GpuProgramParametersSharedPtr params = vprog->getDefaultParameters();
            params->setNamedAutoConstant(WORLDVIEWPROJ_MATRIX_NAME,
                    GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

            pass->setVertexProgram(VERTEX_SHADER_NAME);
            pass->setFragmentProgram(FRAGMENT_SHADER_NAME);
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
