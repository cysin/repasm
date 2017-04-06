
#pragma once

#include <string>
#include <memory>
#include <rxmaptools/RXMinimap.h>
//Wrapper for the minimap generation

class RXSCMpqReader;
class RXMinimap;
class RXMap;
class RXReplay;
class RXRenderableMap;
class RXBitmap;
class RXCache;

class RXMinimapGenerator
{
public:
    RXMinimapGenerator(void);
    ~RXMinimapGenerator(void);

    RXBitmap * generateRatio      (const std::wstring &path, int ratio, bool highQuality, int render /* flags from RXMinimap::enum*/); 
    RXBitmap * generateFixedWidth (const std::wstring &path, int width, bool highQuality, int render /* flags from RXMinimap::enum*/);





private:
    std::auto_ptr<RXMinimap>        minimapGen;
    std::auto_ptr<RXSCMpqReader>    mpqReader;
    std::auto_ptr<RXCache>          replayCache;





    //Get a RXMap from a .scm or a .rep
    RXMap * load    (const std::wstring &path, const RXReplay *&out);
    RXMap * loadRep (const std::wstring &path, const RXReplay *&out);
    RXMap * loadSCM (const std::wstring &path);


    //Do the rendering
    void             initRendering();
    RXBitmap *       generate(const RXMap *map, const RXReplay *replay, int width, bool highQuality, int render);
    RXRenderableMap *convert(const RXMap *map, const RXReplay *replay, bool highQuality);

    //Compute dimension
    int              getMaxDimensionFromRatio(const RXMap *map, float ratio);
    int              getValidMaxDimension(int maxDim);
};


