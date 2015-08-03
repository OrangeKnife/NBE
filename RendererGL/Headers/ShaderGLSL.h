#pragma once

namespace NBE
{
	class ShaderGLSL: public Shader
	{
	public:
		ShaderGLSL(const String& name, const char* format);
		ShaderGLSL(const String& name_vs,const String& name_ps, const char* format);
		~ShaderGLSL();
		inline uint getProgram(){return m_program;}

		void bindAttribute(const char* format);
	private:
		uint verSh;
		uint fragSh;
		uint m_program;

	};
}