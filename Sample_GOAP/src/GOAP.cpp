#include "GOAP.h"
#include "RendererGL.h"
#include "RendererDX11.h"
#define TotalMaps 1
#define Map_MaxWidth 1024
#define Map_MaxHeight 1024

#define ADDNODE(NDTYPE) oneMap->nodesList.push_back(AstarMapNode(NDTYPE,j,h,w*h+j));

#define STEPS 1
#include <math.h>
namespace NBE
{
	RenderInfo* GOAPapp::loadRenderInfo(const String& cfgname)
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

	GOAPapp::GOAPapp(HINSTANCE h) :currentMapIdx(0),
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



		root = new Node(TEXT("root"), vec3f(), Matrix4f::Identity());

		currentState = INGAME;



		initCamera(static_cast<float>(rdinfo->width) / rdinfo->height);

		initInput();

		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key('W', NEvent_Key::KEY_DOWN), this, (void*)('W'));
		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key('A', NEvent_Key::KEY_DOWN), this, (void*)('A'));
		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key('S', NEvent_Key::KEY_DOWN), this, (void*)('S'));
		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key('D', NEvent_Key::KEY_DOWN), this, (void*)('D'));
		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key('C', NEvent_Key::KEY_DOWN), this, (void*)('C'));
		ADDCLASSCALLBACK(NEvent_Key, GOAPapp, handleMovementEvent, NEvent_Key(VK_SPACE, NEvent_Key::KEY_DOWN), this, (void*)(VK_SPACE));


	}

	void GOAPapp::handleMovementEvent(NEvent_Key* eventData, void* p)
	{
		m_pCamera->handleKeyDown(int(p));
	}

	void GOAPapp::initCamera(float asp) {
		//camera
		m_pCamera.reset(new Camera(TEXT("cam1")));
		auto cameraNode = new Node(TEXT("cameraNode1"));
		cameraNode->attachObject(m_pCamera.get());

		m_pCamera->setLocalRotation(vec3f(-PI / 2, 0, 0));
		m_pCamera->setPosition(vec3f(20, 40, 6));
		m_pCamera->updateViewMatrix();
		m_pCamera->setProjection(45.0f, asp, 0.1f, 10000.f);
		if (root)
			root->addAChindNode(cameraNode);

		m_pCamera->applyImpulse(vec3f());
	}

	void GOAPapp::initInput() {
		//control
		auto mouse = new Mouse();
		auto window_hwnd = m_pRenderer->getWindow()->getHWND();
		mouse->init(window_hwnd);
		mouse->calculateCenter(window_hwnd);

		new KeyBoard();
	}


	void GOAPapp::updateFrame()
	{
		m_startFrameTime = m_timer.GetSystemClocks();
		m_timer.PreciseWaitUntill(m_nextUpdateTime);
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

		//static auto defTex = texArray[3];

		m_pRenderer->applyTexture(m_shaderID, "diftex", m_combinedTex, 0);

		static auto pShader = ShaderManager::getInstancePtr()->getShaderByIdx(m_shaderID);








		auto currentMap = m_maps[currentMapIdx];


		//start!
		static uint fcount = 0;
		fcount++;
		for (int i = 0; i < (int)botVec.size(); ++i)
		{
			if (!botVec[i]->foundPlan)
			{
				botVec[i]->updateExistenceByMap();
				botVec[i]->foundPlan = botVec[i]->doPlanfinding();

			}

			if (!botVec[i]->foundPathToTargetNode && botVec[i]->currentPlan.size() > 0)
			{
				uint end;
				if (botVec[i]->currentPlan.back()->needNodeType >= 0)
					end = currentMap->getAstarMapNodeByResourceType(botVec[i]->currentPlan.back()->needNodeType);
				else
					end = botVec[i]->currentMapNode;

				botVec[i]->foundPathToTargetNode = doPathFinding(currentMap, botVec[i], end);
				if (botVec[i]->foundPathToTargetNode)
				{
					botVec[i]->posInPathNode = botVec[i]->pathNodes.size() - 1;
					botVec[i]->goToNodeAction = botVec[i]->currentPlan.back();
					botVec[i]->currentPlan.pop_back();
				}
				else
				{
					clearAllBotsInfo();
				}
			}

			//rendering!!!
			if (fcount == 20 * 500)
			{
				fcount = 0;

				float rdIdx = ((float)rand()) / RAND_MAX * currentMap->nodesList.size();
				AstarMapNode& nd = currentMap->nodesList[(int)rdIdx];
				if (nd.nodeType == EMPTY)
				{
					nd.nodeType = WOLF;
					currentMap->resource[WOLF]->push_back(Res((int)rdIdx, true));
					currentMap->changeNodeTexInVBO(currentMap->nodesList[(int)rdIdx]);
					clearAllBotsInfo();
				}
			}





			if (fcount % 10 == 0)
			{
				int j = botVec[i]->posInPathNode--;
				if (j >= 0)
				{
					uint ndidx = botVec[i]->pathNodes[j];
					botVec[i]->currentMapNode = ndidx;

					botVec[i]->currentMapNode_Type = currentMap->nodesList[ndidx].nodeType;

					//botVec[i]->pathNodes.pop_back();

					if (j > 0)
					{
						currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx], botVec[i]->display);
						m_pRenderer->updateVBO(currentMap->renderObj->vbo_id, currentMap->renderObj->vbo, 0, currentMap->renderObj->VertexNum, sizeof(PT_Vertex));

					}
					else
					{
						botVec[i]->current->act(botVec[i]->goToNodeAction);//change Aibot aistate
						botVec[i]->goToNodeAction = NULL;//action done
						botVec[i]->foundPathToTargetNode = false;//need to find next target node

						if (botVec[i]->currentMapNode_Type == MONEY || botVec[i]->currentMapNode_Type == WOLF)
						{
							currentMap->removeResource(botVec[i]->currentMapNode_Type, ndidx);
							currentMap->nodesList[ndidx].nodeType = EMPTY;//set current node type = EMPTY, wipe it 
							clearAllBotsInfo();
							// map changed, re-find plan and path

						}

					}
				}
				else
				{
					int ndidx = botVec[i]->currentMapNode;

					currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx], botVec[i]->display);

					m_pRenderer->updateVBO(currentMap->renderObj->vbo_id, currentMap->renderObj->vbo, 0, currentMap->renderObj->VertexNum, sizeof(PT_Vertex));
				}
			}


		}


		if (fcount % 10 == 0)
		{
			for (int i = 0; i < (int)botVec.size(); ++i)
			{
				int ndidx = botVec[i]->currentMapNode;
				currentMap->nodesList[ndidx].nodeType = botVec[i]->currentMapNode_Type;
				currentMap->changeNodeTexInVBO(currentMap->nodesList[ndidx]);//,botVec[i]->display );
			}
		}


		m_pRenderer->bindVertexBuffer(currentMap->renderObj->vbo_id, sizeof(PT_Vertex), 0, pShader);//DX11:auto set the input layout
		m_pRenderer->bindIndexBuffer(currentMap->renderObj->ibo_id);



		m_pRenderer->drawIndex(4, currentMap->renderObj->IndexNum, 0, 0);//4 == triangles


		//DX need to reset shader resource = NULL when set render target
		//m_pRenderer->applyTexture(m_shaderID, "diftex", NULL,-1);
		//m_pRenderer->drawIndex(0,0,0,0);//apply the shader cleaning

		m_pRenderer->swapBuff(false);

	}



	GOAPapp::~GOAPapp()
	{

		for (auto it = botVec.begin(); it != botVec.end();)
		{
			delete *it;
			it = botVec.erase(it);
		}


		//////////////////////////////////////////////////////////////////////////
		//maps for rendering
		for (auto it = m_maps.begin(); it != m_maps.end();)
		{
			delete *it;
			it = m_maps.erase(it);
		}

		textureMgr->deleteInstance();
		shaderMgr->deleteInstance();

		MeshManager::deleteInstance();

		delete root;

	}

	void GOAPapp::run()
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
				if (currentState != EXIT)
					updateFrame();
				else
					break;

			}

		}

	}

	void GOAPapp::initTex()
	{
		m_combinedTex = TextureManager::getInstancePtr()->LoadFromFile(TEXT("GOAPTex.png"), TEXT("GOAPTEX"))->textureIdx;
	}

	void GOAPapp::initBots(AstarMap* pMap)
	{
		for (unsigned int i = 0; i < botVec.size(); ++i)
		{
			botVec[i]->defineStartEnd(pMap);
		}

		for (int i = 0; i < (int)botVec.size(); ++i)
		{
			if (!botVec[i]->foundPlan)
			{
				botVec[i]->updateExistenceByMap();
				botVec[i]->foundPlan = botVec[i]->doPlanfinding(0);

			}
		}
	}

	void GOAPapp::loadMaps()//for drawing
	{
		//load public shader
		m_shaderID = shaderMgr->loadShader(TEXT("Shader\\GOAP"), "PT");



		//load all the txt map

		wchar_t* mapsname[TotalMaps] = { L"simulation.txt" };

		for (int i = 0; i < TotalMaps; ++i)
		{
			std::ifstream* ifs = new std::ifstream(L"tests\\" + std::wstring(mapsname[i]));
			char buffer[Map_MaxWidth];
			ifs->getline(buffer, Map_MaxWidth);
			uint w = strlen(buffer);// n
			uint h = 0;
			AstarMap* oneMap = new AstarMap(std::wstring(mapsname[i]));

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
						botVec.push_back(new AIbot(oneMap->nodesList.size() - 1));
						botVec.back()->display = NPC1 + botVec.size() - 1;
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



			oneMap->renderObj.reset(new RenderObject(String(mapsname[i])));
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

			m_pRenderer->createVBO(oneMap->renderObj.get(), sizeof(PT_Vertex), m_shaderID, "PT");
			m_pRenderer->createIBO(oneMap->renderObj.get());


			m_maps.push_back(oneMap);
		}

		initBots(m_maps[currentMapIdx]);

	}




	bool GOAPapp::doPathFinding(AstarMap* pMap, AIbot* bot, uint end, int stepsPerFrame)
	{
		uint stepsCount = 0;
		bot->actionStartMapNode = bot->currentMapNode;
		uint current = bot->currentMapNode;

		while (current != end)
		{
			AstarMapNode& curNd = pMap->nodesList[current];
			//curNd.nodeType = CURNODE;
			//printMapOnConsole(pMap);
			pMap->addNeighorsToOpenList(current, end);
			//printMapOnConsole(pMap);
			pMap->removeNodeInOpenList(current);
			curNd.nodeCode = CLOSED;
			//pMap->changeNodeTexInVBO(curNd);
			pMap->closed.push_back(current);
			//printMapOnConsole(pMap);
			if (pMap->open.size() > 0)
			{
				current = pMap->getLowestCostNodeInOpenList(pMap->open, pMap);
			}
			else
			{
				//std::cout<<"open list empty!\n";
				//errorMsg("There is no way to End","Error");
				return false;
				//printMapOnConsole(pMap);
			}

			++stepsCount;
			if (stepsPerFrame > 0 && stepsCount == stepsPerFrame)
			{
				return false; //jump out, wait for next frame to find the path
			}

		}


		bot->pathNodes.clear();
		//find the path
		uint back = end;
		bot->pathNodes.push_back(end);
		for (; back != bot->actionStartMapNode;)
		{
			//pMap->nodesList[back].nodeType = PATH;

			back = pMap->nodesList[back].parent;
			bot->pathNodes.push_back(back);
			//pMap->changeNodeTexInVBO(pMap->nodesList[back],bot->display);

		}
		//updateVBO(bot->botInMap->renderObj->vbo_id,bot->botInMap->renderObj->vbo,0,bot->botInMap->renderObj->VertexNum,sizeof(PT_Vertex));
		for (unsigned int i = 0; i < pMap->open.size(); ++i)
		{
			pMap->nodesList[pMap->open[i]].nodeCode = EMPTY;
		}
		for (unsigned int i = 0; i < pMap->closed.size(); ++i)
		{
			pMap->nodesList[pMap->closed[i]].nodeCode = EMPTY;
		}
		pMap->open.clear();
		pMap->closed.clear();
		//printOutMap(pMap);
		//printMapOnConsole(pMap);
		return true;
	}

	void GOAPapp::clearAllBotsInfo()
	{


		for (unsigned int i = 0; i < botVec.size(); ++i)
		{
			//botVec[i]->currentMapNode_Type = EMPTY;
			if (botVec[i]->currentMapNode_Type >= MONEY && botVec[i]->currentMapNode_Type <= WOLF)
				botVec[i]->currentMapNode_Type = EMPTY;


			botVec[i]->foundPlan = false;
			botVec[i]->foundPathToTargetNode = false;
			botVec[i]->currentPlan.clear();
			botVec[i]->startState = botVec[i]->current;//new AIState(*botVec[i]->current);//always keep the current state to start state, 

			for (auto it = botVec[i]->open.begin(); it != botVec[i]->open.end(); ++it)
			{
				if (botVec[i]->current != *it)
				{
					delete (*it);
				}
			}

			botVec[i]->open.clear();

			for (auto it = botVec[i]->closed.begin(); it != botVec[i]->closed.end(); ++it)
			{
				if (botVec[i]->current != *it)
				{
					delete (*it);
				}
			}

			botVec[i]->closed.clear();

			botVec[i]->pathNodes.clear();
			botVec[i]->posInPathNode = -1;
		}
	}


	void GOAPapp::exit() {
		currentState = EXIT;
	}

}