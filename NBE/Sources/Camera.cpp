#include "Matrix.h"
#include "NBE.h"

#include "Camera.h"
#include "NBEMath.h"

#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"
namespace NBE
{

	Camera::Camera(std::string camName):Object(camName,false),m_pos(0,0,0),m_direction(0,0,-1),m_up(0,1,0),
		m_rotationMatrix(Matrix3f::Identity()),
		m_lookAtNode(nullptr)

	{}

	void Camera::handleKeyDown(int key)
	{
		vec3f impuse(0,0,0);
		switch(key)
		{
		case 'W':impuse.z -= KEY_BOARD_IMPULSE_SCALAR;break;
		case 'S':impuse.z += KEY_BOARD_IMPULSE_SCALAR;break;
		case 'A':impuse.x -= KEY_BOARD_IMPULSE_SCALAR;break;
		case 'D':impuse.x += KEY_BOARD_IMPULSE_SCALAR;break;
		case VK_SPACE:impuse.y += KEY_BOARD_IMPULSE_SCALAR;break;
		case 'C':impuse.y -= KEY_BOARD_IMPULSE_SCALAR;break;
		}
		applyImpulse(impuse);
	}
	void Camera::applyImpulse(const vec3f& impulse)
	{
		//m_rotationMatrix= Matrix3f::fromEulerAngles( m_rotation.x  , m_rotation.y , m_rotation.z  );
		vec3f worldSpaceImpulse = m_rotationMatrix * impulse ;
		m_direction = m_rotationMatrix * vec3f(0,0,-1); 
		m_up = m_rotationMatrix * vec3f(0,1,0); 

		m_pos += worldSpaceImpulse;//todo remove it
	}

	//void Camera::applyWorldBasisImpulse(const vec3f& worldImpulse)
	//{
	//	m_velocity += worldImpulse;
	//}

	const vec3f& Camera::getLocalRotation()
	{
		return m_localRot;
	}
	const vec3f& Camera::setLocalRotation(const vec3f& rotation)
	{
		m_localRot = rotation;
		m_rotationMatrix = Matrix3f::fromEulerAngles(m_localRot.x, m_localRot.y, m_localRot.z);
		return m_localRot;
	}
	const vec3f& Camera::getDirection()
	{
		return m_direction;
	}
	void Camera::rotate(const vec3f& change, char degreeOrRadian )
	{
		
		/*
		//0: degree, 1: radian
		vec3f changeRot = degreeOrRadian==0? change * ( PI / 180 ): change;
		m_rotationMatrix = m_rotationMatrix * Matrix3f::fromEulerAngles( changeRot.x  , changeRot.y , changeRot.z  );
		m_localRot = m_rotationMatrix * m_localRot;
		// m_rotationMatrix * change * ( PI / 180 )  ;
		//m_rotation.x  = clamp<float>(m_rotation.x, -PI/2, PI/2);
		*/

		//first person view rotation 
	 
		vec3f changeRot = degreeOrRadian==0? change * ( PI / 180 ): change;
		m_localRot += changeRot;
		m_rotationMatrix =  Matrix3f::fromEulerAngles( m_localRot.x  , m_localRot.y , m_localRot.z  );
	 
	}

	void Camera::rotateAround(const vec3f& change, const vec3f& pos, char degreeOrRadian){
		vec3f changeRot = degreeOrRadian==0? change * ( PI / 180 ): change;
		
		m_rotationMatrix =  Matrix3f::fromEulerAngles( changeRot.x  , changeRot.y , changeRot.z  );
		vec3f rel_pos = pos - m_pos;
		vec3f worldSpaceImpulse = m_rotationMatrix * rel_pos;
		m_direction = rel_pos.normalize(); 
		m_up = m_rotationMatrix * vec3f(0,1,0); 

		m_pos = pos - worldSpaceImpulse;
		
	}

	void Camera::moveForward(const float val){
		vec3f rel_pos = m_direction * val;
		m_pos += rel_pos;
	}

	void Camera::update(double deltaTime)
	{
		updateViewMatrix();
	}

	void Camera::updateViewMatrix()
	{		 
// 		vec3f vz= -m_direction;
// 		vz.normalize();
// 		vec3f up = vec3f(0,1,0);
// 		vec3f vx =  up.crossProduct(vz);
// 		vx.normalize();
// 
// 		// vy doesn't need to be normalized because it's a cross
// 		// product of 2 normalized vectors
// 		vec3f vy = vz.crossProduct(vx);
// 		Matrix4f inverseViewMatrix = Matrix4f(vx[0],vx[1],vx[2],0,
// 			vy[0],vy[1],vy[2],0,
// 			vz[0],vz[1],vz[2],0,
// 			m_pos[0],m_pos[1],m_pos[2],1);
// 		m_view = inverseViewMatrix.inverse();
// 		return;
 
#if 1
		//////////////////////////////////////////////////
		//old one
		Matrix4f Tc(1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			m_pos[0],m_pos[1],m_pos[2],1);//camera pos

		//Camera orientation (basis) in the world. 
		//Note that using the canonical space rotation about X matrix (row-major!) works out.

		vec3f kc = -m_direction;
		kc.normalize();


		vec3f j = m_up;//up

		vec3f ic = j.crossProduct(kc);

		ic.normalize();

		vec3f jc = kc.crossProduct(ic);

		Matrix4f Rc= Matrix4f(ic[0],ic[1],ic[2],0,
			jc[0],jc[1],jc[2],0,
			kc[0],kc[1],kc[2],0,
			0,0,0,1 );

		//Rc*Tc is Camera->World. We need World->Camera (its inverse)
		//Inverse is transpose for orthogonal

		Rc = Rc.inverse();
		//or Rc.transpose();
		//Inverse for translation is negation
		Tc = Tc.inverse();
		//or do this:
		//   		Tc.getDataPointer()[12] = -Tc[12];
		//   		Tc.getDataPointer()[13] = -Tc[13];
		//   		Tc.getDataPointer()[14] = -Tc[14];
		// (Rc*Tc).inverse = Tc.inverse * Rc.transpose

		m_view = Tc * Rc; 
#endif		 
	}

	Matrix4f Camera::setProjection(float fov, float aspect,
		float znear, float zfar)
	{
// 		float    h, w, Q;
// 
// 		
// 		h = (float)1/tan(fov * PI_OVER_360);   // 1/tan(x) == cot(x)
// 		w = h/aspect;  // 1/tan(x) == cot(x)
// 		Q = zfar/(zfar - znear);
// 
// 		m_projection = Matrix4f(w,0,0,0,
// 			0,h,0,0,
// 			0,0,Q,1,
// 			0,0,-Q*znear,0);
//  
// 		return m_projection;
	 
		//usage setProjection(45.0,aspectRatio,0.1,10000.0);
		float xymax = znear * tan(fov * PI_OVER_360);
		float ymin = -xymax;
		float xmin = -xymax;

		float width = xymax - xmin;
		float height = xymax - ymin;

		float depth = zfar - znear;
		float q = -(zfar + znear) / depth;
		float qn = -2 * (zfar * znear) / depth;

		float w = 2 * znear / width;
		w = w / aspect;
		float h = 2 * znear / height;
		m_projection = Matrix4f(w,0,0,0,
			0,h,0,0,
			0,0,q,-1,
			0,0,qn,0);

		return m_projection;
	 
	}

	///
	void Camera::lookAtNode(Node* lookAtNode){
		m_lookAtNode = lookAtNode;
		//init local trans 
	}
}