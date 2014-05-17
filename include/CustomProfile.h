#ifndef CUSTOMPROFILE_H
#define CUSTOMPROFILE_H

#include <Terrain/OgreTerrainMaterialGenerator.h>

namespace Ogre {

class CustomProfile : Ogre::TerrainMaterialGenerator::Profile {
public:
    CustomProfile(TerrainMaterialGenerator* parent, const String& name,
            const String& desc);
    ~CustomProfile ();

    virtual bool isVertexCompressionSupported() const;		
    /// Generate / reuse a material for the terrain
    virtual MaterialPtr generate(const Terrain* terrain);
    /// Generate / reuse a material for the terrain
    virtual MaterialPtr generateForCompositeMap(const Terrain* terrain);
    /// Whether to support a light map over the terrain in the shader, if it's present (default true)
    virtual void setLightmapEnabled(bool enabled);
    /// Get the number of layers supported
    virtual uint8 getMaxLayers(const Terrain* terrain) const;

    virtual void updateParams(const MaterialPtr& mat, const Terrain* terrain);
    /// Update params for a terrain
    virtual void updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain);

    /// Request the options needed from the terrain
    virtual void requestOptions(Terrain* terrain);

private:

};

}
#endif //CUSTOMPROFILE_H
