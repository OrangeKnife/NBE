#include "MyAssert.h"
#include "Shader.h"

#include "NBE.h"

#include "RenderSystem.h"
#include "Singleton.h"
#include "ShaderManager.h"

namespace NBE
{
	ShaderManager::ShaderManager()
		:m_RenderSystem(nullptr)
	{}

	ShaderManager::~ShaderManager()
	{
		auto it = m_ShaderVec.begin();
		for (;it!= m_ShaderVec.end();)
		{
			delete (*it);
			it = m_ShaderVec.erase(it);
		}
	}

	void ShaderManager::initialize(RenderSystem* currentRenderSystem)
	{
		myassert(!m_RenderSystem);
		m_RenderSystem = currentRenderSystem;
	}

	unsigned int ShaderManager::loadShader(const String& fileName, const char* format)
	{
		myassert(m_RenderSystem);
		Shader* newSh = m_RenderSystem->loadShader(fileName,format);
		m_ShaderVec.push_back(newSh);
		return newSh->idx;
	}

	unsigned int ShaderManager::loadShader(const String& vsName,const String& psName, const char* format)
	{
		myassert(m_RenderSystem);
		Shader* newSh = m_RenderSystem->loadShader(vsName,psName,format);
		m_ShaderVec.push_back(newSh);
		return newSh->idx;
	}

	Shader* ShaderManager::getShaderByIdx(unsigned int idx)
	{
		for(auto it = m_ShaderVec.begin();it!=m_ShaderVec.end();++it)
		{
			if ((*it)->idx == idx )
			{
				return *it;
			}
		}
		//errorMsg("There is no such shader idx","Error Shader");
		return nullptr;
	}
}