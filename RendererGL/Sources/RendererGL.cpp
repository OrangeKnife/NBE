#include "NBE.h"
#include "RenderSystem.h"

#include "GLheader.h"



#include "RenderObject.h"
#include "RendererGL.h"

#include "Config.h"


#include "ShaderGLSL.h"


#include <algorithm>
#include "GLTypes.h"
namespace NBE
{
	RendererGL::RendererGL(RenderInfo* ri, NBEWindow* win):m_pRendererInfo(ri)
	{
		m_win.reset(win);
		m_hdc= GetDC( m_win->getHWND());

		PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};

		pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
		pixelFormatDescriptor.nVersion = 1;

		pixelFormatDescriptor.dwFlags = 
			PFD_DRAW_TO_WINDOW | 
			PFD_SUPPORT_OPENGL | 
			PFD_DOUBLEBUFFER;
		pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
		pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
		pixelFormatDescriptor.cColorBits = 32;
		pixelFormatDescriptor.cDepthBits = 16;
		pixelFormatDescriptor.cStencilBits = 16;

		int pixelFormat = ChoosePixelFormat( m_hdc, &pixelFormatDescriptor );

		SetPixelFormat( m_hdc, pixelFormat, &pixelFormatDescriptor );

		// Create the OpenGL render context
		//
		m_renderingContext = wglCreateContext( m_hdc );
		wglMakeCurrent ( m_hdc , m_renderingContext );
	 	glewInit();//after init opengl

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		glEnable(GL_BLEND);					
		glAlphaFunc(GL_GREATER,0.f);
		glEnable(GL_ALPHA_TEST);
		//glEnable(GL_DEPTH_TEST);
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	}

	RendererGL::~RendererGL()
	{
		wglMakeCurrent( nullptr, nullptr );
		wglDeleteContext( m_renderingContext );
		m_renderingContext = nullptr;

		for (auto it = m_layoutVec.begin()
			;it!=m_layoutVec.end();)
		{
			delete (*it);
			it = m_layoutVec.erase(it);
		}


		deleteBuffers(m_vbuffVec);
		deleteBuffers(m_ibuffVec);

		std::for_each(begin(m_programVec),end(m_programVec),[](uint& n){
				glDeleteProgram(n);
		});

		delete m_pRendererInfo;

	}

	RendererWindow* RendererGL::getWindow()
	{
		return m_win.get();
	}

	bool RendererGL::isActive()
	{
		return m_win->isActive();
	}

	void RendererGL::update()
	{
		//TODO: rendering the objects craeted, need a queue, so this relates to the layer
		
		
		//auto it = m_renderObjVec.begin();
		//for(;it!=m_renderObjVec.end();++it)
		//{
			//drawIndex(4, (*it)->ibo.size(),0,0);//4 == triangles
		//}
	}

	void RendererGL::clearColorBuffer(float* color)
	{
		glClearColor(color[0],color[1],color[2],color[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void RendererGL::clearDepthBuffer(double depth)
	{

		glFrontFace(GL_CCW);//different form directx
		glClearDepth(depth);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void RendererGL::swapBuff(bool sync)
	{
		SwapBuffers(m_hdc);
		
	}

	uint RendererGL::createTexture(const String& name, void* data, uint width, uint height, uint format, uint type, bool mipmap)
	{
		// Create an OpenGL texture object.
		//

		GLuint theTexture;
		glGenTextures( 1, &theTexture );

		// Configure the texture object.
		//
		glBindTexture( GL_TEXTURE_2D, theTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		GLenum ddd = glGetError();

		// Load the texture object from DevIL.
		//
		glPixelStoref(GL_UNPACK_ALIGNMENT,1);

		format = GLTypes::Map(format);
		type = GLTypes::Map(type);

		glTexImage2D( 
			GL_TEXTURE_2D, 0, 
			GL_RGBA, 
			width, height, 0, 
			format, type, data );

		if(mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);

		m_texVec.push_back(theTexture);
		return m_texVec.size()-1;
	}

	void RendererGL::updateTexture(uint idx, void* data, uint width, uint height, uint format, uint type)
	{
		glBindTexture(GL_TEXTURE_2D, idx);     
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);

	}
	void RendererGL::destroyTexture(uint idx)
	{	
		glDeleteTextures(1,&m_texVec[idx]);
		m_texVec[idx]=0;
	}
	void RendererGL::applyTexture(uint shaderidx, char* shaderTexName, uint textureIdx, int sampleUnit)
	{
		if(sampleUnit == -1)
		{
			glDisable(GL_TEXTURE_2D);
			return;
		}

		int loc = glGetUniformLocation(m_programVec[shaderidx], shaderTexName);
		glUniform1i(loc,sampleUnit);
		glActiveTexture(GL_TEXTURE0 + sampleUnit);
		glBindTexture(GL_TEXTURE_2D, m_texVec[textureIdx]);

	}

	/*uint RendererGL::addRenderObject(RenderObject* obj)
	{
		m_renderObjVec.push_back(obj);
		return m_renderObjVec.size()-1;
	}

	void RendererGL::deleteRenderObject(RenderObject* obj)
	{
		auto it = m_renderObjVec.begin();
		for(;it!= m_renderObjVec.end();)
		{
			if(*it == obj)
				it = m_renderObjVec.erase(it);
			else
				++it;
		}
	}*/

	void RendererGL::bindVertexBuffer( uint idx, uint stride,uint offset, Shader* pShader )
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbuffVec[idx]);
		//enable attributes 

		std::vector<ShaderGLSL::AttriInfo> attriList = pShader->getAttributesList();
		GL_LAYOUT* lay = m_layoutVec[idx];
		for (uint i = 0; i < attriList.size(); ++i)
		{
			ShaderGLSL::AttriInfo info = attriList[i];
			glEnableVertexAttribArray(info.id);
			glVertexAttribPointer(info.id, info.sizeOfAttri , (info.typeOfAttri), false, stride,
				reinterpret_cast< const GLvoid* >(lay->declList[i].OffsetBytes));
		}
	}

	void RendererGL::bindIndexBuffer(uint idx)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibuffVec[idx]);
	}

	Shader* RendererGL::loadShader(const String& fileName, const char* format)
	{
		Shader* newSh = new ShaderGLSL(fileName, format);
		m_programVec.push_back(reinterpret_cast<ShaderGLSL*>(newSh)->getProgram());
		newSh->idx = m_programVec.size()-1;
		return newSh;
	}

	Shader* RendererGL::loadShader(const String& vsName,const String& psName, const char* format)
	{
		Shader* newSh = new ShaderGLSL(vsName,psName,format);
		m_programVec.push_back(reinterpret_cast<ShaderGLSL*>(newSh)->getProgram());
		newSh->idx = m_programVec.size()-1;
		return newSh;
	}

	void RendererGL::createVBO(RenderObject* ro, uint verSize ,uint shIdx, char* fmt)
	{
		GLuint vbo;
		uint length = ro->VertexNum;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, length * verSize, ro->vbo, GL_STATIC_DRAW);
		m_vbuffVec.push_back(vbo);


		int len = strlen(fmt);
		GL_INPUT_ELEMENT_DES* decl = new GL_INPUT_ELEMENT_DES[len];
		memset(decl, 0, sizeof(GL_INPUT_ELEMENT_DES)*len);
		uint offset = 0;//, semidx = 0;

		for(int i = 0; i<len; i++)
		{
			GL_INPUT_ELEMENT_DES& d = decl[i];
			d.OffsetBytes = offset;

			switch(fmt[i])
			{
			case 'P': d.Format = GL_FLOAT; offset += 12; break;
			case 'T': d.Format = GL_FLOAT; offset +=  8; break;
			case 'N': d.Format = GL_FLOAT; offset += 12; break;
			case 'C': d.Format = GL_FLOAT; offset += 16; break;
			case 'G': d.Format = GL_FLOAT; offset += 12; break;//tangent
			case 'B': d.Format = GL_FLOAT; offset += 12; break;//bitangent
			case 'I':d.Format = GL_FLOAT; offset += 16; break;//bone indices
			case 'W':d.Format = GL_FLOAT; offset += 16; break;//bone weight
			default: myassert(0);
			}
		}

		GL_LAYOUT* lay = new GL_LAYOUT(len,decl);
		m_layoutVec.push_back(lay);


		delete[] decl;
		ro->vbo_id = m_vbuffVec.size()-1;
	}

	void RendererGL::createIBO(RenderObject* ro)
	{
		GLuint ibo;
		uint length = ro->ibo.size();
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ARRAY_BUFFER, ibo);
		glBufferData(GL_ARRAY_BUFFER, sizeof( uint ) * length, &ro->ibo.front(), GL_STATIC_DRAW);
		m_ibuffVec.push_back(ibo);
		ro->ibo_id = m_ibuffVec.size()-1;
	}
	
	bool RendererGL::updateVBO(uint vboIdx, void* newBuff, uint start, uint length,uint verSize)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbuffVec[vboIdx]);
		glBufferData(GL_ARRAY_BUFFER, length * verSize, 0, GL_STATIC_DRAW);
		byte* ptr = (byte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		// if the pointer is valid(mapped), update VBO
		if(ptr)
		{
			memcpy_s(ptr + start,length * verSize,newBuff,length * verSize) ; // modify buffer data
			 glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
			return true;
		}

		
		return false;

	}

	void RendererGL::applyShader(uint idx, uint techId = 0, uint passId = 0)
	{
		glUseProgram(m_programVec[idx]);
	}
	void RendererGL::applyMatrix(uint shaderidx, char* matrixName, Matrix4f& Mat)
	{
		//uniform
		int loc = glGetUniformLocation(m_programVec[shaderidx], matrixName);
		glUniformMatrix4fv(loc,1,false,Mat.data());
	}

	void RendererGL::applyVec3f(uint shaderidx, char* variableName, float* startPointer)
	{
		int loc = glGetUniformLocation(m_programVec[shaderidx],variableName);
		glUniform3fv(loc,1,startPointer);
	}

	void RendererGL::applyFloat( uint shaderidx, char* variableName, float val )
	{
		int loc = glGetUniformLocation(m_programVec[shaderidx],variableName);
		glUniform1f(loc,val);
	}

	void RendererGL::drawIndex(uint drawingMod, uint count, uint startIndexLoc,int baseVertexLocation)
	{
		glDrawElements(drawingMod,count,GL_UNSIGNED_INT,
			reinterpret_cast<GLvoid*>(startIndexLoc * sizeof(unsigned int)));
		//disable the attrib array which is enable by bind buffer

	}

	void RendererGL::applyArray(uint shaderidx, char* variableName, uint arraySize, void* pArray, uint elementSize)
	{

	}

	void RendererGL::beginFrame()
	{

	}
	void RendererGL::endFrame()
	{

	}

	void RendererGL::deleteBuffers(std::vector<uint>& buffVec)
	{
		std::for_each(begin(buffVec),end(buffVec),[](uint& n){
			glDeleteBuffers(1,&n);
		});
	}


	void RendererGL::setRenderTarget(uint fboIdx)
	{
		if(fboIdx>=0 && fboIdx < (int)m_rtVec.size())
		{
			RenderTarget rt = m_rtVec[fboIdx];
			glBindFramebuffer(GL_FRAMEBUFFER, rt.rtIdx);
			//glBindTexture(GL_TEXTURE_2D,rt.texIdx);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glViewport(0,0,rt.w,rt.h);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);//default one
			glViewport(0,0,m_pRendererInfo->width,m_pRendererInfo->height);
		}
	}
	void RendererGL::createRenderTarget(uint& idxOfRT, uint& idxOfTex/*shader resource, gl texture*/,uint format,uint w, uint h)
	{
		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		GLuint FramebufferName = 0;
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		GLuint renderTargetTexture;
		glGenTextures(1, &renderTargetTexture);
		glBindTexture(GL_TEXTURE_2D, renderTargetTexture);

		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, w, h, 0,format, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/*
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, renderTargetTexture, 0);

		// create a renderbuffer object to store depth info

		GLuint rboId;
		glGenRenderbuffers(1, &rboId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// attach the renderbuffer to depth attachment point
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);


		/*
		// Create the depth buffer
		GLuint mDepth;
		glGenTextures(1, &mDepth);
		glBindTexture(GL_TEXTURE_2D, mDepth);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepth, 0);
		*/

		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			myassert(0);

		m_texVec.push_back(renderTargetTexture);
		idxOfTex = m_texVec.size()-1;
		//m_texVec.push_back(mDepth);//depth
		m_rtVec.push_back(RenderTarget(w,h, FramebufferName,m_texVec.size()-1));

		idxOfRT = m_rtVec.size()-1;
	}

	void RendererGL::deleteRenderTarget(int fboIdx)
	{
		if(fboIdx>=0 && fboIdx < (int)m_rtVec.size())
		{
			RenderTarget rt = m_rtVec[fboIdx];
			glDeleteFramebuffers(GL_FRAMEBUFFER, (const GLuint*)&rt.rtIdx);
			m_rtVec.erase(m_rtVec.begin() + fboIdx);
		}
		else if(fboIdx == -1)
		{
			for(auto it = m_rtVec.begin(); it != m_rtVec.end(); ++it)
			{
				RenderTarget rt = *it;
				glDeleteFramebuffers(GL_FRAMEBUFFER, (const GLuint*)&rt.rtIdx);
			}

			m_rtVec.clear();
		}
	}

	RenderTarget* RendererGL::getRenderTarget(int fboidx)
	{
		if(fboidx>=0 && fboidx < (int)m_rtVec.size())
			return &m_rtVec[fboidx];
		else
			return nullptr;
	}

 
	NBE_API RenderSystem* __cdecl createGL(RenderInfo* ri, HINSTANCE h)
	{
		NBEWindow* win = new NBEWindow(ri, h);//delete when release renderer, they are 1:1 now
		return new RendererGL(ri, win);
	}
 
}