#include "BehaviorTree.h"
#include "RendererGL.h"
#include "RendererDX11.h"
#define TotalMaps 1
#define Map_MaxWidth 1024
#define Map_MaxHeight 1024

#define ADDNODE(NDTYPE) oneMap->nodesList.push_back(AstarMapNode(NDTYPE,j,h,w*h+j));

#include <math.h>
#include "Sequence.h"
#include "Leaf.h"
#include "Inverter.h"
using namespace NBE;

RenderInfo* BehaviorTreeApp::loadRenderInfo(const String& cfgname)
{
	cfg.loadConfig(cfgname);
	//cfg.load(iniName);
	RenderInfo* rdinfo = new RenderInfo();
	rdinfo->height = TypeCast::stringToInt(cfg.getInfo("height"));
	rdinfo->width = TypeCast::stringToInt(cfg.getInfo("width"));
	rdinfo->fullScreen = TypeCast::charToBool(cfg.getInfo("fullScreen").c_str());
	string str = cfg.getInfo("renderSystem");
	if (str == string("GL"))
	{
		rdinfo->type = 0;
	}
	else if (str == string("DX11"))
	{
		rdinfo->type = 1;
	}
	//get the wchar_t str to initialize window
	str = cfg.getInfo("title");
	TCHAR* t_str = TypeCast::charToTCHAR(str.c_str());
	size_t t_sz = (str.size() + 1) * sizeof(TCHAR);
	memcpy_s(rdinfo->title, t_sz, t_str, t_sz);
	delete t_str;

	str = cfg.getInfo("className");
	t_str = TypeCast::charToTCHAR(str.c_str());
	t_sz = (str.size() + 1) * sizeof(TCHAR);
	memcpy_s(rdinfo->className, t_sz, t_str, t_sz);
	delete t_str;
	m_rdInfo = rdinfo;
	return rdinfo;
}

BehaviorTreeApp::BehaviorTreeApp(HINSTANCE h) :currentMapIdx(0),
m_timer(NBETimer()),
m_nextUpdateTime(0),
m_startFrameTime(0),
root(nullptr)
{
	RenderInfo* rdinfo = loadRenderInfo(TEXT("config.ini"));

	switch (rdinfo->type)
	{
	case 0:
		m_pRenderer.reset(createGL(rdinfo, h));
		break;
	case 1:
		m_pRenderer.reset(createDX11(rdinfo, h));
		break;
	}

	textureMgr = TextureManager::getInstancePtr();
	textureMgr->initialize(m_pRenderer.get());
	textureMgr->LoadFromFile(TEXT("default-alpha.png"), TEXT("Default"));
	shaderMgr = ShaderManager::getInstancePtr();
	shaderMgr->initialize(m_pRenderer.get());



	root = new Node("root", vec3f(), Matrix4f::Identity());

	currentGameState = INGAME;



	initCamera(static_cast<float>(rdinfo->width) / rdinfo->height);

	initInput();

	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key('W', NEvent_Key::KEY_DOWN), this, (void*)('W'));
	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key('A', NEvent_Key::KEY_DOWN), this, (void*)('A'));
	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key('S', NEvent_Key::KEY_DOWN), this, (void*)('S'));
	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key('D', NEvent_Key::KEY_DOWN), this, (void*)('D'));
	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key('C', NEvent_Key::KEY_DOWN), this, (void*)('C'));
	ADDCLASSCALLBACK(NEvent_Key, BehaviorTreeApp, handleMovementEvent, NEvent_Key(VK_SPACE, NEvent_Key::KEY_DOWN), this, (void*)(VK_SPACE));


}

void BehaviorTreeApp::handleMovementEvent(NEvent_Key* eventData, void* p)
{
	m_pCamera->handleKeyDown(int(p));
}

