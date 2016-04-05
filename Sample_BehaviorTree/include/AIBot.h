#pragma once
#include "AIState.h"
#include "BTNode.h"
namespace NBE
{
	namespace BT
	{
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

		class AIBot
		{
		public:
			AIBot(int startMapNode);
			~AIBot();

			void init(AstarMap* map);
			void Tick(float DeltaTime);
			int getAIStateVariable(AIStateVariable _resource);
			void setAIStateVariable(AIStateVariable _resource, int amount);
			void removeResource(AstarNodeType _resource, uint location);
			void addResource(AstarNodeType _resource, uint location);
			int getResource(AstarNodeType _resource);
			void setTargetLocation(int _targetLocation);
			bool doPathFinding();
			void defineStartAndEndState();
			void updateLocation();
			void addNeighorsToOpenList(int ndidx, int targetEnd);
			void addNodeToOpenList(AstarMapNode& nd);
			void removeNodeInOpenList(int current);
			int getLowestCostNodeInOpenList(std::list<int>& open, AstarMap* pMap);
			unsigned int calToEnd(AstarMapNode& nd, AstarMapNode& endNd);
			Blackboard& getBlackboard();
			int getCurrentNode();
			void setCurrentNode(int node);
			int getTargetLocation();
			bool isActive();
			bool isPathFound();
			void forgetPath();
			void setBTRoot(BTNode*);
			BTNode* getBTRoot();
			int getPathNode(int idx);

			int savedMapNode_Type;//to store the type
			int pathNodeIndex;
			int display;
		private:
			AIState* currentState;
			AIState* startState, *endState;
			Blackboard blackboard;

		
			bool bIsActive;
			BTNode* pBTRoot;
			AstarMap* pMap;
			bool bFoundPath;
			int targetLocation;
			int actionStartMapNode;
			int currentMapNode;
		
			std::vector<int> pathNodes;

			//path finding
			std::list<int> open;
			std::vector<int> closed;
		};

	}
}