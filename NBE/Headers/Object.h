#pragma once
#include "NBE.h"
namespace NBE
{
	class NBE_API Object
	{
		friend class Node;
	private:
		std::string name;
		Node* attachedNode;
		bool renderable;
	public:
		//Object(bool render=true);
		Object(std::string& nm,bool render=true);
		virtual ~Object();
		inline bool isRenderable(){return renderable;}
		Node* getAttachedNode();
		const std::string getObjName();
		void reName(const std::string& newNm);
		bool isAttached();
	};
}


