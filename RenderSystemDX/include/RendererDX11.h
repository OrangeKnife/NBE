#pragma once

#include <memory>
#include <vector>
#include "RenderInfo.h"
#include "NBEWindow.h"
#include "Dx11DeviceManager.h"
namespace NBE
{
	class RendererDX11 : public RenderSystem
	{
	public:
		RendererDX11(RenderInfo* ri, NBEWindow* win);

		~RendererDX11();

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

		void bindVertexBuffer(uint idx, uint stride, uint offset, Shader* pShader);
		void bindIndexBuffer(uint idx);
		void applyShader(uint idx, uint techId, uint passId);

		void applyMatrix(uint shaderidx, char* matrixName, Matrix4f& modelMat);
		void applyVec3f(uint shaderidx, char* variableName, float* startPointer);
		void applyFloat(uint shaderidx, char* variableName, float val);
		void applyArray(uint shaderidx, char* variableName, uint arraySize, void* pArray, uint elementSize);

		//void createVBO(RenderObject* ro, uint verSize, uint shIdx, char* fmt);
		//void createIBO(RenderObject* ro);
		void createVBO(void* verticesData, uint verSize, uint verNum, uint shIdx, char* fmt);
		void createIBO(void* indicesData, uint indexNum);
		bool updateVBO(uint vboIdx, void* newBuff, uint start, uint length, uint verSize);

		Shader* _loadShader(const String& fileName);
		Shader* loadShader(const String& fileName, const char* format);
		Shader* loadShader(const String& vsName, const String& psName, const char* format);

		void drawIndex(uint drawingMod, uint count, uint startIndexLoc, int baseVertexLocation);
		//void endDraw(Shader* pShader);

		void beginFrame();
		void endFrame();

		void deleteBuffers(std::vector<uint>& buffVec);

		void setRenderTarget(uint idx);
		void createRenderTarget(uint& idxOfRT, uint& idxOfTex/*shader resource*/, uint format, uint w, uint h);
		RenderTarget* getRenderTarget(int fboidx);
		void deleteRenderTarget(int fboIdx);

		void setViewPort(uint w, uint h, float minDepth, float maxDepth);
		void cullFace(uint CULLMODE);
		int getRenderType();
		Dx11DeviceManager* getDeviceMgr();
		ID3D11Texture2D* _createDX11Tex2D(void* data, uint width, uint height, uint format, uint type);
		Shader* loadShader(wchar_t* fileName);
		ID3D11ShaderResourceView* _createShaderResourceView(ID3D11Texture2D* tex);
		void clearDepthBuffer(float depth);
		void _applyLastShaderResource();

		template<typename T>
		void releaseVec(vector<T*>& vec);
	private:
		RenderInfo* m_pRendererInfo;
		//dx11
		Dx11DeviceManager* m_deviceManager;
		ID3D11Device* m_currentDevice;

		ID3D11DeviceContext* m_context;
		IDXGISwapChain* m_swapChain;
		//support different effect render targets
		//ID3D11RenderTargetView* m_rtview;
		vector<ID3D11RenderTargetView*> m_rtviewVec;
		uint currentRT;
		ID3D11DepthStencilView* m_dsview;
		ID3D11RasterizerState* m_rs;
		D3D11_VIEWPORT* viewport;
		//		ID3D11SamplerState* m_sampler;

		vector<ID3D11InputLayout*> m_inputLayoutVec;
		vector<ID3DX11Effect*> m_effectVec;
		//buffer
		vector<ID3D11Buffer*> m_ibuffVec;
		vector<ID3D11Buffer*> m_vbuffVec;
		//texture
		vector<ID3D11ShaderResourceView*> m_texVec;

		//store the last "applyShader idx"
		int m_lastShaderIdx;

		std::shared_ptr<NBEWindow> m_win;
		std::vector<RenderTarget> m_rtVec;//render target

		ID3D11BlendState* m_BlendState;

	};


	NBE_API RenderSystem* __cdecl createDX11(RenderInfo* ri, HINSTANCE h);
}