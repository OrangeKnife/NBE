#include "NBE.h"
#include "RenderSystem.h"
#include "DXRequirements.h"

#include "RenderObject.h"
#include "RendererDX11.h"
#include "Config.h"
#include "ShaderHLSL.h"
#include "Dx11DeviceManager.h"
#include <algorithm>
#include "DXTypes.h"


namespace NBE
{

	RendererDX11::RendererDX11(RenderInfo* ri, NBEWindow* win)
		:m_pRendererInfo(ri), m_lastShaderIdx(-1)
	{
		m_win.reset(win);

		DXGI_SWAP_CHAIN_DESC sd =
		{
			{ ri->width, ri->height, { 60, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED },
			{ 1, 0 },
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			1,
			win->getHWND(),
			!ri->fullScreen,
			DXGI_SWAP_EFFECT_DISCARD,
			0
		};
		D3D_FEATURE_LEVEL featurelevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		D3D_FEATURE_LEVEL featurelevelpicked;
		uint createdeviceflags = 0;
#ifdef _DEBUG
		createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HV(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, featurelevels, sizeof(featurelevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION,
			&sd, &m_swapChain, &m_currentDevice, &featurelevelpicked, &m_context));
		ID3D11Texture2D *backbuffer;
		HV(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbuffer));

		ID3D11RenderTargetView* rtview;
		HV(m_currentDevice->CreateRenderTargetView(backbuffer, NULL, &rtview));
		HV(backbuffer->Release());
		D3D11_TEXTURE2D_DESC desc =
		{
			m_pRendererInfo->width, m_pRendererInfo->height,
			1, 1,
			DXGI_FORMAT_D32_FLOAT,
			{ 1, 0 },
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL,
			0, 0
		};
		ID3D11Texture2D *tex = NULL;
		HV(m_currentDevice->CreateTexture2D(&desc, NULL, &tex));

		//////////////////////////////////////////////////////////////////////////
		// Initialize the description of the stencil state.
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* depthStencilState;
		HV(m_currentDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));
		// Set the depth stencil state.
		m_context->OMSetDepthStencilState(depthStencilState, 1);
		HV(depthStencilState->Release());
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		// Initialize the depth stencil view.
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = desc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;


		HV(m_currentDevice->CreateDepthStencilView(tex, &depthStencilViewDesc, &m_dsview));
		HV(tex->Release());



		m_rtviewVec.push_back(rtview);//original
		m_context->OMSetRenderTargets(1, &rtview, m_dsview);
		currentRT = 0;//initialize

		m_BlendState = NULL;

		D3D11_BLEND_DESC BlendState;
		ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
		BlendState.AlphaToCoverageEnable = false;
		BlendState.IndependentBlendEnable = false;
		BlendState.RenderTarget[0].BlendEnable = true;

		BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;	//D3D11_BLEND_SRC_COLOR;
		BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_DEST_COLOR;
		BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;//D3D11_BLEND_ONE;
		BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;//D3D11_BLEND_ZERO;
		BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_currentDevice->CreateBlendState(&BlendState, &m_BlendState);

		UINT sampleMask = 0xffffffff;
		m_context->OMSetBlendState(m_BlendState, NULL, sampleMask);