void BehaviorTreeApp::initCamera(float asp) {
	//camera
	m_pCamera.reset(new Camera("cam1"));
	auto cameraNode = new Node("cameraNode1");
	cameraNode->attachObject(m_pCamera.get());

	m_pCamera->setLocalRotation(vec3f(-PI / 2, 0, 0));
	m_pCamera->setPosition(vec3f(20, 40, 6));
	m_pCamera->updateViewMatrix();
	m_pCamera->setProjection(45.0f, asp, 0.1f, 10000.f);
	if (root)
		root->addAChindNode(cameraNode);

	m_pCamera->applyImpulse(vec3f());
}

void BehaviorTreeApp::initInput() {
	//control
	auto mouse = new Mouse();
	auto window_hwnd = m_pRenderer->getWindow()->getHWND();
	mouse->init(window_hwnd);
	mouse->calculateCenter(window_hwnd);

	new KeyBoard();
}

void BehaviorTreeApp::updateFrame()
{
	m_startFrameTime = m_timer.GetSystemClocks();
	m_timer.PreciseWaitUntill(m_nextUpdateTime);
	float m_deltaTime = static_cast<float>( m_timer.clocktoSeconds(m_timer.GetSystemClocks() - m_startFrameTime) );
	m_nextUpdateTime = m_timer.GetSystemClocks() + m_timer.getDesireClocsPerFrame();

	if (m_pRenderer->isActive())
	{
		for (size_t i = 0; i < Input::getTotalInputNum(); ++i)
			Input::getInput(i)->update();
	}

	m_pCamera->updateViewMatrix();

	float color[4] = { 0.5f,0.6f,0.7f,0.1f };
	m_pRenderer->clearColorBuffer(color);
	m_pRenderer->clearDepthBuffer(1.0);

	m_pRenderer->applyShader(m_shaderID);

	m_pRenderer->applyMatrix(m_shaderID, "modelMat", Matrix4f::Identity());
	m_pRenderer->applyMatrix(m_shaderID, "viewMat", m_pCamera->getViewMatrix());
	m_pRenderer->applyMatrix(m_shaderID, "projMat", m_pCamera->getProjection());
	
	m_pRenderer->applyTexture(m_shaderID, "diftex", m_combinedTex, 0);

	static auto pShader = ShaderManager::getInstancePtr()->getShaderByIdx(m_shaderID);
	auto currentMap = m_maps[currentMapIdx];

	//start!
	static uint fcount = 0;
	fcount++;
	for (int i = 0; i < (int)currentMap->bots.size(); ++i)
	{
		if (currentMap->bots[i]->isActive())
		{
			currentMap->bots[i]->Tick(m_deltaTime);
		}

		//Update Moving
		if (fcount % 10 == 0)
		{
			currentMap->bots[i]->updateLocation();
			//10 frames per move
			int j = currentMap->bots[i]->pathNodeIndex--;
			if (j >= 0)
			{
				int ndidx = currentMap->bots[i]->getPathNode(j);
				currentMap->bots[i]->setCurrentNode(ndidx);

				currentMap->bots[i]->savedMapNode_Type = currentMap->nodesList[ndidx].nodeType;

				if (j > 0)
				{
					currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx], currentMap->bots[i]->display);
					m_pRenderer->updateVBO(currentMap->renderObj->vbo_id, currentMap->renderObj->vbo, 0, currentMap->renderObj->VertexNum, sizeof(PT_Vertex));
				}
				else
				{
					currentMap->bots[i]->forgetPath();
				}
			}
			else
			{
				int ndidx = currentMap->bots[i]->getCurrentNode();
				currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx], currentMap->bots[i]->display);
				m_pRenderer->updateVBO(currentMap->renderObj->vbo_id, currentMap->renderObj->vbo, 0, currentMap->renderObj->VertexNum, sizeof(PT_Vertex));
			}
		}

	}

	if (fcount % 10 == 0)
	{
		for (int i = 0; i < (int)currentMap->bots.size(); ++i)
		{
			int ndidx = currentMap->bots[i]->getCurrentNode();
			currentMap->nodesList[ndidx].nodeType = currentMap->bots[i]->savedMapNode_Type;
			currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx]);//,botVec[i]->display );
		}
	}


	m_pRenderer->bindVertexBuffer(currentMap->renderObj->vbo_id, sizeof(PT_Vertex), 0, pShader);//DX11:auto set the input layout
	m_pRenderer->bindIndexBuffer(currentMap->renderObj->ibo_id);
	m_pRenderer->drawIndex(4, currentMap->renderObj->IndexNum, 0, 0);//4 == triangles
	
	m_pRenderer->swapBuff(false);

}

