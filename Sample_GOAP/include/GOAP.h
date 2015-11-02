#pragma once
#pragma comment (lib,"NBE.lib")
#pragma comment (lib,"RendererGL.lib")
#pragma comment (lib,"RenderSystemDX.lib")
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


#include "CustomAction.h"
#include "Astar.h"
#include "Config.h"
#include "Camera.h"
#include "NBETimer.h"

#include "EventHub.h"
#include "CustomEvent.h"
namespace NBE
{

	class GOAPapp
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

		struct AIbot
		{
			AIbot(uint startMapNode):foundPathToTargetNode(false),foundPlan(false),actionStartMapNode(startMapNode),currentMapNode(startMapNode),speed(60)
			{
				loadActions();
			}
			~AIbot()
			{
				clearList<CustomAction>(actionlist);
				clearList<AIState>(open);
				clearList<AIState>(closed);
				delete endState;
			}

			template<typename T>
			void clearList(vector<T*>& list)
			{
				for (auto it = list.begin(); it!=list.end(); )
				{
					delete (*it);
					it = list.erase(it);
				}
			}
			void defineStartEnd(AstarMap* mp)
			{
				botInMap = mp;
				startState = new AIState(NumOfVar);
				startState->formerAction = NULL;
				startState->m_variableVec[Hunger] = 2;
				endState = new AIState(NumOfVar);
				endState->m_variableVec[Hunger] = 0;

			}
			void loadActions()
			{
				//actions
				//TODO: support different bot actions

				CustomAction* eatBread = new CustomAction(string("eatBread"),0);
				eatBread->operations.push_back(  Action::Operation(Bread,-1) );
				eatBread->operations.push_back(  Action::Operation(Hunger,-1) );
				eatBread->needNodeType = TABLE;
				actionlist.push_back(eatBread);

				CustomAction* eatPizza = new CustomAction(string("eatPizza"),0);
				eatPizza->operations.push_back(  Action::Operation(Pizza,-1) );
				eatPizza->operations.push_back(  Action::Operation(Hunger,-1) );
				eatPizza->needNodeType = TABLE;
				actionlist.push_back(eatPizza);

				CustomAction* buyPizza = new CustomAction(string("buyPizza"),0);
				buyPizza->operations.push_back(  Action::Operation(Money,-2) );
				buyPizza->operations.push_back(  Action::Operation(Pizza,1) );
				buyPizza->needNodeType = PIZZAHUT;
				actionlist.push_back(buyPizza);

				CustomAction* buyFlour = new CustomAction(string("buyFlour"),0);
				buyFlour->operations.push_back(  Action::Operation(Money,-1) );
				buyFlour->operations.push_back(  Action::Operation(Flour,1) );
				buyFlour->needNodeType = GENERALSTORE;
				actionlist.push_back(buyFlour);

				CustomAction* bake = new CustomAction(string("bake"),0);
				bake->operations.push_back(  Action::Operation(Flour,-1) );
				bake->operations.push_back(  Action::Operation(Bread,1) );
				bake->needNodeType = OVEN;
				actionlist.push_back(bake);

				CustomAction* kill = new CustomAction(string("kill"),0);
				kill->operations.push_back(  Action::Operation(Wolf,-1) );
				kill->operations.push_back(  Action::Operation(Fur,1) );
				//kill->needNodeType = WOLF;
				actionlist.push_back(kill);

				CustomAction* sellSkin = new CustomAction(string("sellFur"),0);
				sellSkin->operations.push_back(  Action::Operation(Fur,-1) );
				sellSkin->operations.push_back(  Action::Operation(Money,1) );
				sellSkin->needNodeType = FURTRADER;
				actionlist.push_back(sellSkin);

				CustomAction* getMoney = new CustomAction(string("getMoney"),0);
				getMoney->operations.push_back(  Action::Operation(Money,1) );
				getMoney->needNodeType = MONEY;
				actionlist.push_back(getMoney);

				CustomAction* getWolf = new CustomAction(string("getWolf"),0);
				getWolf->operations.push_back(  Action::Operation(Wolf,1) );
				getWolf->needNodeType = WOLF;
				actionlist.push_back(getWolf);

				CustomAction* steal = new CustomAction(string("steal"),10);
				steal->operations.push_back(  Action::Operation(Money,2) );
				steal->needNodeType = BANK;
				actionlist.push_back(steal);
			}
			//////////////////////////////////////////////////////////////////////////
			bool doPlanfinding( int stepsPerFrame = 0)
			{
				uint stepsCount = 0;
				current = startState;
				open.push_back(current);
				//define your end logic here in the while loop
				while((*current).m_variableVec[Hunger] > endState->m_variableVec[Hunger])
				{
					addNeighorsToOpenList();
					//printMapOnConsole(pMap);
					removeNodeInOpenList();
					 
					 
					if(!checkStateInClosedList(current))
						closed.push_back(current);
 
					//printMapOnConsole(pMap);
					if(open.size() > 0)
					{
						current = getLowestCostNodeInOpenList();
						
					}
					else
					{
						//std::cout<<"open list empty!\n";
						//errorMsg("There is no plan","Error");
					 
						return false;
						//printMapOnConsole(pMap);
					}

					++stepsCount;
					if (stepsPerFrame > 0 && stepsCount == stepsPerFrame)
					{
						return false; //jump out, wait for next frame to find the path
					}
				}

				//find the plan
				
				//AIState* back = current;
				currentPlan.push_back((CustomAction*)current->formerAction);

				//trace("["<<back->m_variableVec[0] <<","<<back->m_variableVec[1]<<","<<back->m_variableVec[2]<<","<<back->m_variableVec[3]<<","<<back->m_variableVec[4]<<","<<back->m_variableVec[5]<<","<<back->m_variableVec[6]<<"]" );
				for (;current != startState;)
				{
					current = static_cast<AIState*>(current->parent);
					if(current != startState)
						currentPlan.push_back((CustomAction*)current->formerAction);

					//trace("["<<back->m_variableVec[0] <<","<<back->m_variableVec[1]<<","<<back->m_variableVec[2]<<","<<back->m_variableVec[3]<<","<<back->m_variableVec[4]<<","<<back->m_variableVec[5]<<","<<back->m_variableVec[6]<<"]" );
				}
				//trace(open.size());
				//trace(closed.size());

				return true;//at this point, current == startState, so in astar map , I can use it 
			}

