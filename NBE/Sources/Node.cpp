#include "Object.h"
#include "Node.h"

namespace NBE
{
	Node::Node(const string& name,vec3f& pos,Matrix4f& transf)
		:m_name(name),m_pos(pos),m_localRotMatrix(Matrix3f::Identity()),m_localTM(transf),m_parent(nullptr),m_flag(NodeInit)
	{}
	Node::Node()
		:m_name(""),m_pos(vec3f()),m_localTM(Matrix4f::Identity()),m_parent(nullptr),m_flag(NodeInit)
	{}
	Node::~Node()
	{
		ChildNodeMap::iterator it = m_ChildNodeMap.begin();
		for (;it!=m_ChildNodeMap.end();)
		{
			delete (it->second);
			it = m_ChildNodeMap.erase(it);
		}
		
	}
	Node* Node::addAChindNode(Node* child)
	{
		m_ChildNodeMap[child->getName()] = child;
		child->setParent(this);
		return child;
	}
	Node* Node::removeChildNode(const string& name)
	{
		auto it = m_ChildNodeMap.find(name);
		if(it != m_ChildNodeMap.end())
		{
			return it->second;
		}

		return nullptr;
	}

	Object* Node::attachObject(Object* _obj)
	{
		for(auto it = m_objectMap.begin();
			it != m_objectMap.end(); ++it)
		{
			if(it->second == _obj)
				return _obj;
		}
		m_objectMap.insert( ObjectMap::value_type(_obj->getObjName(),_obj));
		m_flag = ObjAttached;
		_obj->attachedNode = this;
		return _obj;

	}
	
	Object* Node::detachObject(const string& name)
	{
		auto it = m_objectMap.find(name);
		if(it != m_objectMap.end())
		{
			m_flag = ObjDetached;
			Object* rtv = it->second;
			m_objectMap.erase(it);
			return rtv;
		}
		else
		{
			return nullptr;
		}
	}


	const string& Node::getName()
	{
		return m_name;
	}

	void Node::setParent( Node* p )
	{
		m_parent = p;
	}

	void Node::update()
	{
		 //TODO ??
	}

	void Node::rotate(const vec3f& change, char degreeOrRadian )
	{
		 
		//0: degree, 1: radian
		vec3f changeRot = degreeOrRadian==0? change * ( PI / 180 ): change;
		m_localRotMatrix = m_localRotMatrix * Matrix3f::fromEulerAngles( changeRot.x  , changeRot.y , changeRot.z  );
		m_localRot = m_localRotMatrix * m_localRot;
		vec3f tempTrans(m_localTM[12],m_localTM[13],m_localTM[14]);
		m_localTM = Matrix4f(m_localRotMatrix);
		memcpy(&m_localTM[12],&tempTrans[0],3*sizeof(float));

		
		// m_rotationMatrix * change * ( PI / 180 )  ;
		//m_rotation.x  = clamp<float>(m_rotation.x, -PI/2, PI/2);
		 

		//first person view rotation 
		//vec3f changeRot = degreeOrRadian==0? change * ( PI / 180 ): change;
		//m_localRot += changeRot;
		//m_rotationMatrix =  Matrix3f::fromEulerAngles( m_localRot.x  , m_localRot.y , m_localRot.z  );
		

	}
}