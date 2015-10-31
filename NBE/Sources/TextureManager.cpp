#include "RenderSystem.h"
#include "Singleton.h"
#include "TextureManager.h"
#include "MyAssert.h"
#include "IL/il.h"
#pragma comment (lib,"DevIL.lib")

#include "Singleton.h"
#include "TypeCast.h"
#include "DataTypes.h"
#define REQUIRES myassert
namespace NBE
{
	TextureManager::TextureManager()
		:m_currentRenderer(nullptr)
	{
		ilInit();
	}

	TextureManager::~TextureManager()
	{
		DestroyAllTextures();
	}

	Texture* TextureManager::LoadFromMemory(String& uniName, Texture::Size width, 
		Texture::Size height, unsigned int format, unsigned int type, void* pRGBATexels )
	{
		REQUIRES( width > 0 );
		REQUIRES( height > 0 );
		REQUIRES( pRGBATexels );

		Texture* newTex = new Texture(String(),uniName,width,height,format,type);

		//bool powerOftwo = isPowerOfTwo(width) && isPowerOfTwo(height);
		newTex->textureIdx = m_currentRenderer->createTexture(TEXT(""), pRGBATexels, newTex->width, newTex->height, newTex->format, newTex->type, true);
		
		m_loadedTextures.push_back(newTex);

		// The buffer pointed to by pRGBATexels 
		// *may be* modified by this function.
		return newTex;
	}

	//Texture* TextureManager::LoadFromFile(const string& szTexturePath, const string& uniName)
	//{
	//	return LoadFromFile(TypeCast::stringToString(const_cast<string&>(szTexturePath))
	//		,TypeCast::stringToString(const_cast<string&>(uniName)));
	//}
	Texture* TextureManager::LoadFromFile(const String& szTexturePath, const String& uniName)
	{
		REQUIRES(m_currentRenderer);
		REQUIRES( !szTexturePath.empty() );

		//TODO check if the texture is loaded before : return the old tex

		//ilInit();

		ilEnable( IL_ORIGIN_SET );
		ilOriginFunc( IL_ORIGIN_LOWER_LEFT );

		ILuint idImage = ilGenImage();
		ilBindImage( idImage );
		ilLoadImage( szTexturePath.c_str() );


		if(IL_NO_ERROR != ilGetError())
		{
			throw NException(TextureDevilError, String(TEXT("IL Load From File Error:\n")) + szTexturePath);
		}

		Texture* newTex = new Texture(szTexturePath,uniName);

		ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);
		//convert to this type

		newTex->width = ilGetInteger( IL_IMAGE_WIDTH );
		newTex->height = ilGetInteger( IL_IMAGE_HEIGHT );
		newTex->format = NBE_COLOR_RGBA;
		newTex->type = NBE_UNSIGNED_BYTE;

		newTex->textureIdx = m_currentRenderer->createTexture(szTexturePath.c_str(),
			ilGetData(),newTex->width, newTex->height, newTex->format, newTex->type, true);
																						
		m_loadedTextures.push_back(newTex);
		
		// Delete the DevIL image.

		ilDeleteImage( idImage );
		return newTex;

	}

	void TextureManager::UpdateTexture(Texture*& pTexture, void* data, unsigned int width, unsigned int height,
		unsigned int format, unsigned int type)
	{	
		REQUIRES(m_currentRenderer);
		REQUIRES( pTexture );
		m_currentRenderer->updateTexture(pTexture->textureIdx,data, width, height,format,type);
	}

	void TextureManager::DestroyTexture( Texture*& pTexture )
	{
		REQUIRES(m_currentRenderer);
		REQUIRES( pTexture );

		m_currentRenderer->destroyTexture(pTexture->textureIdx);

		std::list<Texture*>::iterator i = m_loadedTextures.begin();
		while (i!=m_loadedTextures.end())
		{
			if (pTexture == *i)
			{
				i = m_loadedTextures.erase(i);
				delete pTexture;
			}else
				++i;
		}

	}

	void TextureManager::DestroyTexture( unsigned int id )
	{
		REQUIRES(m_currentRenderer);
		REQUIRES( id>=0 );

		m_currentRenderer->destroyTexture(id);

		std::list<Texture*>::iterator i = m_loadedTextures.begin();
		while (i!=m_loadedTextures.end())
		{
			if (id == (*i)->textureIdx)
			{
				delete (*i);
				i = m_loadedTextures.erase(i);	
			}else
				++i;
		}
	}

	Texture* TextureManager::getTextureByName(String& textureName )
	{
		REQUIRES(m_currentRenderer);
		std::list<Texture*>::iterator i = m_loadedTextures.begin();
		while (i!=m_loadedTextures.end())
		{
			if ((*i)->path == textureName )
			{
				return (*i);
			}
			++i;
		}
		return 0;
	}
	
	Texture* TextureManager::getTextureByIdx( unsigned int id )
	{
		REQUIRES(m_currentRenderer);
		std::list<Texture*>::iterator i = m_loadedTextures.begin();
		while (i!=m_loadedTextures.end())
		{
			if ((*i)->textureIdx == id)
			{
				return (*i);
			}
			++i;
		}
		return nullptr;
	}

	void TextureManager::DestroyAllTextures()
	{
		REQUIRES(m_currentRenderer);
		std::list<Texture*>::iterator it = m_loadedTextures.begin();
		while (it!=m_loadedTextures.end())
		{
			m_currentRenderer->destroyTexture((*it)->textureIdx);
			delete *it;
			it = m_loadedTextures.erase(it); // re-locate it
		}
	}

	Texture* TextureManager::getDefaultTex()
	{
		REQUIRES(m_currentRenderer);
		return *m_loadedTextures.begin();//default-alpha.png
	}

	void TextureManager::initialize(RenderSystem* renderer)
	{
		REQUIRES(!m_currentRenderer);
		m_currentRenderer = renderer;
	}


	bool TextureManager::isPowerOfTwo(unsigned int num)
	{
		//move out of here?
		return (num & (num - 1)) == 0;
	}

	//Texture* TextureManager::addTextureByIdx( unsigned int idx )
	//{
	//    REQUIRES(m_currentRenderer);
	//    Texture* newOne = new Texture(idx);
	//    m_loadedTextures.push_back(newOne);
	//    return newOne;
	//}
}