			void updateExistenceByMap()
			{
				 auto r = botInMap->resource.begin();
				 for (;r!= botInMap->resource.end(); ++r)
				 {
					 auto it = (*r).second->begin();
					 for (;it!=(*r).second->end(); ++it)
					 {
						 it->exist = true;
					 }
				 }
			}
			void addNeighorsToOpenList() //add next states into openlist
			{
				for (unsigned int i = 0; i < actionlist.size(); ++i)
				{

					if (actionlist[i]->preCondition(current) && botInMap->checkResourceExist2(actionlist[i]->needNodeType))
					{
						AIState* newState = new AIState(*current);
						newState->act(actionlist[i]);//will recored this action in the newState

						int needRes = actionlist[i]->needNodeType;
						if( needRes > EMPTY)
						{
							auto fst = botInMap->resource[ needRes ]->begin();
							for( ;fst != botInMap->resource[ needRes ]->end(); fst++)
							{
								if ( fst->isItem && fst->exist)
								{
									fst->exist = false;
									break;
								}

							}
							//update the resource
						}
						AIState* stateInOpen = checkStateInOpenList(newState);
						bool isInClosed = checkStateInClosedList(newState);
						if(!stateInOpen && !isInClosed )
						{
							newState->parent = current;

							newState->G = current->G + 1 ;

							newState->H = calToEndState(newState,endState) + actionlist[i]->cost;

							open.push_back( newState );
						}
						else if(!isInClosed)
						{ 
							if( current->G + 1 + (current->formerAction ?current->formerAction->cost :0 ) < stateInOpen->G + stateInOpen->formerAction->cost)
							{
								//re-connect the node
								stateInOpen->G = current->G + 1;
								stateInOpen->parent = current;
							}

							delete newState;
						}
						else
						{

							delete newState;
						}

					}
				}

			}
			void removeNodeInOpenList()
			{
				for(auto it = open.begin();it!=open.end();++it)
				{
					if (*it == current)
					{
						open.erase(it);
						return;
					}
				}
			}
			AIState* getLowestCostNodeInOpenList()
			{
				AIState* rt = open.back();
				int val = rt->G + rt->H ;
				for(uint i = 0;i < open.size() ;++i)
				{
					AIState* nd  = open[i];
					int ndVal =  nd->G + nd->H;
					if(val   > ndVal  )
					{
						val = ndVal;
						rt = open[i];
					}
					else if(val == ndVal)
					{
						if(rt->H > nd->H)
						{
							val = ndVal;
							rt = open[i];
						}
					}
				}
				return rt;
			}
			AIState* checkStateInOpenList(AIState* st)
			{
				for (auto it = open.begin(); it!= open.end(); ++it)
				{
					if( *(*it) == *st )
						return *it;
				}

				return NULL;
			}
			bool checkStateInClosedList(AIState* st)
			{
				for (auto it = closed.begin(); it!= closed.end(); ++it)
				{
					if( *(*it) == *st )
						return true;
				}
				return false;
			}
			uint calToEndState(AIState* nd,AIState* endState)
			{
				//this func is determined by the logic
				uint rt = 0;
				/*for(unsigned int i = 0; i < nd->m_variableVec.size(); ++i)
				{
				rt += abs(nd->m_variableVec[i] - endState->m_variableVec[i]);
				}*/
				rt = abs(nd->m_variableVec[Hunger] - endState->m_variableVec[Hunger]);
				return rt;
			}

