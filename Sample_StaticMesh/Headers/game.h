#pragma once

#pragma comment (lib,"NBE.lib")
#pragma comment (lib,"RendererGL.lib")
#pragma comment (lib,"RenderSystemDX.lib")
#include <vector>
#include <list>

#include <crtdbg.h> 
#include "NBE.h"
#include "Exception.h"
#include "TypeCast.h"

#include "RenderObject.h"
#include "RenderSystem.h"
 
#include "Singleton.h"
#include "ShaderManager.h"

#include "Texture.h"
#include "RenderSystem.h"
#include "Singleton.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Listener.h"
#include "Input.h"

#include "NBETimer.h"
#include "RenderInfo.h"
#include "Config.h"
#include "Quaternion.h"

#include "Node.h" 
#include "NBEWindow.h"

#ifdef _DEBUG
#include <sstream>
#define trace( text )\
{\
	std::wstringstream stringBuilder;\
	stringBuilder << text << std::endl;\
	OutputDebugString(stringBuilder.str().c_str());\
}
#else
#define trace( text )
#endif

#include "Mouse.h"
#include "KeyBoard.h"


using namespace NBE;
namespace NBEANIMATOIN
{
	class Game:public Listener
	{
	protected:
		enum GAMESTATE
		{
			START = 0,
			INGAME,
			EXIT,
			TOTALSTATES
		};
	public:
		Game(HINSTANCE h);
		~Game();
		RenderInfo* loadRenderInfo(const String& cfgname);
		void run();
		void initData();
		void initCamera(float asp);
		void initInput();

		void updateFrame();
		void drawBatch(Batch* bat, Matrix4f& worldMat);
		void drawObj(RenderObject* obj, Matrix4f& transform);
		 
		void handleEvent(Event* e);
		void handleKeyDown(int key);
		void handleKeyRelease(int key);
		 
 
 
		void exit();
	private: 
		Config cfg;
		//HINSTANCE m_rendererDLL;
		std::shared_ptr<RenderSystem> m_pRenderer;
		std::vector<RenderObject*> m_renderObjVec;
		RenderInfo* m_rdInfo;

		Mouse* mouse;
		KeyBoard* keyboard;
		ShaderManager* shaderMgr;
		TextureManager* textureMgr;

		std::shared_ptr<Camera> m_pCamera;
		std::vector<Input*> m_inputVec;
		//timing
		clocks m_startFrameTime;
		clocks m_nextUpdateTime;
		NBETimer m_timer;
	
		GAMESTATE currentState;
		Node* root; 
		std::list<Node*> m_renderQueue;

		uint generalShader;
	};
}