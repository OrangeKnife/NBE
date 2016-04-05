#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <list>
#define TEXCOUNT 19.0f
#include <map>
#include <memory>
#define uint unsigned int
#include "RenderObject.h"
namespace NBE
{
	namespace BT
	{
		enum AstarNodeType
		{
			EMPTY = 0,
			START,
			END,
			BLOCKER,
			SEMIBLOCKER,
			MONEY,
			WOLF,
			OVEN,
			TABLE,
			FURTRADER,
			GENERALSTORE,
			BANK,
			PIZZAHUT,
			OPEN,
			CLOSED,
			CURNODE,
			PATH,
			NPC1,
			NPC2,
			NPC3,
			TOTALTYPE
		};

		struct AstarMapNode
		{
			AstarMapNode(int type, uint _x, uint _z, uint _idx) :nodeType(type), parent(), nodeCode(type),
				x(_x), z(_z), idx(_idx), G(0), H(0)
			{}
			~AstarMapNode()
			{}

			int idx;
			int parent;//index of the parent in the node list
			int nodeType;//display type
			int nodeCode;// open or closed ?
			int x;
			int z;

			uint G;
			uint H;
		};

		struct Res
		{
			Res(int idx, bool i = false) :index(idx), isItem(i)
			{}
			int index;
			bool isItem;
		};
		
		class AIBot;//forward declaration

		struct AstarMap
		{
			AstarMap(std::string& nm);
			~AstarMap();

			AstarMapNode& getNode(uint x, uint z)
			{
				return nodesList[width * z + x];
			}

			void changeNodeTexInVBO(AstarMapNode& nd);
			void changeNodeTexInVBO(AstarMapNode& nd, int tp);
			vec2f getTexCoord(int nodeType);

			int getAstarMapNodeByResourceType(int nodeType);
			bool removeResource(int nodeType, uint nodeIdx);
			bool addResource(int nodeType, uint nodeIdx);

			std::string mapName;
			std::vector<AstarMapNode> nodesList;
			std::vector<AIBot*> bots;

			uint width;
			uint height;
			std::vector<int> startNode;
			int endNode;
			int current;
			std::map<int, std::vector<Res>*> resource;

			//for drawing
			std::shared_ptr<RenderObject> renderObj;
			std::vector<PT_Vertex> vertices;

		};
	}
}