		D3D11_RASTERIZER_DESC rasterizerstate = { D3D11_FILL_SOLID, D3D11_CULL_BACK, true, 0, 0, 0, true, false, false, false };
		HV(m_currentDevice->CreateRasterizerState(&rasterizerstate, &m_rs));
		m_context->RSSetState(m_rs);
		setViewPort(0, 0, 0, 1);
		//manager the device
		m_deviceManager = new Dx11DeviceManager();
		m_deviceManager->addDevice(m_currentDevice);


	}

	void RendererDX11::cullFace(uint CULLMODE)
	{
		D3D11_RASTERIZER_DESC  rasterizerstate;//= { D3D11_FILL_SOLID, (D3D11_CULL_MODE)CULLMODE, true, 0, 0, 0, true, false, false, false };

		m_rs->GetDesc(&rasterizerstate);
		rasterizerstate.CullMode = (D3D11_CULL_MODE)CULLMODE;
		m_rs->Release();
		HV(m_currentDevice->CreateRasterizerState(&rasterizerstate, &m_rs));
		m_context->RSSetState(m_rs);
	}

	void RendererDX11::setViewPort(uint w, uint h, float minDepth, float maxDepth)
	{
		D3D11_VIEWPORT* tviewport;
		tviewport = new D3D11_VIEWPORT();
		tviewport->TopLeftX = tviewport->TopLeftY = 0;
		tviewport->Width = w == 0 ? m_pRendererInfo->width : static_cast<FLOAT>(w);
		tviewport->Height = h == 0 ? m_pRendererInfo->height : static_cast<FLOAT>(h);
		tviewport->MinDepth = minDepth;
		tviewport->MaxDepth = maxDepth;

		m_context->RSSetViewports(1, tviewport);
		delete tviewport;
	}
	void RendererDX11::update()
	{}

	int RendererDX11::getRenderType()
	{
		return RenderSystemType::DX11;
	}

	Dx11DeviceManager* RendererDX11::getDeviceMgr()
	{
		return m_deviceManager;
	}
	RendererDX11::~RendererDX11()
	{
		delete m_pRendererInfo;

		//if(m_currentDevice)m_currentDevice->Release();//release by the manager
		delete m_deviceManager;

		if (m_context)m_context->Release();
		if (m_swapChain)m_swapChain->Release();

		if (m_dsview)m_dsview->Release();
		if (m_rs)m_rs->Release();
		//		if(m_sampler)m_sampler->Release();
		if (m_BlendState)m_BlendState->Release();


		releaseVec(m_ibuffVec);
		releaseVec(m_vbuffVec);
		releaseVec(m_inputLayoutVec);
		releaseVec(m_effectVec);
		releaseVec(m_rtviewVec);

		for (vector<ID3D11ShaderResourceView*>::iterator it = m_texVec.begin()
			; it != m_texVec.end();)
		{
			if (*it)
			{
				(*it)->Release();
			}
			it = m_texVec.erase(it);

		}

	}

	template<typename T>
	void RendererDX11::releaseVec(vector<T*>& vec)
	{
		for (vector<T*>::iterator it = vec.begin()
			; it != vec.end();)
		{
			(*it)->Release();
			it = vec.erase(it);
		}
	}

	void RendererDX11::createVBO(void* verticesData, uint verSize, uint verNum, uint shIdx, char* fmt)
	{
		ID3D11Buffer* vbuff;
		PNCT_Vertex* test = (PNCT_Vertex*)verticesData;
		D3D11_SUBRESOURCE_DATA srd = { verticesData, 0, 0 };
		HV(m_currentDevice->CreateBuffer(&CD3D11_BUFFER_DESC(verNum * verSize, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), &srd, &vbuff));

		int len = strlen(fmt);
		D3D11_INPUT_ELEMENT_DESC *decl = new D3D11_INPUT_ELEMENT_DESC[len];
		memset(decl, 0, sizeof(D3D11_INPUT_ELEMENT_DESC)*len);
		int offset = 0, semidx = 0;

		for (int i = 0; i < len; i++)
		{
			D3D11_INPUT_ELEMENT_DESC &d = decl[i];

			d.AlignedByteOffset = offset;
			switch (fmt[i])
			{
			case 'P': d.SemanticName = "POSITION"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;
			case 'T': d.SemanticName = "TEXCOORD"; d.Format = DXGI_FORMAT_R32G32_FLOAT;    offset += 8;d.SemanticIndex = semidx++; break;
			case 'N': d.SemanticName = "NORMAL";   d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;
			case 'C': d.SemanticName = "COLOR";    d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  offset += 16; break;
			case 'G': d.SemanticName = "TANGENT"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;//tangent
			case 'B': d.SemanticName = "BITANGENT"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;//bitangent
			case 'I': d.SemanticName = "SKINBONE"; d.Format = DXGI_FORMAT_R32G32B32A32_SINT; offset += 16; break;//skin bone index
			case 'W': d.SemanticName = "BONEWEIGHT"; d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; offset += 16; break;//skin bone weight
			default: assert(0);
			}


		}


		//TODO remove shader* , use all the same layout for the same vertex structure

		ID3DX11Effect* effect = m_effectVec[shIdx];
		D3DX11_PASS_DESC passDesc;
		ID3DX11EffectTechnique* pRenderTechnique = effect->GetTechniqueByIndex(0);//default technique TODO: multiple technique

		ID3D11InputLayout* inputLayout;
		HV(pRenderTechnique->GetPassByIndex(0)->GetDesc(&passDesc));
		HV(m_currentDevice->CreateInputLayout(decl, len, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));
		//m_context->IASetInputLayout(inputLayout);	
		delete[] decl;
		m_inputLayoutVec.push_back(inputLayout);
		m_vbuffVec.push_back(vbuff);
		//vertex buffer and the layout have the same index in the vector
		auto vbo_id = m_vbuffVec.size() - 1;
	}
	void RendererDX11::createIBO(void* indicesData, uint indexNum)
	{
		ID3D11Buffer* ibuff;
		D3D11_SUBRESOURCE_DATA srd = { indicesData, 0, 0 };
		HV(m_currentDevice->CreateBuffer(&CD3D11_BUFFER_DESC(indexNum*sizeof(uint), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE), &srd, &ibuff));
		//m_context->IASetIndexBuffer(ibuff, DXGI_FORMAT_R32_UINT, 0);
		m_ibuffVec.push_back(ibuff);
		auto ibo_id = m_ibuffVec.size() - 1;
	}

	//void RendererDX11::createVBO(RenderObject* ro, uint verSize, uint shIdx, char* fmt)
	//{
	//	ID3D11Buffer* vbuff;
	//	D3D11_SUBRESOURCE_DATA srd = { ro->vbo, 0, 0 };
	//	HV(m_currentDevice->CreateBuffer(&CD3D11_BUFFER_DESC(ro->VertexNum * verSize, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), &srd, &vbuff));

	//	int len = strlen(fmt);
	//	D3D11_INPUT_ELEMENT_DESC *decl = new D3D11_INPUT_ELEMENT_DESC[len];
	//	memset(decl, 0, sizeof(D3D11_INPUT_ELEMENT_DESC)*len);
	//	int offset = 0, semidx = 0;

	//	for (int i = 0; i < len; i++)
	//	{
	//		D3D11_INPUT_ELEMENT_DESC &d = decl[i];

	//		d.AlignedByteOffset = offset;
	//		switch (fmt[i])
	//		{
	//		case 'P': d.SemanticName = "POSITION"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;
	//		case 'T': d.SemanticName = "TEXCOORD"; d.Format = DXGI_FORMAT_R32G32_FLOAT;    offset += 8;d.SemanticIndex = semidx++; break;
	//		case 'N': d.SemanticName = "NORMAL";   d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;
	//		case 'C': d.SemanticName = "COLOR";    d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  offset += 16; break;
	//		case 'G': d.SemanticName = "TANGENT"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;//tangent
	//		case 'B': d.SemanticName = "BITANGENT"; d.Format = DXGI_FORMAT_R32G32B32_FLOAT; offset += 12; break;//bitangent
	//		case 'I': d.SemanticName = "SKINBONE"; d.Format = DXGI_FORMAT_R32G32B32A32_SINT; offset += 16; break;//skin bone index
	//		case 'W': d.SemanticName = "BONEWEIGHT"; d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; offset += 16; break;//skin bone weight
	//		default: assert(0);
	//		}

	//		
	//	}


	//	//TODO remove shader* , use all the same layout for the same vertex structure

	//	ID3DX11Effect* effect = m_effectVec[shIdx];
	//	D3DX11_PASS_DESC passDesc;
	//	ID3DX11EffectTechnique* pRenderTechnique = effect->GetTechniqueByIndex(0);//default technique TODO: multiple technique

	//	ID3D11InputLayout* inputLayout;
	//	HV(pRenderTechnique->GetPassByIndex(0)->GetDesc(&passDesc));
	//	HV(m_currentDevice->CreateInputLayout(decl, len, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLayout));
	//	//m_context->IASetInputLayout(inputLayout);	
	//	delete[] decl;
	//	m_inputLayoutVec.push_back(inputLayout);
	//	m_vbuffVec.push_back(vbuff);
	//	//vertex buffer and the layout have the same index in the vector
	//	ro->vbo_id = m_vbuffVec.size() - 1;
	//}


	//void RendererDX11::createIBO(RenderObject* ro)
	//{
	//	ID3D11Buffer* ibuff;
	//	D3D11_SUBRESOURCE_DATA srd = { ro->ibo.data(), 0, 0 };
	//	HV(m_currentDevice->CreateBuffer(&CD3D11_BUFFER_DESC(ro->ibo.size()*sizeof(uint), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE), &srd, &ibuff));
	//	//m_context->IASetIndexBuffer(ibuff, DXGI_FORMAT_R32_UINT, 0);
	//	m_ibuffVec.push_back(ibuff);
	//	ro->ibo_id = m_ibuffVec.size() - 1;
	//}

	bool RendererDX11::updateVBO(uint vboIdx, void* newBuff, uint start, uint length, uint verSize)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		HRESULT hResult = m_context->Map(m_vbuffVec[vboIdx], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (hResult != S_OK)
			return false;

		memcpy_s((byte*)resource.pData + start, length * verSize, newBuff, length * verSize);

		m_context->Unmap(m_vbuffVec[vboIdx], 0);
		return true;

	}

	void RendererDX11::bindVertexBuffer(uint idx, uint stride, uint offset, Shader* pShader)
	{
		m_context->IASetVertexBuffers(0, 1, &m_vbuffVec[idx], &stride, &offset);
		m_context->IASetInputLayout(m_inputLayoutVec[idx]);
	}

	void RendererDX11::bindIndexBuffer(uint idx)
	{
		m_context->IASetIndexBuffer(m_ibuffVec[idx], DXGI_FORMAT_R32_UINT, 0);
	}

	void RendererDX11::applyShader(uint idx, uint techId, uint passId)
	{
		m_lastShaderIdx = idx;
		//HV(m_effectVec[idx]->GetTechniqueByIndex(techId)->GetPassByIndex(0)->Apply(0,m_context));
	}

	void RendererDX11::applyMatrix(uint shaderidx, char* matrixName, Matrix4f& modelMat)
	{
		HV(m_effectVec[shaderidx]->GetVariableByName(matrixName)->AsMatrix()->SetMatrix(modelMat.data()));
	}

	void RendererDX11::applyVec3f(uint shaderidx, char* variableName, float* startPointer)
	{
		HV(m_effectVec[shaderidx]->GetVariableByName(variableName)->AsVector()->SetFloatVector(startPointer));
	}
	void RendererDX11::applyFloat(uint shaderidx, char* variableName, float val)
	{
		HV(m_effectVec[shaderidx]->GetVariableByName(variableName)->AsScalar()->SetFloat(val));
	}
	void RendererDX11::applyArray(uint shaderidx, char* variableName, uint arraySize, void* pArray, uint elementSize)
	{
		ID3DX11EffectVariable* arrayEffVar;
		arrayEffVar = m_effectVec[shaderidx]->GetVariableByName(variableName);
		HV(arrayEffVar->SetRawValue(pArray, 0, arraySize * elementSize))
	}

	void RendererDX11::clearColorBuffer(float* color)
	{
		for_each(begin(m_rtviewVec), end(m_rtviewVec), [&](ID3D11RenderTargetView* rt) {
			m_context->ClearRenderTargetView(rt, color);
		});

	}

	void RendererDX11::clearDepthBuffer(double depth)
	{
		m_context->ClearDepthStencilView(m_dsview, D3D11_CLEAR_DEPTH, float(depth), 0);
	}

	void RendererDX11::swapBuff(bool sync)
	{
		m_swapChain->Present(sync, 0);
	}

	void RendererDX11::drawIndex(uint drawingMod, uint count, uint startIndexLoc, int baseVertexLocation)
	{
		_applyLastShaderResource();
		if (count > 0)
		{
			m_context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawingMod);
			m_context->DrawIndexed(count, startIndexLoc, 0);
		}

	}

	void RendererDX11::_applyLastShaderResource()
	{
		if (m_lastShaderIdx >= 0) {
			HV(m_effectVec[m_lastShaderIdx]->GetTechniqueByIndex(0)->GetPassByIndex(0)->Apply(0, m_context));
		}
	}
	void endDraw(Shader* pShader)
	{

	}
	//mipmap in dx
	uint getNumMipLevels(uint width, uint height)
	{
		uint numLevels = 1;
		while (width > 1 || height > 1)
		{
			width = std::max<uint>(width / 2, 1);
			height = std::max<uint>(height / 2, 1);
			++numLevels;
		}

		return numLevels;
	}

	ID3D11Texture2D* RendererDX11::_createDX11Tex2D(void* data, uint width, uint height, uint format, uint type)
	{
		type = DXTypes::Map(type);
		format = DXTypes::Map(format);

		D3D11_TEXTURE2D_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tDesc.Height = height;
		tDesc.Width = width;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.CPUAccessFlags = 0;
		tDesc.MipLevels = 0;
		tDesc.ArraySize = 1;
		tDesc.SampleDesc.Count = 1;
		tDesc.Format = (DXGI_FORMAT)format;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;



		ID3D11Texture2D * pTexture;
		HV(m_currentDevice->CreateTexture2D(&tDesc, NULL, &pTexture));
		//////////////////////////////////////////////////////////////////////////
		tDesc.Usage = D3D11_USAGE_STAGING;
		tDesc.MipLevels = 1;//only 1 level 
		tDesc.CPUAccessFlags =/* D3D11_CPU_ACCESS_WRITE|*/D3D11_CPU_ACCESS_READ;
		tDesc.BindFlags = 0;
		tDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subRd;
		ZeroMemory(&subRd, sizeof(D3D11_SUBRESOURCE_DATA));
		subRd.pSysMem = data;
		subRd.SysMemPitch = 4 * width;

		ID3D11Texture2D * pTexture_src;
		HV(m_currentDevice->CreateTexture2D(&tDesc, &subRd, &pTexture_src));
		//////////////////////////////////////////////////////////////////////////
		//copy data
		D3D11_BOX sourceRegion;
		sourceRegion.left = 0;
		sourceRegion.right = width;
		sourceRegion.top = 0;
		sourceRegion.bottom = height;
		sourceRegion.front = 0;
		sourceRegion.back = 1;

		m_context->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pTexture_src, 0, &sourceRegion);
		pTexture_src->Release();
		return pTexture;
	}

	uint RendererDX11::createTexture(const String& name, void* data, uint width, uint height, uint format, uint type, bool mipmap)
	{
		// 			type = DXTypes::Map(type);
		// 			format = DXTypes::Map(format);
		// 
		// 			D3D11_TEXTURE2D_DESC tDesc;
		// 			ZeroMemory(&tDesc,sizeof(D3D11_TEXTURE2D_DESC));
		// 			tDesc.Height = height;
		// 			tDesc.Width = width;
		// 			tDesc.Usage = D3D11_USAGE_DEFAULT;
		// 			tDesc.CPUAccessFlags = 0;
		// 			tDesc.MipLevels = 1;
		// 			tDesc.ArraySize = 1;
		// 			tDesc.SampleDesc.Count = 1;
		// 			tDesc.Format = (DXGI_FORMAT)format;
		// 			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE| D3D11_BIND_RENDER_TARGET;;
		// 
		// 			tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		// 
		// 			D3D11_SUBRESOURCE_DATA subRd;
		// 			ZeroMemory(&subRd, sizeof(D3D11_SUBRESOURCE_DATA));
		// 			subRd.pSysMem = data;
		// 			subRd.SysMemPitch = 4 * width;
		// 
		// 			// Create the 2d texture from data
		ID3D11Texture2D * pTexture = _createDX11Tex2D(data, width, height, format, type);
		ID3D11ShaderResourceView* srv = _createShaderResourceView(pTexture);

		pTexture->Release();

		m_texVec.push_back(srv);
		return m_texVec.size() - 1;
	}

	ID3D11ShaderResourceView* RendererDX11::_createShaderResourceView(ID3D11Texture2D* tex)
	{
		ID3D11ShaderResourceView* srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC rvDesc;
		ZeroMemory(&rvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		D3D11_TEXTURE2D_DESC des;
		tex->GetDesc(&des);
		rvDesc.Format = des.Format;
		rvDesc.Texture2D.MipLevels = -1;
		rvDesc.Texture2D.MostDetailedMip = 0;

		rvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		HV(m_currentDevice->CreateShaderResourceView(tex, &rvDesc, &srv));
		m_context->GenerateMips(srv);
		return srv;
	}

	void RendererDX11::setRenderTarget(uint idxOfRT)
	{
		m_context->OMSetRenderTargets(1, &m_rtviewVec[idxOfRT], m_dsview);
	}


	void RendererDX11::createRenderTarget(uint& idxOfRT, uint& idxOfTex/*shader resource*/, uint format, uint w, uint h)
	{
		format = DXTypes::Map(format);
		D3D11_TEXTURE2D_DESC tDesc;
		ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tDesc.Height = (h == 0 ? m_pRendererInfo->height : h);
		tDesc.Width = (w == 0 ? m_pRendererInfo->width : w);
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.CPUAccessFlags = 0;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.SampleDesc.Count = 1;
		tDesc.Format = (DXGI_FORMAT)format;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;;

		tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D *pTex = NULL;
		HV(m_currentDevice->CreateTexture2D(&tDesc, NULL, &pTex));

		//more render target view
		//unsigned char* data=  new unsigned char[m_pRendererInfo->width * m_pRendererInfo->height * 4];
		//ID3D11Texture2D* pTex= _createDX11Tex2D(data, m_pRendererInfo->width,  m_pRendererInfo->height,
		//	1,100);
		ID3D11RenderTargetView* rtview;
		HV(m_currentDevice->CreateRenderTargetView(pTex, NULL, &rtview));
		m_rtviewVec.push_back(rtview);

		m_texVec.push_back(_createShaderResourceView(pTex));
		idxOfRT = m_texVec.size() - 1;

		pTex->Release();
		//delete[] data;
		idxOfTex = m_rtviewVec.size() - 1;
	}

	RenderTarget* RendererDX11::getRenderTarget(int fboidx)
	{
		if (fboidx >= 0 && fboidx < (int)m_rtVec.size())
			return &m_rtVec[fboidx];
		else
			return nullptr;
	}
	void RendererDX11::deleteRenderTarget(int fboIdx)
	{
		if (fboIdx >= 0 && fboIdx < (int)m_rtVec.size())
		{
			RenderTarget rt = m_rtVec[fboIdx];
			m_rtviewVec[rt.rtIdx]->Release();
			m_rtviewVec.erase(m_rtviewVec.begin() + rt.rtIdx);
			m_rtVec.erase(m_rtVec.begin() + fboIdx);
		}
		else if (fboIdx == -1)
		{
			releaseVec(m_rtviewVec);
			m_rtVec.clear();
		}
	}

	void RendererDX11::updateTexture(uint idx, void* data, uint width, uint height, uint format, uint type)
	{
	}

	void RendererDX11::destroyTexture(uint idx)
	{
		m_texVec[idx]->Release();
		m_texVec[idx] = NULL;
		//destroy a texture should not affect the other textures' index, just release it
	}

	void RendererDX11::applyTexture(uint shaderidx, char* shaderTexName, uint textureIdx, int sampleUnit)
	{
		if (sampleUnit == -1)
		{
			m_effectVec[shaderidx]->GetVariableByName(shaderTexName)->AsShaderResource()->SetResource(NULL);
		}
		else
		{
			m_effectVec[shaderidx]->GetVariableByName(shaderTexName)->AsShaderResource()->SetResource(m_texVec[textureIdx]);
		}

	}

	Shader* RendererDX11::_loadShader(const String& fileName)
	{
		Shader* newSh = new ShaderHLSL(fileName, m_currentDevice);
		m_effectVec.push_back(reinterpret_cast<ShaderHLSL*>(newSh)->getEffect());
		newSh->idx = m_effectVec.size() - 1;
		return newSh;
	}

	Shader* RendererDX11::loadShader(const String& fileName, const char* format)
	{
		return _loadShader(fileName);
	}

	Shader* RendererDX11::loadShader(const String& vsName, const String& psName, const char* format)
	{
		return _loadShader(vsName);
	}

	RendererWindow* RendererDX11::getWindow()
	{
		return m_win.get();
	}
	bool RendererDX11::isActive()
	{
		return m_win->isActive();
	}
	void RendererDX11::beginFrame()
	{}
	void RendererDX11::endFrame()
	{}

	NBE_API RenderSystem* __cdecl createDX11(RenderInfo* ri, HINSTANCE h)
	{
		NBEWindow* win = new NBEWindow(ri, h);//delete when release renderer, they are 1:1 now
		return new RendererDX11(ri, win);
	}



}
