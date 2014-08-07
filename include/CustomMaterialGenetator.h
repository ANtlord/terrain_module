#ifndef CUSTOMMATERIALGENETATOR_H
#define CUSTOMMATERIALGENETATOR_H

#include <OGRE/Terrain/OgreTerrainMaterialGenerator.h>

namespace Ogre {

class CustomMaterialGenetator : public TerrainMaterialGenerator {
public:
    CustomMaterialGenetator();
    ~CustomMaterialGenetator();
    class CustomProfile : public Ogre::TerrainMaterialGenerator::Profile {
    public:
        CustomProfile(TerrainMaterialGenerator* parent, const String& name,
                const String& desc);
        ~CustomProfile ();

        // Override
        virtual bool isVertexCompressionSupported() const;		
        /// Generate / reuse a material for the terrain
        // Override
        virtual MaterialPtr generate(const Terrain* terrain);
        /// Generate / reuse a material for the terrain
        // Override
        virtual MaterialPtr generateForCompositeMap(const Terrain* terrain);
        /// Whether to support a light map over the terrain in the shader, if it's present (default true)
        // Override
        virtual void setLightmapEnabled(bool enabled);
        /// Get the number of layers supported
        // Override
        virtual uint8 getMaxLayers(const Terrain* terrain) const;

        // Override
        virtual void updateParams(const MaterialPtr& mat, const Terrain* terrain);
        /// Update params for a terrain
        // Override
        virtual void updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain);

        /// Request the options needed from the terrain
        // Override
        virtual void requestOptions(Terrain* terrain);

    private:
        //MaterialPtr _material;

    };
protected:

private:
};


}
#endif //CUSTOMMATERIALGENETATOR_H
