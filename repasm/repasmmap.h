#ifndef __RX_REPASMMAP
#define __RX_REPASMMAP



void generateMinimapRatioFromScm(const char *datPath,const char *srcPath, const char *destPath, float ratio, int quality, int jpegQuality, int renderingMode);
void generateMinimapDimFromScm(const char *datPath,const char *srcPath, const char *destPath, int maxDimension, int quality, int jpegQuality, int renderingMode);



void generateMinimapRatioFromRep(const char *datPath,const char *srcPath, const char *destPath, float ratio, int quality, int jpegQuality, int renderingMode);
void generateMinimapDimFromRep(const char *datPath,const char *srcPath, const char *destPath, int maxDimension, int quality, int jpegQuality, int renderingMode);
RXMap * getCHKMap(const char *path);

#endif
