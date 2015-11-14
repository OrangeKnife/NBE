#include "Object.h"

namespace NBE
{
	//Object::Object(bool render):attachedNode(nullptr),renderable(render)
	//{}

	Object::Object(std::string& nm,bool render): name(nm),attachedNode(nullptr),
		renderable(render)
	{}

	Object::~Object()
	{
		//TODO: remove it from the node it attached
	}

	void Object::reName(const std::string& newNm)
	{
		name = newNm;
	}

	Node* Object::getAttachedNode()
	{
		return attachedNode;
	}
	const std::string Object::getObjName()
	{
		return name;
	}

	bool Object::isAttached()
	{
		return attachedNode != nullptr;
	}

}