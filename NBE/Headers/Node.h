#pragma once
#include "NBE.h"
#include "Vector3.h"
#include "Matrix.h"
#include <unordered_map>

using namespace std;
namespace NBE
{
	enum NodeFlag
	{
		Clean = 0,
		NodeInit,
		NodeAdded,
		NodeRemoved,
		ObjAttached,
		ObjDetached,
		DirtyMatrix,
		DirtyGeometry,
		DirtyMaterial
	};

	class NBE_API Node
	{
	public:
		typedef unordered_map<std::string,Node*> ChildNodeMap;
		typedef unordered_map<std::string,Object*> ObjectMap;

		Node();
		Node(const string& name,vec3f& pos= vec3f(),Matrix4f& transf = Matrix4f::Identity());
		virtual ~Node();

		const string& getName();
		
		Node* addAChindNode(Node*);
		Node* removeChildNode(const string& name);

		Object* attachObject(Object* _obj);
		Object* detachObject(const string& name);

		virtual void update();
		inline void moveLocal(const vec3f& localVector)
		{m_pos += m_localRotMatrix * localVector;}
		inline void move(const vec3f& worldVector)
		{m_pos += worldVector;}
		inline void setPosition(const vec3f& position){m_pos = position;}
		inline const vec3f getPosition(){
						Matrix4f posMatrix = getWorldTM();
			m_pos = vec3f(posMatrix[12],posMatrix [13],posMatrix [14]);
			return m_pos;
		}
		inline const vec3f& getLocalRotation(){return m_localRot;};//radian
		inline const void setLocalRotation(const vec3f& rotation)
		{m_localRot = rotation;}
		inline void rotate(const vec3f& change, char degreeOrRadian=0);

		inline void setLocalTM(Matrix4f& lc){m_localTM = lc;}
		inline Matrix4f getLocalTM(){return m_localTM;}

		inline void markDirty(int fg){m_flag = fg;}

		inline bool hasChild(){return m_ChildNodeMap.size()>0;}
		inline ChildNodeMap& getChildren(){return m_ChildNodeMap;}
		inline ObjectMap& getObjects(){return m_objectMap;}
		inline Matrix4f getWorldTM() const 
		{
			if(m_flag > Clean)
				return m_parent ? m_localTM * m_parent->getWorldTM() : m_localTM;
			else
				return m_transformation;
		}
	private:

		vec3f m_pos;
		vec3f m_localRot;
		Matrix3f m_localRotMatrix;

		void setParent(Node* p);
		Node* m_parent;
		ChildNodeMap m_ChildNodeMap;
		ObjectMap m_objectMap;


		string m_name;
		Matrix4f m_transformation;//world TM, TODO when the node keep still, this keep still
		Matrix4f m_localTM;//local transformation matrix

		int m_flag;
	};

	
}