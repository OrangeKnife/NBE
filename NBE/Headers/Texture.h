#pragma once 
#include "NBE.h"

using namespace std;
namespace NBE
{
    struct Texture
    {
    public:
        typedef unsigned int Size;	// Used for the width and height
        typedef unsigned int Color;	// RGBA color

        Texture(const String& _path, const String& _name, unsigned int w = 0, 
                unsigned int h = 0, unsigned int _format = 0, unsigned int _type = 0)
                :path(_path),name(_name),width(w),height(h),format(_format),type(_type)
        {}

        Texture(unsigned int idx):textureIdx(idx){}
        ~Texture()
        {}
        
        unsigned int width;
        unsigned int height;
        unsigned int format;
        unsigned int type;
        String path;
        String name;
        unsigned int textureIdx;
    };
}