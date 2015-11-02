#include "RenderObject.h"
#include "Astar.h"
#include "Error.h"
namespace NBE
{
	void AstarMap::addNeighorsToOpenList(uint ndidx, uint targetEnd)
	{
		AstarMapNode& nd = nodesList[ndidx];
		AstarMapNode& targetEndNd = nodesList[targetEnd];

		auto checkNewNode = [&](AstarMapNode& newNode){
			if (newNode.nodeCode == OPEN)
			{
				if( nd.G + 1 < newNode.G)
				{
					//re-connect the node
					newNode.G = nd.G + 1;
					newNode.parent = nd.idx;
				}
			}
			else if(newNode.nodeCode != CLOSED && newNode.nodeCode != BLOCKER)
			{
				newNode.parent = nd.idx;

				newNode.G = nd.G + 1;

				if(newNode.nodeCode==SEMIBLOCKER)
				{
					newNode.H = calToEnd(newNode,targetEndNd) - 1 + (1 * 5) ;
				}
				else
				{
					newNode.H = calToEnd(newNode,targetEndNd) - 1 + 1 ;
				}
				newNode.nodeCode = OPEN;
				//changeNodeTexInVBO(newNode);
				open.push_back( newNode.idx ); 
			}
		};


		if((uint)nd.z + 1 <  height)
		{
			AstarMapNode& newNode = getNode(nd.x,nd.z + 1);
			checkNewNode(newNode);
		}

		if(nd.z - 1 >= 0)
		{
			AstarMapNode& newNode = getNode(nd.x,nd.z - 1);
			checkNewNode(newNode);
		}

		if((uint)nd.x + 1 < width)
		{
			AstarMapNode& newNode = getNode(nd.x + 1,nd.z);
			checkNewNode(newNode);
		}

		if(nd.x - 1 >= 0)
		{
			AstarMapNode& newNode = getNode(nd.x - 1,nd.z);
			checkNewNode(newNode);
		}
	}

	void AstarMap::removeNodeInOpenList(uint ndidx)
	{
		for(auto it = open.begin();it!=open.end();++it)
		{
			if (*it == ndidx)
			{
				open.erase(it);
				return;
			}
		}
	}

	void AstarMap::changeNodeTexInVBO(AstarMapNode& nd)
	{
		uint idx = ( width * nd.z +  nd.x ) * 4; 
		vec2f tc= getTexCoord(nd.nodeType);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx].texcoord =  tc;
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 1].texcoord = tc + vec2f(1, 0);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 2].texcoord = tc + vec2f(1,1/TEXCOUNT);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 3].texcoord = tc + vec2f(0,1/TEXCOUNT);
	}

	void AstarMap::changeNodeTexInVBO(AstarMapNode& nd,int tp)//keep the nd info ,just change the coordnates in vbo
	{
		uint idx = ( width * nd.z +  nd.x ) * 4; 
		vec2f tc= getTexCoord(tp);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx].texcoord =  tc;
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 1].texcoord = tc + vec2f(1, 0);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 2].texcoord = tc + vec2f(1,1/TEXCOUNT);
		static_cast<PT_Vertex*>(renderObj->vbo)[idx + 3].texcoord = tc + vec2f(0,1/TEXCOUNT);
	}

	unsigned int AstarMap::calToEnd(AstarMapNode& nd, AstarMapNode& endNd)
	{
		return (endNd.x > nd.x ? endNd.x - nd.x: nd.x - endNd.x ) + (endNd.z > nd.z ? endNd.z - nd.z : nd.z - endNd.z);
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
		switch(nodeType)
		{
		case START:
			return vec2f(0,16/TEXCOUNT);
		case END:
			return vec2f(0,15/TEXCOUNT);
		case EMPTY:
			return vec2f(0,13/TEXCOUNT);
		case SEMIBLOCKER:
			return vec2f(0,17/TEXCOUNT);
		case BLOCKER:
			return vec2f(0,18/TEXCOUNT);
		case CLOSED:
			return vec2f(0,12/TEXCOUNT);
		case OPEN:
			return vec2f(0,11/TEXCOUNT);
		case PATH:
			return vec2f(0,14/TEXCOUNT);
			//extra goap tex
		case MONEY:
			return vec2f(0,10/TEXCOUNT);
		case WOLF:
			return vec2f(0,9/TEXCOUNT);
		case OVEN:
			return vec2f(0,8/TEXCOUNT);
		case TABLE:
			return vec2f(0,7/TEXCOUNT);
		case FURTRADER:
			return vec2f(0,6/TEXCOUNT);
		case GENERALSTORE:
			return vec2f(0,5/TEXCOUNT);
		case PIZZAHUT:
			return vec2f(0,4/TEXCOUNT);
		case NPC1:
			return vec2f(0,3/TEXCOUNT);
		case NPC2:
			return vec2f(0,2/TEXCOUNT);
		case NPC3:
			return vec2f(0,1/TEXCOUNT);
		case BANK:
			return vec2f(0,0);
		default:
			return vec2f();
		}
	}

	uint AstarMap::getAstarMapNodeByResourceType( int nodeType )
	{
		uint rt = nodesList.size();
		if(nodeType <= EMPTY)
			return rt;

		
		auto it = resource[nodeType]->begin();

		if(it!= resource[nodeType]->end())
		{
			return it->index;
		}
		//errorMsg("Can't find the node in the current map","error");
		return rt;
	}

	bool AstarMap::removeResource(int nodeType, uint nodeIdx)
	{
		if(nodeType <= EMPTY)
			return false;

		auto it = resource[nodeType]->begin();

		for(;it!= resource[nodeType]->end(); ++it)
		{
			if(it->index == nodeIdx)
			{
				resource[nodeType]->erase(it);
				return true;
			}
		}
		return false;
	}

	bool AstarMap::checkResourceExist2(int nodeType)
	{
		bool rt = false;
		if (nodeType <= EMPTY )
		{
			return true;
		}

		auto it = resource[nodeType]->begin();
		for (;it!=resource[nodeType]->end(); ++it)
		{
			return true;
		}
		return rt;
	}

	bool AstarMap::checkResourceExist(int nodeType)
	{
		bool rt = false;
		if (nodeType <= EMPTY )
		{
			return true;
		}

		auto it = resource[nodeType]->begin();
		for (;it!=resource[nodeType]->end(); ++it)
		{
			if(it->exist )
				return true;
		}
		return rt;
	}


	uint AstarMap::getLowestCostNodeInOpenList(std::vector<uint>& open,AstarMap* pMap)
	{
		uint rt = open.back();
		AstarMapNode backnd = pMap->nodesList[rt];
		uint val = backnd.G + backnd.H;
		for(uint i = 0;i < open.size() ;++i)
		{
			AstarMapNode nd  = pMap->nodesList[open[i]];
			if(val > nd.G + nd.H)
			{
				val = nd.G + nd.H;
				rt = open[i];
			}
			else if(val == nd.G + nd.H)
			{
				if(backnd.H > nd.H)
				{
					val = nd.G + nd.H;
					rt = open[i];
				}
			}
		}
		return rt;
	}

	/*bool AstarMap::updateResourceDisplay( uint idx, int nodeType )
	{
		nodesList[idx].nodeType = nodeType;
		changeNodeTexInVBO(nodesList[idx]);
		return true;
	}*/


}