#include "Astar.h"
#include "AIBot.h"

namespace NBE
{
	namespace BT
	{
		AIBot::AIBot(int startMapNode) :bFoundPath(false), bIsActive(false), actionStartMapNode(startMapNode), currentMapNode(startMapNode)
		{}
		AIBot::~AIBot()
		{
			if (pBTRoot)
				delete pBTRoot;
			if (startState)
				delete startState;
			if (endState)
				delete endState;
			if (currentState)
				delete currentState;
		}

		void AIBot::init(AstarMap* map)
		{
			myassert(map != nullptr);
			pMap = map;

			bIsActive = true;
			defineStartAndEndState();
			currentState = new AIState(*startState);
		}
		void AIBot::Tick(float DeltaTime)
		{
			pBTRoot->Process(DeltaTime);

			if (!bFoundPath)
			{
				doPathFinding();
				if (bFoundPath)
				{
					pathNodeIndex = pathNodes.size() - 1;
				}
			}
		}
		int AIBot::getAIStateVariable(AIStateVariable _resource)
		{
			if (currentState)
				return currentState->GetVariable(_resource);
			else
				return 0;
		}
		void AIBot::setAIStateVariable(AIStateVariable _resource, int amount)
		{
			if (currentState)
				currentState->SetVariable(_resource, amount);
		}
		void AIBot::removeResource(AstarNodeType _resource, uint location)
		{
			pMap->removeResource(_resource, location);
			pMap->nodesList[location].nodeType = EMPTY;
			
			for (auto pBot : pMap->bots)
			{
				if (pBot->targetLocation == location)
				{
					pBot->setTargetLocation(-1);
				}
				auto pRL = pBot->blackboard.GetData<int>("resourceLocation");
				if (pRL && *pRL == location)
				{
					pBot->blackboard.RemoveData("resourceLocation");
				}
			}
		}
		void AIBot::addResource(AstarNodeType _resource, uint location)
		{
			pMap->addResource(_resource, location);
		}
		int AIBot::getResource(AstarNodeType _resource)
		{
			return pMap->getAstarMapNodeByResourceType(_resource);
		}
		void AIBot::setTargetLocation(int _targetLocation)
		{
			if (targetLocation != _targetLocation)
			{
				targetLocation = _targetLocation;
				if (_targetLocation < 0)
				{
					bFoundPath = false;
					pathNodes.clear();
					pathNodeIndex = -1;
				}
			}
		}
		bool AIBot::doPathFinding()
		{
			auto end = targetLocation;
			if (end < 0)
				return false;

			uint stepsCount = 0;
			actionStartMapNode = currentMapNode;
			int current = currentMapNode;

			while (current != end)
			{
				AstarMapNode& curNd = pMap->nodesList[current];
				addNeighorsToOpenList(current, end);
				removeNodeInOpenList(current);
				curNd.nodeCode = CLOSED;
				closed.push_back(current);
				if (open.size() > 0)
				{
					current = getLowestCostNodeInOpenList(open, pMap);
				}
				else
				{
					return false;
				}

				++stepsCount;
			}

			pathNodes.clear();
			//find the path
			int back = end;
			pathNodes.push_back(end);
			for (; back != actionStartMapNode;)
			{
				back = pMap->nodesList[back].parent;
				pathNodes.push_back(back);

			}
			for (auto it = open.begin(); it != open.end(); ++it)
			{
				pMap->nodesList[*it].nodeCode = EMPTY;
			}
			for (unsigned int i = 0; i < closed.size(); ++i)
			{
				pMap->nodesList[closed[i]].nodeCode = EMPTY;
			}
			open.clear();
			closed.clear();

			bFoundPath = true;
			return true;
		}
		void AIBot::defineStartAndEndState()
		{
			//TODO: remove the hardcoded conditions
			startState = new AIState(NumOfVar);
			startState->SetVariable(Hunger, 3);
			endState = new AIState(NumOfVar);
			endState->SetVariable(Hunger, 0);
		}
		void AIBot::updateLocation()
		{

		}
		//Astar pathing
		void AIBot::addNeighorsToOpenList(int ndidx, int targetEnd)
		{
			AstarMapNode& nd = pMap->nodesList[ndidx];
			AstarMapNode& targetEndNd = pMap->nodesList[targetEnd];
			auto checkNewNode = [&](AstarMapNode& newNode) {
				if (newNode.nodeCode == OPEN)
				{
					if (nd.G + 1 < newNode.G)
					{
						//re-connect the node
						newNode.G = nd.G + 1;
						newNode.parent = nd.idx;
					}
				}
				else if (newNode.nodeCode != CLOSED && newNode.nodeCode != BLOCKER)
				{
					newNode.parent = nd.idx;

					newNode.G = nd.G + 1;

					if (newNode.nodeCode == SEMIBLOCKER)
					{
						newNode.H = /*newNode.G +*/ calToEnd(newNode, targetEndNd) - 1 + (1 * 5);
					}
					else
					{
						newNode.H = /*newNode.G +*/ calToEnd(newNode, targetEndNd) - 1 + 1;
					}
					newNode.nodeCode = OPEN;
					addNodeToOpenList(newNode);
				}
			};

			if ((uint)nd.z + 1 < pMap->height)
			{
				AstarMapNode& newNode = pMap->getNode(nd.x, nd.z + 1);
				checkNewNode(newNode);
			}

			if (nd.z - 1 >= 0)
			{
				AstarMapNode& newNode = pMap->getNode(nd.x, nd.z - 1);
				checkNewNode(newNode);
			}

			if ((uint)nd.x + 1 < pMap->width)
			{
				AstarMapNode& newNode = pMap->getNode(nd.x + 1, nd.z);
				checkNewNode(newNode);
			}

			if (nd.x - 1 >= 0)
			{
				AstarMapNode& newNode = pMap->getNode(nd.x - 1, nd.z);
				checkNewNode(newNode);
			}
		}
		void AIBot::addNodeToOpenList(AstarMapNode& nd)
		{
			for (auto it = open.begin(); it != open.end(); ++it)
			{
				const auto& cur = pMap->nodesList[*it];
				if (cur.G + cur.H >= nd.G + nd.H)
				{
					open.insert(it, nd.idx);
					return;
				}
			}
			open.push_back(nd.idx);
		}
		void AIBot::removeNodeInOpenList(int ndidx)
		{
			for (auto it = open.begin(); it != open.end(); ++it)
			{
				if (*it == ndidx)
				{
					open.erase(it);
					return;
				}
			}
		}
		int AIBot::getLowestCostNodeInOpenList(std::list<int>& open, AstarMap* pMap)
		{
			if (open.size() > 0)
				return open.front();
			return -1;
		}
		unsigned int AIBot::calToEnd(AstarMapNode& nd, AstarMapNode& endNd)
		{
			return (endNd.x > nd.x ? endNd.x - nd.x : nd.x - endNd.x) + (endNd.z > nd.z ? endNd.z - nd.z : nd.z - endNd.z);
		}

		Blackboard& AIBot::getBlackboard()
		{
			return blackboard;
		}
		int AIBot::getCurrentNode()
		{
			return currentMapNode;
		}
		void AIBot::setCurrentNode(int node)
		{
			currentMapNode = node;
		}
		int AIBot::getTargetLocation()
		{
			return targetLocation;
		}
		bool AIBot::isActive()
		{
			return bIsActive;
		}
		bool AIBot::isPathFound()
		{
			return bFoundPath;
		}
		void AIBot::forgetPath()
		{
			bFoundPath = false;
		}
		void AIBot::setBTRoot(BTNode* r)
		{
			pBTRoot = r;
		}
		BTNode* AIBot::getBTRoot()
		{
			return pBTRoot;
		}
		int AIBot::getPathNode(int idx)
		{
			myassert(idx < (int)pathNodes.size());
			return pathNodes[idx];
		}
	}
}