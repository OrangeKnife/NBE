#pragma once
#define NOMINMAX
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
#include "Input.h"

#include "NBETimer.h"
#include "RenderInfo.h"
#include "Config.h"
#include "Quaternion.h"

#include "Node.h" 
#include "NBEWindow.h"


#include "Mouse.h"
#include "KeyBoard.h"

#include "EventHub.h"
#include "CustomEvent.h"
#include "Mesh.h"

using namespace NBE;
namespace NBEANIMATOIN
{
	class Game
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

		void initCamera(float asp);
		void initInput();

		void updateFrame();
		void drawBatch(Batch* bat, Matrix4f& worldMat);
		void drawObj(RenderObject* obj, Matrix4f& transform);


		void drawStaticMeshBatch(const NBESTATICMESH::Batch* bat, Matrix4f& worldMat);
		void drawStaticMesh(const NBESTATICMESH::StaticMeshData* data, Matrix4f& transform);
		 
		void handleMovementEvent(NEvent_Key* eventData, void* p);
		
		 
 
 
		void exit();


	private: 
		Config cfg;
		//HINSTANCE m_rendererDLL;
		std::shared_ptr<RenderSystem> m_pRenderer;
		std::vector<RenderObject*> m_renderObjVec;
		RenderInfo* m_rdInfo;

		ShaderManager* shaderMgr;
		TextureManager* textureMgr;

		std::shared_ptr<Camera> m_pCamera;//main camera
		//timing
		clocks m_currentTime;
		clocks m_nextUpdateRenderingTime;
		clocks logicUpdateRate;
		double logicUpdateSecondsPerFrame;
		NBETimer m_timer;
	
		GAMESTATE currentState;
		Node* root; 
		std::list<Node*> m_renderQueue;

		uint generalShader;
	};
}