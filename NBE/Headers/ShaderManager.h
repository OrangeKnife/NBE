#pragma once
#include <vector>

namespace NBE
{
    class NBE_API ShaderManager: public Singleton<ShaderManager>
    {
    public:
        ShaderManager();
        ~ShaderManager();
        void initialize(RenderSystem* currentRenderSystem);
        unsigned int loadShader(const String& fileName,const char* format);
		unsigned int loadShader(const String& vsName,const String& psName,const char* format);
        //unsigned int loadShader(String& fileName);//no extension; eg. L"specular"
        Shader* getShaderByIdx(unsigned int idx);
    private:
        std::vector<Shader*> m_ShaderVec;
        RenderSystem* m_RenderSystem;
    };
}