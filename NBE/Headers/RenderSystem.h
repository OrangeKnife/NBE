#pragma once

//#include "RenderObject.h"
#include "Shader.h"
#include "Matrix.h"
#include "Input.h"
#include "RendererWindow.h"
#include "RenderTarget.h"
#define uint unsigned int
namespace NBE
{
	enum RenderSystemType
	{
		GL,
		DX11,
		ERRORSYSTEMTYPE
	};
    class NBE_API RenderSystem
    {
    public:
        RenderSystem()
        {}
        virtual ~RenderSystem()
        {}
        //interface 
        virtual RendererWindow* getWindow()=0;
        virtual bool isActive()=0;//means is window active
        virtual void update()=0;
        virtual void clearColorBuffer(float* color)=0;
        virtual void clearDepthBuffer(double depth)=0;
        virtual void swapBuff(bool sync)=0;
        
        virtual uint createTexture(const String& name, void* data, uint width, uint height, uint format, uint type,bool mipmap)=0;
        virtual void updateTexture(uint idx, void* data, uint width, uint height, uint format, uint type)=0;
        virtual void destroyTexture(uint idx)=0;
        virtual void applyTexture(uint shaderidx, char* shaderTexName, uint textureIdx, int sampleUnit)=0;

        //virtual uint addRenderObject(RenderObject* obj)=0;
        //virtual void deleteRenderObject(RenderObject* obj)=0;

        virtual void bindVertexBuffer( uint idx, uint stride,uint offset, Shader* pShader )=0;
        virtual void bindIndexBuffer(uint idx)=0;
        virtual void applyShader(uint idx, uint techId = 0, uint passId = 0)=0;

        virtual void applyMatrix(uint shaderidx, char* matrixName, Matrix4f& modelMat)=0;
        virtual void applyVec3f(uint shaderidx, char* variableName, float* startPointer)=0;
        virtual void applyFloat( uint shaderidx, char* variableName, float val )=0;
        virtual void applyArray(uint shaderidx, char* variableName, uint arraySize, void* pArray, uint elementSize)=0;

        virtual void createVBO(RenderObject* ro, uint verSize ,uint shIdx, char* fmt)=0;
        virtual void createIBO(RenderObject* ro)=0;
        virtual bool updateVBO(uint vboIdx, void* newBuff, uint start, uint length,uint verSize)=0;

        virtual Shader* loadShader(const String& fileName, const char* format)=0;
 		virtual Shader* loadShader(const String& vsName,const String& psName, const char* format)=0;

        virtual void drawIndex(uint drawingMod, uint count, uint startIndexLoc,int baseVertexLocation)=0;
        //virtual void endDraw(Shader* pShader)=0;

        virtual void beginFrame()=0;
        virtual void endFrame()=0;

		//render target
		virtual void setRenderTarget(uint idx)=0;
		virtual void createRenderTarget(uint& idxOfRT, uint& idxOfTex/*shader resource*/,uint format, uint w, uint h)=0;
		virtual RenderTarget* getRenderTarget(int fboidx)=0;
		virtual void deleteRenderTarget(int fboIdx)=0;
		
    };
}