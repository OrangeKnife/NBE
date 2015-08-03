#pragma once
#include "NBE.h"
#include "Vertex.h"
#include <vector>
#include "Object.h"
#define uint unsigned int
namespace NBE
{
	enum TEXSLOT
	{
		Diffuse = 1
	};

	struct NBE_API TexMap
	{
		std::string texMapFileName;
		int texMapSlot;//means type like diffuse or ambient
		int texId;
	};

	struct NBE_API Material
	{
		std::string matName;
		int shader;
		std::vector<TexMap*>* texMapVec;

		int getTexMapIdBySlot(int mapSlot)
		{
			for (auto it = texMapVec->begin(); it != texMapVec->end();++it)
			{
				if ((*it)->texMapSlot == mapSlot)
				{
					return (*it)->texId;
				}
			}
			return -1;
		}

		Material(std::string& nm)
			:matName(nm)
		{
			texMapVec = new std::vector<TexMap*>();
		}

		~Material()
		{
			if(texMapVec)
			{
				for(auto it = texMapVec->begin(); it != texMapVec->end();)
				{
					delete *it;
					it = texMapVec->erase(it);
				}
				delete texMapVec;
			}

		}
	};


 

	struct NBE_API Batch//one material uses one batch
	{
		Material* pMtl;
		uint startIndex;
		uint size;
		Batch(Material* mat, int start = 0, int sz = 0):pMtl(mat),
			startIndex(start),size(sz)
		{}
		Batch()
			:pMtl(nullptr),startIndex(0),size(0)
		{}
		~Batch()
		{
			//delete pMtl;
			//material lib will be deleted when mesh is gone
		}
	};

	class NBE_API RenderObject: public Object
	{
	//friend class RendererGL;
	public:
		uint VertexNum,IndexNum;
		Vertex* vbo;
		std::vector<uint> ibo;
		int vbo_id;
		int ibo_id;
		
		std::vector<Batch*>* batchVec;

		int frameIndex;
	public:
		void addBatch(Batch* b);
		void deleteBufferObjectData();


		RenderObject(String& nm);
		~RenderObject();
	};
	
}