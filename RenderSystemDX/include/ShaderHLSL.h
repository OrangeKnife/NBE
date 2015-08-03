#pragma once
#include "DXRequirements.h"
#include "Shader.h"
namespace NBE
{
	class NBE_API ShaderHLSL: public Shader
	{
	public:
		ShaderHLSL(const String& shaderName, ID3D11Device* device);
		~ShaderHLSL();
		inline ID3DX11Effect* getEffect(){return m_pEffect;}
	private:
		ID3DX11Effect* m_pEffect;
	};
}