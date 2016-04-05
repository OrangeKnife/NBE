#pragma once
#pragma comment (lib,"NBE.lib")
#pragma comment (lib,"RendererGL.lib")
#pragma comment (lib,"RenderSystemDX.lib")
#define NOMINMAX
#include <crtdbg.h>
#include "RenderSystem.h"
#include "Mouse.h"
#include "KeyBoard.h"
#include "NBEWindow.h"

#include "File.h"
#include "Mesh.h"

#include "Shader.h"

#include "RenderObject.h"
#include "node.h"
#include <string>

#include "TextureManager.h"
#include "ShaderManager.h"
#include "DataTypes.h"


#include "Astar.h"
#include "Config.h"
#include "Camera.h"
#include "NBETimer.h"

#include "EventHub.h"
#include "CustomEvent.h"


using namespace NBE;
using namespace BT;

class BehaviorTreeApp
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

	BehaviorTreeApp(HINSTANCE h);
	~BehaviorTreeApp();
	RenderInfo* loadRenderInfo(const String& cfgname);
	void updateFrame();
	//////////////////////////////////////////////////////////////////////////
	void initTex();
	void initBots(AstarMap* pMap);
	void loadMaps();
	void initCamera(float asp);
	void initInput();
	void run();
	void exit();
	void handleMovementEvent(NEvent_Key* eventData, void* p);
private:

	//std::vector<AIBot*> bots;
	Config cfg;
	RenderInfo* m_rdInfo;
	std::shared_ptr<Camera> m_pCamera;//main camera
	std::shared_ptr<RenderSystem> m_pRenderer;
	GAMESTATE currentGameState;

	ShaderManager* shaderMgr;
	TextureManager* textureMgr;
	Node* root;

	//timing
	clocks m_startFrameTime;
	clocks m_nextUpdateTime;
	NBETimer m_timer;

	///

	uint m_shaderID;
	std::vector<AstarMap*> m_maps;
	uint currentMapIdx;
	uint m_combinedTex;
};

