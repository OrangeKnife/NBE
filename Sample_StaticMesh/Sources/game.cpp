
#include "game.h"


#include "Vertex.h"


#include "Vector3.h"
#include "Vector2.h"
#include "Listener.h"
#include "Input.h"


#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"



 

#include "RenderObject.h"   
#include "RendererGL.h"
#include "RendererDX11.h"
#include "Mesh.h"
 
using namespace NBE;
namespace NBEANIMATOIN
{
	RenderInfo* Game::loadRenderInfo(const String& cfgname)
	{
		cfg.loadConfig(cfgname);
		//cfg.load(iniName);
		RenderInfo* rdinfo = new RenderInfo();
		rdinfo->height = TypeCast::stringToInt( cfg.getInfo("height") );
		rdinfo->width = TypeCast::stringToInt( cfg.getInfo("width") );
		rdinfo->fullScreen = TypeCast::charToBool( cfg.getInfo("fullScreen").c_str() );
		string str = cfg.getInfo("renderSystem");
		if(str == string("GL"))
		{
			rdinfo->type = 0;
		}
		else if (str == string("DX11"))
		{
			rdinfo->type = 1;
		}
		//get the wchar_t str to initialize window
		str = cfg.getInfo("title");
		wchar_t* w_str = TypeCast::charToWchar(str.c_str());
		size_t w_len = str.size();
		wcscpy_s(rdinfo->title,w_len+1,w_str);
		delete w_str;

		str = cfg.getInfo("className");
		w_str = TypeCast::charToWchar(str.c_str());
		w_len = str.size();
		wcscpy_s(rdinfo->className,w_len+1,w_str);
		delete w_str;
		m_rdInfo = rdinfo;
		return rdinfo;
	}

	Game::Game(HINSTANCE h)
		:m_timer(NBETimer()),
		m_nextUpdateTime(0),
		m_startFrameTime(0),
		root(nullptr)
	{

		RenderInfo* rdinfo = loadRenderInfo(L"config.ini");

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
		textureMgr->LoadFromFile(L"default-alpha.png",L"Default");
		shaderMgr = ShaderManager::getInstancePtr();
		shaderMgr->initialize(m_pRenderer.get());


 
		root = new Node(L"root",vec3f(),Matrix4f::Identity());

		currentState = INGAME;

	 

		initCamera( static_cast<float>(rdinfo->width)/rdinfo->height );

		initInput();


  
		generalShader = shaderMgr->loadShader(L"Shader\\general","PTNC");
 

		//test
		/*
		RenderObject* map = new RenderObject(String(L"Square"));
		map->vbo = (Vertex*) new PTNC_Vertex[4];
		PTNC_Vertex* tempArray = new PTNC_Vertex[4];
		tempArray[0] = (PTNC_Vertex(vec3f(0,0,0),vec2f(0,0),vec3f(0,1,0),vec4f(1,1,1,1)));
		tempArray[1] = (PTNC_Vertex(vec3f(100,0,0),vec2f(1,0),vec3f(0,1,0),vec4f(1,1,1,1)));
		tempArray[2] = (PTNC_Vertex(vec3f(100,100,0),vec2f(1,1),vec3f(0,1,0),vec4f(1,1,1,1)));
		tempArray[3] = (PTNC_Vertex(vec3f(0,100,0),vec2f(0,1),vec3f(0,1,0),vec4f(1,1,1,1)));

		memcpy_s(map->vbo,sizeof(PTNC_Vertex) * 4 , tempArray, sizeof(PTNC_Vertex)*4);
		map->VertexNum = 4;
		map->IndexNum = 6;

		delete[] tempArray;

		map->ibo.push_back(0);map->ibo.push_back(1);map->ibo.push_back(2);
		map->ibo.push_back(2);map->ibo.push_back(3);map->ibo.push_back(0);

		TexMap* texbackground = new TexMap();
		texbackground->texId = 0;// textureMgr->LoadFromFile(L"scenes\\darktile.jpg", L"BGTex")->textureIdx;
		texbackground->texMapSlot = Diffuse;

		string ss("BGMat");
		Material* mtl = new Material(ss);
		 
		mtl->texMapVec->push_back(texbackground);
		mtl->shader = generalShader;
		Batch* b = new Batch(mtl, 0, map->ibo.size());//this is the drawing info

		map->addBatch(b);
		m_pRenderer->createVBO(map,sizeof(PTNC_Vertex),generalShader,"PTNC");
		m_pRenderer->createIBO(map);
		root->attachObject(map);
 */
		
		Mesh* virgil = MeshManager::getInstancePtr()->loadMeshFromFile(L"scenes\\virgil.n3d");
		for(uint i = 0; i < virgil->objectVec->size(); ++i)
		{
			m_pRenderer->createVBO(virgil->objectVec->at(i),sizeof(PTNC_Vertex),generalShader,"PTNC");
			m_pRenderer->createIBO(virgil->objectVec->at(i));
			root->addAChindNode(virgil->objectVec->at(i)->meshObjecNode);//attachObject(virgil->objectVec->at(i));
			 
		}
		 
	}

