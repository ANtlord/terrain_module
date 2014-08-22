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


void setTesxturesToPass(Pass * pass, const std::string * textureNames,
        int COUNT)
{
    for (int i = 0; i < COUNT; ++i) {
        TexturePtr tex = TextureManager::getSingleton().load(
                textureNames[i], ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );

        // Create indentificator of unit state for checking.
        std::string UNIT_STATE_NAME = textureNames[i].substr(0,
                textureNames[i].size()-4);
        if (pass->getTextureUnitState(UNIT_STATE_NAME) == 0) {
            pass->createTextureUnitState(UNIT_STATE_NAME)->setTexture(tex);

			//TextureUnitState* tu = pass->createTextureUnitState();
			//tu->setTextureName(terrain->getCompositeMap()->getName());
			//tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
        }
    }
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
            Material::LodValueList lodValues;
            lodValues.push_back(TerrainGlobalOptions::getSingleton().getCompositeMapDistance());
            mat->setLodLevels(lodValues);

            Pass * pass = mat->getTechnique(0)->getPass(0); // Technique for high LOD.
            Technique * tech = 0;   // Technique for low LOD.
            if (mat->getNumTechniques() == 1) {
                tech = mat->createTechnique();
                tech->setLodIndex(1);
            }
            else tech = mat->getTechnique(1);

            Pass * pass2 = 0;
            if (tech != 0) {
                if (tech->getNumPasses() == 0) {
                    pass2 = tech->createPass();
                    //TextureUnitState* tu = pass2->createTextureUnitState();
                    //tu->setTextureName(terrain->getCompositeMap()->getName());
                    //tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
                }
                else pass2 = tech->getPass(0);
            }
            else {
                std::cout << "pass hasn't been created" << std::endl;
            }

            const std::string VERTEX_SHADER_NAME = "tvprog";
            const std::string FRAGMENT_SHADER_NAME = "tfprog";
            const std::string VERTEX_SHADER_ENTRY_NAME = "qwe";
            const std::string FRAGMENT_SHADER_ENTRY_NAME = "asd";
            const std::string WORLDVIEWPROJ_MATRIX_NAME = "worldViewMatrix";
            const std::string PASS_TEXTURE_NAMES[1] = {"grass_mini.jpg"};
            const std::string PASS2_TEXTURE_NAMES[1] = {"tusk.jpg"};

            HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
            HighLevelGpuProgramPtr vprog = initShader(VERTEX_SHADER_NAME,
                    GPT_VERTEX_PROGRAM);
            HighLevelGpuProgramPtr fprog = initShader(FRAGMENT_SHADER_NAME,
                    GPT_FRAGMENT_PROGRAM);

            setTesxturesToPass(pass, PASS_TEXTURE_NAMES, 1);
            setTesxturesToPass(pass2, PASS2_TEXTURE_NAMES, 1);

            vprog->setParameter("entry_point", VERTEX_SHADER_ENTRY_NAME);
            fprog->setParameter("entry_point", FRAGMENT_SHADER_ENTRY_NAME);
            vprog->setParameter("profiles", "vs_4_0 arbvp1");
            fprog->setParameter("profiles", "ps_4_0 arbfp1");

            std::stringstream ss;
            ss<<"void "<<VERTEX_SHADER_ENTRY_NAME<<"(float4 position : POSITION,"
                "out float4 oPosition : POSITION, out float4 texCoord : TEXCOORD0,"
                "uniform float4x4 "<<WORLDVIEWPROJ_MATRIX_NAME<<")"
                "{"
                    "oPosition =  mul("<<WORLDVIEWPROJ_MATRIX_NAME<<", position);"
                    "texCoord.x = position.x/513.;"
                    "texCoord.y = position.z/513.;"
                "}";
            vprog->setSource(ss.str());
            ss.clear();
            ss<<"void "<<FRAGMENT_SHADER_ENTRY_NAME<<
                "(in float4 texCoord : TEXCOORD0, out float4 color: COLOR,"
                "uniform sampler2D tex1 : register(s0)"
                //", uniform sampler2D tex2 : register(s1)"
                ")"
                "{"
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
