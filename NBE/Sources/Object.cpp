#include "Object.h"

namespace NBE
{
	//Object::Object(bool render):attachedNode(nullptr),renderable(render)
	//{}

	Object::Object(String& nm,bool render): name(nm),attachedNode(nullptr),
		renderable(render)
	{}

	Object::~Object()
	{
		//TODO: remove it from the node it attached
	}

	Node* Object::getAttachedNode()
	{
		return attachedNode;
	}
	String Object::getObjName()
	{
		return name;
	}

	bool Object::isAttached()
	{
		return attachedNode != nullptr;
	}

}