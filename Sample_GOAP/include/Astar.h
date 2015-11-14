#pragma once
//this is similar to the project Astar's Astar.h 
//more rendering stuff than normal Astar logic
#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#define TEXCOUNT 19.0f
#include <map>
#include <memory>
#define uint unsigned int
namespace NBE
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

	

	//AstarMapNode is similar to the AstarNode in Astar Project
	//this only for drawing and path finding
	class AstarMapNode
	{
	public:
		AstarMapNode(int type,uint _x,uint _z,uint _idx):nodeType(type),parent(),nodeCode(type),
			x(_x),z(_z),idx(_idx),G(0),H(0)
		{}
		~AstarMapNode()
		{}//nothing

		uint idx;
		uint parent;//index of the parent in the node list
		int nodeType;//display type
		int nodeCode;// open or closed ?
		int x;
		int z;

		uint G;
		uint H;
	};

	struct Res
	{
		Res(int idx, bool i = false):index(idx),isItem(i),exist(true)
		{}
		int index;
		bool isItem;
		bool exist;
	};

	struct AstarMap
	{
		//this is different from the Astar in project AstarMap, cuz it has multiple start point (means different npc in the map)
		AstarMap(std::string& nm):width(0),height(0),mapName(nm),startNode(0)
		{
			resource[MONEY] = new std::vector<Res>();
			resource[WOLF] =  new std::vector<Res>();
			resource[OVEN] = new std::vector<Res>();
			resource[TABLE] =  new std::vector<Res>();
			resource[FURTRADER] = new std::vector<Res>();
			resource[GENERALSTORE] =  new std::vector<Res>();
			resource[BANK] = new std::vector<Res>();
			resource[PIZZAHUT] =  new std::vector<Res>();
		}
		~AstarMap()
		{
			nodesList.clear();
			for (auto it = resource.begin(); it!= resource.end(); )
			{
				delete it->second;
				it = resource.erase(it);
			}

		}

		AstarMapNode& getNode(uint x,uint z)
		{
			return nodesList[ width * z + x  ];
		}


		void addNeighorsToOpenList(uint ndidx, uint targetEnd);

		void removeNodeInOpenList(uint current);


		void changeNodeTexInVBO(AstarMapNode& nd);	
		void changeNodeTexInVBO(AstarMapNode& nd,int tp);


		uint getLowestCostNodeInOpenList(std::vector<uint>& open,AstarMap* pMap);

		uint calToEnd(AstarMapNode& nd, AstarMapNode& endNd);

		vec2f getTexCoord(int nodeType);

		uint getAstarMapNodeByResourceType(int nodeType);
		bool removeResource(int nodeType, uint nodeIdx);

		bool checkResourceExist(int nodeType);
		bool checkResourceExist2(int nodeType);
		//bool updateResourceDisplay(uint idx, int newNodeType);//update idx node's nodeType to newNodeType

		std::string mapName;
		std::vector<AstarMapNode> nodesList;

		uint width;
		uint height;
		std::vector<uint> startNode;
		uint endNode;//there no end node, cuz end is a state, when never current state meets the end state, in astar map, it stops moving forward
		uint current;//to do every frame  
		std::map<int,std::vector<Res>*> resource;

		//path finding
		std::vector<uint> open;
		std::vector<uint> closed;

		//for drawing
		std::shared_ptr<RenderObject> renderObj;
		std::vector<PT_Vertex> vertices;

	};

}