BehaviorTreeApp::~BehaviorTreeApp()
{
	//////////////////////////////////////////////////////////////////////////
	//maps for rendering
	for (auto it = m_maps.begin(); it != m_maps.end();++it)
	{
		delete *it;
	}
	m_maps.clear();

	textureMgr->deleteInstance();
	shaderMgr->deleteInstance();

	MeshManager::deleteInstance();

	delete root;

}

void BehaviorTreeApp::run()
{
	//msg
	bool gotMsg;
	MSG  msg;

	::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (m_pRenderer->isActive())
		{
			gotMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) ? true : false;
		}
		else
		{
			gotMsg = ::GetMessage(&msg, NULL, 0, 0) ? true : false;
		}

		if (gotMsg)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			if (currentGameState != EXIT)
				updateFrame();
			else
				break;

		}

	}

}

void BehaviorTreeApp::initTex()
{
	m_combinedTex = TextureManager::getInstancePtr()->LoadFromFile(TEXT("GOAPTex.png"), TEXT("GOAPTEX"))->textureIdx;
}

void BehaviorTreeApp::initBots(AstarMap* pMap)
{
	for (unsigned int i = 0; i < pMap->bots.size(); ++i)
	{
		auto bot = pMap->bots[i];
		bot->init(pMap);
		bot->setBTRoot(new Sequence());

		auto hungerCheck = new Leaf_GetItem(bot, AIStateVariable::Hunger, 2);
		bot->getBTRoot()->AddChild(hungerCheck);

		auto eatBreadSeq = new Sequence();
		auto anInverter = new Inverter();
		eatBreadSeq->AddChild(new Leaf_GetItem(bot, AIStateVariable::Bread, 1));
		eatBreadSeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::TABLE)));
		eatBreadSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Bread, 1));
		eatBreadSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Hunger, 1));
		anInverter->AddChild(eatBreadSeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto eatPizzaSeq = new Sequence();
		anInverter = new Inverter();
		eatPizzaSeq->AddChild(new Leaf_GetItem(bot, AIStateVariable::Pizza, 1));
		eatPizzaSeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::TABLE)));
		eatPizzaSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Pizza, 1));
		eatPizzaSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Hunger, 2));
		anInverter->AddChild(eatPizzaSeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto haveFlourSeq = new Sequence();
		anInverter = new Inverter();
		haveFlourSeq->AddChild(new Leaf_GetItem(bot, AIStateVariable::Flour, 1));
		haveFlourSeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::OVEN)));
		haveFlourSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Flour, 1));
		haveFlourSeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Bread, 1));
		anInverter->AddChild(haveFlourSeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto haveMoneySeq = new Sequence();
		anInverter = new Inverter();
		haveMoneySeq->AddChild(new Leaf_GetItem(bot, AIStateVariable::Money, 2));
		haveMoneySeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::PIZZAHUT)));
		haveMoneySeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Money, 2));
		haveMoneySeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Pizza, 1));
		anInverter->AddChild(haveMoneySeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto haveMoneySeq2 = new Sequence();
		anInverter = new Inverter();
		haveMoneySeq2->AddChild(new Leaf_GetItem(bot, AIStateVariable::Money, 1));
		haveMoneySeq2->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::GENERALSTORE)));
		haveMoneySeq2->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Money, 1));
		haveMoneySeq2->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Flour, 1));
		anInverter->AddChild(haveMoneySeq2);
		bot->getBTRoot()->AddChild(anInverter);

		auto pickupMoneySeq = new Sequence();
		anInverter = new Inverter();
		pickupMoneySeq->AddChild(new Leaf_GetResource(bot, AstarNodeType::MONEY));
		pickupMoneySeq->AddChild(new Leaf_SetResourceTarget(bot));
		pickupMoneySeq->AddChild(new Leaf_MoveToTarget(bot));
		pickupMoneySeq->AddChild(new Leaf_ConsumeResource(bot, AstarNodeType::MONEY));
		pickupMoneySeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Money, 1));
		anInverter->AddChild(pickupMoneySeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto sellFurSeq = new Sequence();
		anInverter = new Inverter();
		sellFurSeq->AddChild(new Leaf_GetItem(bot, AIStateVariable::Fur, 1));
		sellFurSeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::FURTRADER)));
		sellFurSeq->AddChild(new Leaf_ConsumeItem(bot, AIStateVariable::Fur, 1));
		sellFurSeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Money, 1));
		anInverter->AddChild(sellFurSeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto KillWolfSeq = new Sequence();
		anInverter = new Inverter();
		KillWolfSeq->AddChild(new Leaf_GetResource(bot, AstarNodeType::WOLF));
		KillWolfSeq->AddChild(new Leaf_SetResourceTarget(bot));
		KillWolfSeq->AddChild(new Leaf_MoveToTarget(bot));
		KillWolfSeq->AddChild(new Leaf_ConsumeResource(bot,AstarNodeType::WOLF));
		KillWolfSeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Fur, 1));
		anInverter->AddChild(KillWolfSeq);
		bot->getBTRoot()->AddChild(anInverter);

		auto StealMoneySeq = new Sequence();
		anInverter = new Inverter();
		StealMoneySeq->AddChild(new Leaf_MoveTo(bot, pMap->getAstarMapNodeByResourceType(AstarNodeType::BANK)));
		StealMoneySeq->AddChild(new Leaf_CreateItem(bot, AIStateVariable::Money, 1));
		anInverter->AddChild(StealMoneySeq);
		bot->getBTRoot()->AddChild(anInverter);
	}
}

