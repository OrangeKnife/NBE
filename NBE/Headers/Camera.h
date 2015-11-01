#pragma once
#include "Object.h"
namespace NBE
{
	class NBE_API Camera: public Object
	{
	public:
		Camera(const String camName);
		void handleKeyDown(int key);
		void setPosition(const vec3f& position){m_pos = position;}
		//void setDirection(const vec3f& direction){m_direction = direction;}
		void applyImpulse(const vec3f& impulse);
		//void applyWorldBasisImpulse(const vec3f& worldImpulse);
		const vec3f& getLocalRotation();//radian
		const vec3f& setLocalRotation(const vec3f& rotation);
		void rotate(const vec3f& change, char degreeOrRadian=0);
		void rotateAround(const vec3f& change, const vec3f& pos, char degreeOrRadian=0);
		const vec3f& getDirection();
		
		void updateViewMatrix();
		Matrix4f setProjection(float fov, float aspect,float znear, float zfar);

		inline vec3f& getUp(){return m_up;}
		//camera up
		inline vec3f& getPosition(){return m_pos;}
		inline Matrix4f getProjection(){return m_projection;}
		inline Matrix4f getViewMatrix(){return m_view;}

		void lookAtNode(Node* lookAtNode);
		Node* getLookAtNode(){ return m_lookAtNode;}
		void moveForward(const float val);
	private:
		vec3f m_pos;
		vec3f m_localRot;
		Matrix3f m_rotationMatrix;
		
		vec3f m_direction;
		vec3f m_up;

		Matrix4f m_view;
		Matrix4f m_projection;

		Node* m_lookAtNode;

		const float KEY_BOARD_IMPULSE_SCALAR = 1.f;
	};
}