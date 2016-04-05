#include "RenderObject.h"
#include "Astar.h"
#include "AIBot.h"
#include "Error.h"
namespace NBE
{
	namespace BT
	{
		AstarMap::AstarMap(std::string& nm) :width(0), height(0), mapName(nm), startNode(0)
		{
			resource[MONEY] = new std::vector<Res>();
			resource[WOLF] = new std::vector<Res>();
			resource[OVEN] = new std::vector<Res>();
			resource[TABLE] = new std::vector<Res>();
			resource[FURTRADER] = new std::vector<Res>();
			resource[GENERALSTORE] = new std::vector<Res>();
			resource[BANK] = new std::vector<Res>();
			resource[PIZZAHUT] = new std::vector<Res>();
		}
		AstarMap::~AstarMap()
		{
			nodesList.clear();
			for (auto rit = resource.rbegin(); rit != resource.rend(); ++rit)
			{
				delete rit->second;
			}
			resource.clear();

			for (auto it = bots.begin(); it != bots.end(); ++it)
			{
				delete *it;
			}
			bots.clear();

		}
		void AstarMap::changeNodeTexInVBO(AstarMapNode& nd)
		{
			uint idx = (width * nd.z + nd.x) * 4;
			vec2f tc = getTexCoord(nd.nodeType);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx].texcoord = tc;
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 1].texcoord = tc + vec2f(1, 0);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 2].texcoord = tc + vec2f(1, 1 / TEXCOUNT);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 3].texcoord = tc + vec2f(0, 1 / TEXCOUNT);
		}
		void AstarMap::changeNodeTexInVBO(AstarMapNode& nd, int tp)//keep the nd info ,just change the coordnates in vbo
		{
			uint idx = (width * nd.z + nd.x) * 4;
			vec2f tc = getTexCoord(tp);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx].texcoord = tc;
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 1].texcoord = tc + vec2f(1, 0);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 2].texcoord = tc + vec2f(1, 1 / TEXCOUNT);
			static_cast<PT_Vertex*>(renderObj->vbo)[idx + 3].texcoord = tc + vec2f(0, 1 / TEXCOUNT);
		}
		vec2f AstarMap::getTexCoord(int nodeType)
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
			switch (nodeType)
			{
			case START:
				return vec2f(0, 16 / TEXCOUNT);
			case END:
				return vec2f(0, 15 / TEXCOUNT);
			case EMPTY:
				return vec2f(0, 13 / TEXCOUNT);
			case SEMIBLOCKER:
				return vec2f(0, 17 / TEXCOUNT);
			case BLOCKER:
				return vec2f(0, 18 / TEXCOUNT);
			case CLOSED:
				return vec2f(0, 12 / TEXCOUNT);
			case OPEN:
				return vec2f(0, 11 / TEXCOUNT);
			case PATH:
				return vec2f(0, 14 / TEXCOUNT);
				//extra goap tex
			case MONEY:
				return vec2f(0, 10 / TEXCOUNT);
			case WOLF:
				return vec2f(0, 9 / TEXCOUNT);
			case OVEN:
				return vec2f(0, 8 / TEXCOUNT);
			case TABLE:
				return vec2f(0, 7 / TEXCOUNT);
			case FURTRADER:
				return vec2f(0, 6 / TEXCOUNT);
			case GENERALSTORE:
				return vec2f(0, 5 / TEXCOUNT);
			case PIZZAHUT:
				return vec2f(0, 4 / TEXCOUNT);
			case NPC1:
				return vec2f(0, 3 / TEXCOUNT);
			case NPC2:
				return vec2f(0, 2 / TEXCOUNT);
			case NPC3:
				return vec2f(0, 1 / TEXCOUNT);
			case BANK:
				return vec2f(0, 0);
			default:
				return vec2f();
			}
		}
		int AstarMap::getAstarMapNodeByResourceType(int nodeType)
		{
			int rt = -1;
			if (nodeType <= EMPTY)
				return rt;


			auto it = resource[nodeType]->begin();

			if (it != resource[nodeType]->end())
			{
				return it->index;
			}
			//errorMsg("Can't find the node in the current map","error");
			return rt;
		}
		bool AstarMap::removeResource(int nodeType, uint nodeIdx)
		{
			if (nodeType <= EMPTY)
				return false;

			auto it = resource[nodeType]->begin();

			for (; it != resource[nodeType]->end(); ++it)
			{
				if (it->index == nodeIdx)
				{
					resource[nodeType]->erase(it);
					return true;
				}
			}
			return false;
		}
		bool AstarMap::addResource(int nodeType, uint nodeIdx)
		{
			if (nodeType <= EMPTY)
				return false;

			auto it = resource[nodeType]->begin();

			for (; it != resource[nodeType]->end(); ++it)
			{
				if (it->index == nodeIdx)
				{
					return true;
				}
			}
			resource[nodeType]->push_back(Res(nodeIdx));
			return false;
		}
	}
}