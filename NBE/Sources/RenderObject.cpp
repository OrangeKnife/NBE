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
		if(batchVec->size())
		{
			for(auto it = batchVec->begin(); it != batchVec->end();)
			{
				delete *it;
				it = batchVec->erase(it);
			}
			delete batchVec;
		}
		
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