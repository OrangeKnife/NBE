#include "ShaderHLSL.h"
#include "Dx11DeviceManager.h"



namespace NBE
{
 
	ShaderHLSL::~ShaderHLSL()
	{
		//m_pEffect->Release();
		//control by the rendersystem not shadermanager
	}


	ShaderHLSL::ShaderHLSL(const String& shaderName,ID3D11Device* device)
	{
		//ID3D11Device* device = reinterpret_cast<RendererDX11*>(renderer)->getDeviceMgr()->getDevice(0);
		String fxName = String(shaderName);
		fxName = fxName.append(TEXT(".fx"));

		ID3D10Blob *fxblob = NULL, *errblob = NULL;
		if(FAILED(D3DCompileFromFile(fxName.c_str(), NULL, NULL, "", "fx_5_0", 
			D3D10_SHADER_ENABLE_STRICTNESS, 0, &fxblob, &errblob))) 
		{
			Exit((char*)errblob->GetBufferPointer());
			errblob->Release();
		}
		HV(D3DX11CreateEffectFromMemory(fxblob->GetBufferPointer(), fxblob->GetBufferSize(), 0, device, &m_pEffect));
		fxblob->Release();
	
	}

}