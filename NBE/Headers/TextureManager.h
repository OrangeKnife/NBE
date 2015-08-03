#pragma once
#include "Texture.h"
#include <list>

using namespace std;
namespace NBE
{
    class NBE_API TextureManager: public Singleton<TextureManager>
    {
    public:
        TextureManager();
        ~TextureManager();

        Texture* LoadFromMemory(String& uniName, Texture::Size width, Texture::Size height, 
            unsigned int format, unsigned int type, void* pRGBATexels );
 

        void DestroyTexture( Texture*& pTexture );
        void DestroyTexture( unsigned int id);
		Texture* LoadFromFile(const string& szTexturePath, const string& uniName);
        Texture* LoadFromFile(const String& szTexturePath, const String& uniName);
        
        void UpdateTexture(Texture*& pTexture, void* data, unsigned int width, unsigned int height,
            unsigned int format, unsigned int type);

        //unsigned char* getTexturePixel(String& filePath);
        
        Texture* getTextureByName(String& textureName );

        Texture* getTextureByIdx(unsigned int id);
		//Texture* addTextureByIdx(unsigned int idx);

        void DestroyAllTextures();

        void initialize(RenderSystem* renderer);
        Texture* getDefaultTex();
        
    private:
        bool isPowerOfTwo(unsigned int num);
        list<Texture*> m_loadedTextures;
        RenderSystem* m_currentRenderer;

    };

}
