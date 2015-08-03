#pragma once
#include "NBE.h"
namespace NBE
{
	class NBE_API Object
	{
		friend class Node;
	private:
		String name;
		Node* attachedNode;
		bool renderable;
	public:
		//Object(bool render=true);
		Object(String& nm,bool render=true);
		virtual ~Object();
		inline bool isRenderable(){return renderable;}
		Node* getAttachedNode();
		String getObjName();
		bool isAttached();
	};
}


