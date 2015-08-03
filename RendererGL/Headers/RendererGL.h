#pragma once

#include <memory>
#include <vector>
#include "RenderInfo.h"
#include "NBEWindow.h"
namespace NBE
{ 
	struct GL_INPUT_ELEMENT_DES
	{
		uint OffsetBytes;
		uint Format;
	};

	struct GL_LAYOUT
	{
		uint len;//layout len
		GL_INPUT_ELEMENT_DES* declList;//layout info list
		GL_LAYOUT(uint _len,GL_INPUT_ELEMENT_DES* _decl)
			:len(_len)
		{
			declList = new GL_INPUT_ELEMENT_DES[len];
			memcpy_s(declList,sizeof(GL_INPUT_ELEMENT_DES)*len, _decl,sizeof(GL_INPUT_ELEMENT_DES)*len);
		}
		~GL_LAYOUT()
		{
			delete[] declList;
		}
	};


	class RendererGL: public RenderSystem
	{
	public:
		RendererGL(RenderInfo* ri, NBEWindow* win);
		 
		~RendererGL();

		RendererWindow* getWindow();
		bool isActive();
		void update();
		void clearColorBuffer(float* color);
		void clearDepthBuffer(double depth);
		void swapBuff(bool sync);

		uint createTexture(const String& name, void* data, uint width, uint height, uint format, uint type, bool mipmap);
		void updateTexture(uint idx, void* data, uint width, uint height, uint format, uint type);
		void destroyTexture(uint idx);
		void applyTexture(uint shaderidx, char* shaderTexName, uint textureIdx, int sampleUnit);

		//uint addRenderObject(RenderObject* obj);
		//void deleteRenderObject(RenderObject* obj);

		void bindVertexBuffer( uint idx, uint stride,uint offset, Shader* pShader );
		void bindIndexBuffer(uint idx);
		void applyShader(uint idx, uint techId, uint passId);

		void applyMatrix(uint shaderidx, char* matrixName, Matrix4f& modelMat);
		void applyVec3f(uint shaderidx, char* variableName, float* startPointer);
		void applyFloat( uint shaderidx, char* variableName, float val );
		void applyArray(uint shaderidx, char* variableName, uint arraySize, void* pArray, uint elementSize);

		void createVBO(RenderObject* ro, uint verSize ,uint shIdx, char* fmt);
		void createIBO(RenderObject* ro);
		bool updateVBO(uint vboIdx, void* newBuff, uint start, uint length,uint verSize);

		Shader* loadShader(const String& fileName, const char* format);
		Shader* loadShader(const String& vsName,const String& psName, const char* format);
		
		void drawIndex(uint drawingMod, uint count, uint startIndexLoc,int baseVertexLocation);
		//void endDraw(Shader* pShader);

		void beginFrame();
		void endFrame();

		void deleteBuffers(std::vector<uint>& buffVec);

		void setRenderTarget(uint idx);
		void createRenderTarget(uint& idxOfRT, uint& idxOfTex/*shader resource*/,uint format, uint w, uint h);
		RenderTarget* getRenderTarget(int fboidx);
		void deleteRenderTarget(int fboIdx);

	private:
		RenderInfo* m_pRendererInfo;
		HDC m_hdc;
		HGLRC m_renderingContext;
		std::shared_ptr<NBEWindow> m_win;

		//texture
		std::vector<uint> m_texVec;
		//
		//vbo ibo
		std::vector<uint> m_vbuffVec;
		std::vector<uint> m_ibuffVec;
		//layout
		std::vector<GL_LAYOUT*> m_layoutVec;
		//shader 
		std::vector<uint> m_programVec; //need to use the attributes id
		
		std::vector<RenderTarget> m_rtVec;//render target

	};


	NBE_API RenderSystem* __cdecl createGL(RenderInfo* ri, HINSTANCE h);
}