void BehaviorTreeApp::loadMaps()//for drawing
{
	//load public shader
	m_shaderID = shaderMgr->loadShader(TEXT("Shader\\GOAP"), "PT");
	//load all the txt map

	char* mapsname[TotalMaps] = { "simulation.txt" };

	for (int i = 0; i < TotalMaps; ++i)
	{
		std::ifstream* ifs = new std::ifstream("tests\\" + std::string(mapsname[i]));
		char buffer[Map_MaxWidth];
		ifs->getline(buffer, Map_MaxWidth);
		uint w = strlen(buffer);// n
		uint h = 0;
		AstarMap* oneMap = new AstarMap(std::string(mapsname[i]));

		for (;; ++h, ifs->getline(buffer, Map_MaxWidth))
		{
			for (uint j = 0; j < w; ++j)
			{
				/*
				Start (1 NPC each)
				Money
				Wolf
				Oven
				Table (eat)
				Fur Trader
				General Store
				Pizza Hut
				*/
				vec2f texCoordInCombinedTex;
				switch (buffer[j])
				{
				case 'S'://start
					oneMap->nodesList.push_back(AstarMapNode(START, j, h, w*h + j));
					oneMap->startNode.push_back(oneMap->nodesList.size() - 1);
					oneMap->bots.push_back(new AIBot(oneMap->nodesList.size() - 1));
					oneMap->bots.back()->display = NPC1 + oneMap->bots.size() - 1;
					break;
				case 'E'://end
					ADDNODE(END)
						//oneMap->endNode = oneMap->nodesList.size()-1;
						break;
				case '.'://empty
					ADDNODE(EMPTY)
						break;
				case '/'://water
					ADDNODE(SEMIBLOCKER)
						break;
				case '#'://stone
					ADDNODE(BLOCKER)
						break;
				case 'M'://money
					ADDNODE(MONEY)
						oneMap->resource[MONEY]->push_back(Res(w*h + j, true));
					break;
				case 'W'://wolf
					ADDNODE(WOLF)
						oneMap->resource[WOLF]->push_back(Res(w*h + j, true));
					break;
				case 'O'://Oven
					ADDNODE(OVEN)
						oneMap->resource[OVEN]->push_back(Res(w*h + j));
					break;
				case 'T'://table , means eat
					ADDNODE(TABLE)
						oneMap->resource[TABLE]->push_back(Res(w*h + j));
					break;
				case 'F'://Fur Trader
					ADDNODE(FURTRADER)
						oneMap->resource[FURTRADER]->push_back(Res(w*h + j));
					break;
				case 'G'://	General Store
					ADDNODE(GENERALSTORE)
						oneMap->resource[GENERALSTORE]->push_back(Res(w*h + j));
					break;
				case 'B':
					ADDNODE(BANK)
						oneMap->resource[BANK]->push_back(Res(w*h + j));
					break;
				case 'P'://	Pizza Hut						
					ADDNODE(PIZZAHUT)
						oneMap->resource[PIZZAHUT]->push_back(Res(w*h + j));
					break;
				default:
					errorMsg("Map error: can't recognize the character or this line doesn't match the width", "error!");
					break;
				}

				texCoordInCombinedTex = oneMap->getTexCoord(oneMap->nodesList.back().nodeType);

				oneMap->vertices.push_back(PT_Vertex(j, h, texCoordInCombinedTex));
				oneMap->vertices.push_back(PT_Vertex(j + 1, h, texCoordInCombinedTex + vec2f(1, 0)));
				oneMap->vertices.push_back(PT_Vertex(j + 1, h - 1, texCoordInCombinedTex + vec2f(1, 1 / TEXCOUNT)));
				oneMap->vertices.push_back(PT_Vertex(j, h - 1, texCoordInCombinedTex + vec2f(0, 1 / TEXCOUNT)));
			}

			if (ifs->eof())
			{
				ifs->close();
				delete ifs;
				break;
			}

		}



		oneMap->width = w;
		oneMap->height = h + 1;



		oneMap->renderObj.reset(new RenderObject(std::string(mapsname[i])));
		oneMap->renderObj->vbo = new PT_Vertex[oneMap->vertices.size()];
		oneMap->renderObj->VertexNum = oneMap->vertices.size();
		memcpy_s(oneMap->renderObj->vbo, oneMap->renderObj->VertexNum * sizeof(PT_Vertex), &oneMap->vertices.front(), oneMap->renderObj->VertexNum * sizeof(PT_Vertex));



		//vbo,ibo

		for (unsigned int row = 0; row < h + 1; ++row)
		{
			for (unsigned int col = 0; col < w; ++col)
			{
				unsigned int idx = 4 * (row * w + col);
				oneMap->renderObj->ibo.push_back(idx);
				oneMap->renderObj->ibo.push_back(idx + 1);
				oneMap->renderObj->ibo.push_back(idx + 2);

				oneMap->renderObj->ibo.push_back(idx);
				oneMap->renderObj->ibo.push_back(idx + 2);
				oneMap->renderObj->ibo.push_back(idx + 3);
			}
		}

		oneMap->renderObj->IndexNum = oneMap->renderObj->ibo.size();

		oneMap->renderObj.get()->vbo_id = m_pRenderer->createVBO(oneMap->renderObj->vbo, sizeof(PT_Vertex), oneMap->renderObj.get()->VertexNum, m_shaderID, "PT");
		oneMap->renderObj.get()->ibo_id = m_pRenderer->createIBO(oneMap->renderObj->ibo.data(), oneMap->renderObj->IndexNum);


		m_maps.push_back(oneMap);
	}

	initBots(m_maps[currentMapIdx]);
}

void BehaviorTreeApp::exit() {
	currentGameState = EXIT;
}

