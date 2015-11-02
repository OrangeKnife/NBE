#include "RenderObject.h"
namespace NBE
{
	RenderObject::RenderObject(String& nm)
		:Object(nm),vbo_id(-1),ibo_id(-1)
		,VertexNum(0),IndexNum(0),vbo(nullptr)
		,batchVec(new std::vector<Batch*>())
	{}

	RenderObject::~RenderObject()
	{
		if (batchVec) {
			for (size_t i = 0; i<batchVec->size(); ++i)
			{
				delete (*batchVec)[i];
			}
		}

		delete batchVec;
		deleteBufferObjectData();
	}
	void RenderObject::deleteBufferObjectData()
	{
		/*for(auto it = vbo.begin(); it != vbo.end();)
		{
			delete *it;
			it = vbo.erase(it);
		}
		*/
		delete[] vbo;
		ibo.clear();
	}

	void RenderObject::addBatch(Batch* b)
	{
		batchVec->push_back(b);
	}
}