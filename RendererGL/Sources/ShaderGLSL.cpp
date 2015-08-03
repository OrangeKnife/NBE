#include "GLheader.h"
#include "Shader.h"
#include "File.h"
#include "ShaderGLSL.h"


#include <vector>
namespace NBE
{
	ShaderGLSL::ShaderGLSL(const String& name, const char* format)
	{
		
		char* source = nullptr;
		String shaderFileName = String(name).append(L".vs");
 
		FileIO::readFileIntoStr(shaderFileName,source);
		verSh = glCreateShader(GL_VERTEX_SHADER);
		const char * constStr = (const char *)source;
		glShaderSource(verSh,1,&constStr,nullptr);
		delete source;
 
		glCompileShader(verSh);
 
		int success;
		glGetShaderiv(verSh, GL_COMPILE_STATUS, &success);
		int length;
		glGetShaderiv (verSh, GL_INFO_LOG_LENGTH, &length);
		char log[1024];
		glGetShaderInfoLog (verSh, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(VertexShaderError, String(L"Create vertex shader error"));
		}
 
		shaderFileName = String(name).append(L".ps");
		FileIO::readFileIntoStr(shaderFileName,source);
 
		fragSh = glCreateShader(GL_FRAGMENT_SHADER);
		constStr = (const char *)source;
		glShaderSource(fragSh,1, &constStr,nullptr);
		delete source;
 
		glCompileShader(fragSh);
 
		glGetShaderiv(fragSh, GL_COMPILE_STATUS, &success);
 
		glGetShaderiv (fragSh, GL_INFO_LOG_LENGTH, &length);
 
		glGetShaderInfoLog (fragSh, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(PixelShaderError, String(L"Create pixel shader error"));
		}
		m_program = glCreateProgram();
 
		glAttachShader(m_program,verSh);
		glAttachShader(m_program,fragSh);
 
		bindAttribute(format);
 
		glLinkProgram(m_program);
		glGetShaderiv(m_program, GL_LINK_STATUS, &success);
		glGetProgramInfoLog (m_program, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(LinkShaderError, String(L"GL: Link shader error"));
		}

		 
		
 

		 
	}

	ShaderGLSL::ShaderGLSL(const String& name_vs, const String& name_ps, const char* format)
	{
		
		char* source = nullptr;
		String shaderFileName = String(name_vs).append(L".vs");
 
		FileIO::readFileIntoStr(shaderFileName,source);
		verSh = glCreateShader(GL_VERTEX_SHADER);
		const char * constStr = (const char *)source;
		glShaderSource(verSh,1,&constStr,nullptr);
		delete source;
 
		glCompileShader(verSh);
 
		int success;
		glGetShaderiv(verSh, GL_COMPILE_STATUS, &success);
		int length;
		glGetShaderiv (verSh, GL_INFO_LOG_LENGTH, &length);
		char log[1024];
		glGetShaderInfoLog (verSh, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(VertexShaderError, String(L"Create vertex shader error"));
		}
 
		shaderFileName = String(name_ps).append(L".ps");
		FileIO::readFileIntoStr(shaderFileName,source);
 
		fragSh = glCreateShader(GL_FRAGMENT_SHADER);
		constStr = (const char *)source;
		glShaderSource(fragSh,1, &constStr,nullptr);
		delete source;
 
		glCompileShader(fragSh);
 
		glGetShaderiv(fragSh, GL_COMPILE_STATUS, &success);
 
		glGetShaderiv (fragSh, GL_INFO_LOG_LENGTH, &length);
 
		glGetShaderInfoLog (fragSh, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(PixelShaderError, String(L"Create pixel shader error"));
		}
		m_program = glCreateProgram();
 
		glAttachShader(m_program,verSh);
		glAttachShader(m_program,fragSh);


		bindAttribute(format);

 
		glLinkProgram(m_program);
		glGetShaderiv(m_program, GL_LINK_STATUS, &success);
		glGetProgramInfoLog (m_program, length, &length, log);
		if (success == GL_FALSE)
		{
			throw NException(LinkShaderError, String(L"GL: Link shader error"));
		}		 
	}

	ShaderGLSL::~ShaderGLSL()
	{}

	void ShaderGLSL::bindAttribute(const char* format)
	{
		int texcoordCount = 0;
		int len = strlen(format);
		myassert(len>0);

		uint AttrName,sizeOfAttr,typeOfAttr;
		typeOfAttr = GL_FLOAT;

		for(int i = 0; i<len; i++)
		{ 
			switch(format[i])
			{
			case 'P': glBindAttribLocation(m_program, POS, ATTRI_Names[POS].c_str());
					  AttrName = POS; sizeOfAttr = 3; typeOfAttr = GL_FLOAT;
					  break;
			case 'T': glBindAttribLocation(m_program, TEXCOORD0+texcoordCount, ATTRI_Names[TEXCOORD0+texcoordCount].c_str());
					  AttrName = TEXCOORD0+texcoordCount; sizeOfAttr = 2; typeOfAttr = GL_FLOAT;
					  texcoordCount++;
					  myassert(texcoordCount<4)
					  break;
			case 'N': glBindAttribLocation(m_program, NORMAL, ATTRI_Names[NORMAL].c_str());
					  AttrName = NORMAL; sizeOfAttr = 3; typeOfAttr = GL_FLOAT;
					  break;
			case 'C': glBindAttribLocation(m_program, COLOR, ATTRI_Names[COLOR].c_str());
					  AttrName = COLOR; sizeOfAttr = 4; typeOfAttr = GL_FLOAT;
					  break;
			case 'G': glBindAttribLocation(m_program, TANGENT, ATTRI_Names[TANGENT].c_str());
					  AttrName = TANGENT; sizeOfAttr = 3; typeOfAttr = GL_FLOAT;
					  break;//tangent
			case 'B': glBindAttribLocation(m_program, BITANGENT, ATTRI_Names[BITANGENT].c_str());
					  AttrName = BITANGENT; sizeOfAttr = 3; typeOfAttr = GL_FLOAT;
					  break;//bitangent
			case 'I': glBindAttribLocation(m_program, BONE, ATTRI_Names[BONE].c_str());
					  AttrName = BONE; sizeOfAttr = 4; typeOfAttr = GL_INT;
					  break;//bone indices
			case 'W': glBindAttribLocation(m_program, WEIGHT, ATTRI_Names[WEIGHT].c_str());
					  AttrName = BONE; sizeOfAttr = 4; typeOfAttr = GL_FLOAT;
					  break;//bone weight
			default: myassert(0);
			}

			attriList.push_back(AttriInfo(AttrName,sizeOfAttr, typeOfAttr));
		}
	}
}