			AIState* startState;
			AIState* endState;//goal
			AIState* current;

			std::vector<AIState*> open;
			std::vector<AIState*> closed;

			std::vector<CustomAction*> actionlist;//all the actions available
			std::vector<CustomAction*> currentPlan;//current actions based on the map

			bool foundPlan;
			
			AstarMap* botInMap;
			bool foundPathToTargetNode;
			uint actionStartMapNode;
			uint currentMapNode;
			int currentMapNode_Type;//to store the type, so that I can draw a green square regardless what the type is 

			std::vector<uint> pathNodes;
			int posInPathNode;
			CustomAction* goToNodeAction;//after reach the pathNodes target, do this goToNodeAction, like "kill" wolf

			int speed ;
			int display;//to pick the texture
		};

		enum AIStateVariable
		{
			Money = 0,
			Flour,
			Bread,
			Pizza,
			Hunger,
			Wolf,
			Fur,



			NumOfVar
		};

		GOAPapp(HINSTANCE h);
		~GOAPapp();
		RenderInfo* loadRenderInfo(const String& cfgname);


		void updateFrame();
		

		//////////////////////////////////////////////////////////////////////////

		bool doPathFinding( AstarMap* pMap, AIbot* bot, uint end, int stepsPerFrame = 0);
		void initTex();
		void initBots(AstarMap* pMap);
		void loadMaps();//more like astar path finding load maps
		

		void clearAllBotsInfo();

		void initCamera(float asp);
		void initInput();

		void run();
		void exit();

		void handleMovementEvent(NEvent_Key* eventData, void* p);
	private:
		
		std::vector<AIbot*> botVec;

		//////////////////////////////////////////////////////////////////////////
		Config cfg;
		RenderInfo* m_rdInfo;
		std::shared_ptr<Camera> m_pCamera;//main camera
		std::shared_ptr<RenderSystem> m_pRenderer;
		GAMESTATE currentState;

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



}