	Game::~Game()
	{
		textureMgr->deleteInstance();
		shaderMgr->deleteInstance();

		MeshManager::deleteInstance();

		for(auto it = m_inputVec.begin(); it!=m_inputVec.end();)
		{
			delete *it;
			it = m_inputVec.erase(it);
		}
		delete root;  
	}

	void Game::run()
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
				if(currentState != EXIT)
					updateFrame();
				else
					break;

			}

		}

	}

	void Game::updateFrame()
	{
		m_startFrameTime = m_timer.GetSystemClocks();
		m_timer.PreciseWaitUntill(m_nextUpdateTime);
		m_nextUpdateTime = m_timer.GetSystemClocks()  + m_timer.getDesireColocsPerFrame();


		if (m_pRenderer->isActive())
		{
			std::for_each(begin(m_inputVec),end(m_inputVec),[&](Input* input){
				input->update();//read events
				input->handleAllEvents();
			});
		}

		m_pCamera->updateViewMatrix();

		float color[4] = {0.5f,0.6f,0.7f,0.1f}; 
		m_pRenderer->clearColorBuffer(color);
		m_pRenderer->clearDepthBuffer(1.0);

		
		m_renderQueue.push_back(root);


		while(m_renderQueue.size() > 0)
		{
			Node* current = m_renderQueue.front();
			Node::ChildNodeMap children = current->getChildren();
			for(auto it = children.begin(); it != children.end(); ++it)
			{
				m_renderQueue.push_back(it->second);
			}

			Node::ObjectMap objs = current->getObjects();
			for(auto it = objs.begin(); it != objs.end(); ++it)
			{
				if((*it).second->isRenderable())
					drawObj(reinterpret_cast<RenderObject*>(it->second),current->getWorldTM());
			}

			m_renderQueue.pop_front();
		}

		//m_pRenderer->applyTexture(generalShader, "diftex", NULL, -1);
		//m_pRenderer->drawIndex(0, 0, 0, 0);


		m_pRenderer->swapBuff(true);
	}

	 
	 

	void Game::handleEvent(Event* e)
	{
		 
	}

	void Game::handleKeyDown(int key)
	{
		/*
		vec3f impuse;
		switch(key)
		{
		case 'W':impuse.z -=  .10f;break;
		case 'S':impuse.z +=  .10f;break;
		case 'A':impuse.x -=  .10f;break;
		case 'D':impuse.x +=  .10f;break;
		case VK_SPACE:impuse.y += 0.1f;break;
		case 'C':impuse.y -= 0.1f;break;

		}
		m_pCamera->applyImpulse(impuse);
		*/

	}

	void Game::handleKeyRelease(int key)
	{
	 
	}

	//btVector3 Game::getRayTo(float x,float y){
	//	//float top = 1.f;
	//	//float bottom = -1.f;
	//	//float nearPlane = 1.f;
	//	//float tanFov = (top-bottom)*0.5f / nearPlane;
	//	float fov = 45 *  PI_OVER_360 * 2;

	//	vec3f cpos = m_pCamera->getPosition();
	//	btVector3			rayFrom=btVector3(cpos.x,cpos.y,cpos.z);
	//	vec3f camDir = m_pCamera->getDirection();
	//	btVector3 rayForward = (btVector3(camDir.x,camDir.y,camDir.z));
	//	rayForward.normalize();
	//	float farPlane = 10000.f;
	//	rayForward*= farPlane;

	//	btVector3 rightOffset;
	//	vec3f camUp = m_pCamera->getUp();
	//	btVector3 vertical = btVector3(camUp.x,camUp.y,camUp.z);

	//	btVector3 hor;
	//	hor = rayForward.cross(vertical);
	//	hor.normalize();
	//	vertical = hor.cross(rayForward);
	//	vertical.normalize();

	//	float tanfov = tanf(0.5f*fov);


	//	hor *= 2.f * farPlane * tanfov;
	//	vertical *= 2.f * farPlane * tanfov;

	//	btScalar aspect;

	//	aspect = m_rdInfo->width / (btScalar)m_rdInfo->height;

	//	hor*=aspect;


	//	btVector3 rayToCenter = rayFrom + rayForward;
	//	btVector3 dHor = hor * 1.f/float(m_rdInfo->width);
	//	btVector3 dVert = vertical * 1.f/float(m_rdInfo->height);


	//	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
	//	rayTo += btScalar(x) * dHor;
	//	rayTo -= btScalar(y) * dVert;
	//	return rayTo;
	//}
	 
	void Game::initCamera(float asp){
		//camera
		m_pCamera.reset(new Camera(String(L"cam1")));

		m_pCamera->setPosition(vec3f(0,0,350));
		m_pCamera->updateViewMatrix();
		m_pCamera->setProjection(45.0f,asp,0.1f,10000.f);
		if(root)
			root->attachObject(m_pCamera.get());
	}

	void Game::initInput(){
		//control
		mouse = new Mouse();
		mouse->calculateCenter( m_pRenderer->getWindow()->getHWND());
		mouse->registerListener(this);
		keyboard = new KeyBoard();
		keyboard->registerListener(this);
		m_inputVec.push_back(mouse);
		m_inputVec.push_back(keyboard);
	}
 
	void Game::drawObj(RenderObject* obj, Matrix4f& transform){

		for(auto it = obj->batchVec->begin();
			it != obj->batchVec->end(); ++it)
		{
			int shader = generalShader;// (*it)->pMtl->shader;


			m_pRenderer->applyShader(shader);
			m_pRenderer->applyMatrix(shader,"viewMat",m_pCamera->getViewMatrix());
			m_pRenderer->applyMatrix(shader,"projMat",m_pCamera->getProjection());

			//obj->
			int diffTex = (*it)->pMtl->getTexMapIdBySlot(Diffuse);
			if(diffTex >= 0)
				m_pRenderer->applyTexture(shader, "diftex", diffTex ,0);
			else
				m_pRenderer->applyTexture(shader, "diftex", 0 ,0);
			m_pRenderer->bindVertexBuffer(obj->vbo_id,sizeof(PTNC_Vertex),0, ShaderManager::getInstancePtr()->getShaderByIdx( shader) );
			m_pRenderer->bindIndexBuffer(obj->ibo_id);

 


			drawBatch(*it,transform);



		}
	}

	void Game::drawBatch(Batch* bat, Matrix4f& worldMat){
		m_pRenderer->applyMatrix(generalShader,"modelMat",worldMat);//
		m_pRenderer->drawIndex(4,bat->size,bat->startIndex,0);//4 == triangles

	}
	void Game::exit(){
		currentState = EXIT;
	}

}