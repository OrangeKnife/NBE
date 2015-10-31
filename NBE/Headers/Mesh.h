#pragma once
//this is for skinning animation
#include "Matrix.h"
#include "Singleton.h"
#include <vector>
#include "Vector3.h"
#include "Quaternion.h"
#include "Vertex.h"
#include "RenderObject.h"
#include "TypeCast.h"
#include "Node.h"
#define uint unsigned int

using namespace std;
namespace NBE
{
	//this is the same defines for 3ds max 2012
#define ID_AM 0   //!< Ambient 
#define ID_DI 1   //!< Diffuse
#define ID_SP 2   //!< Specular
#define ID_SH 3   //!< Glossiness (Shininess in 3ds Max release 2.0 and earlier)
#define ID_SS 4   //!< Specular Level (Shininess strength in 3ds Max release 2.0 and earlier)
#define ID_SI 5   //!< Self-illumination
#define ID_OP 6   //!< Opacity
#define ID_FI 7   //!< Filter color
#define ID_BU 8   //!< Bump 
#define ID_RL 9   //!< Reflection
#define ID_RR 10  //!< Refraction 
#define ID_DP 11  //!< Displacement 
#define NTEXMAPS 12

	struct KeyFrame
	{
		KeyFrame(float* p)
		{
			memcpy_s(&pos,sizeof(KeyFrame),p,sizeof(KeyFrame));
		}
		Matrix4f toMax4x4()
		{
			Matrix4f mat4 = Matrix4f(rot.toMat3());
			mat4[0] *= scale[0];
			mat4[5] *= scale[1];
			mat4[10] *= scale[2];
			memcpy_s(&mat4[12],3 * sizeof(float),&pos,3 * sizeof(float));
			return mat4;
		}
		vec3f 	    pos; // if you're sampling 30fps, you end up with 60 frames for 2s anim 
		vec3f 	    scale;
		Quaternionf rot; // per single object
		unsigned int frameIdx;//idx in the whole frames
	};

	struct KeyFrameContainer
	{
		float			framesPerSecond;	// index = int(framesPerSecond*time_s) % pos.size()
		uint framesCount;
		//vector<Matrix4f> localTM;
		//vector<vec3f> scale;
		vector<KeyFrame> frames;
		int getRealFrameIdx(uint fidx)
		{
			for (int i = 0; i < (int)frames.size(); ++i)
			{
				if( frames[i].frameIdx > fidx && i > 0)
				{
					return i - 1;
				}
			}
			return frames.size() - 1;
		}
	};

	struct bonesInVertex
	{
		int idx4[4];//idx in the bone vec in the mesh file
		float wt4[4];//weight
	};

	struct Skeleton
	{
		vector<bonesInVertex> boneIdxVec;//the whole obj's bones indexing
	};

	class MeshObject : public RenderObject
	{
	public:
		Node* meshObjecNode;
		MeshObject* parent;
		string objectName;
		//for animation
		KeyFrameContainer* localTMAnim;
		Matrix4f localTM;

		
		/*SkinVertex* verticesList;
		uint* indicesList;
		int vboID;
		int iboID;*/
		bool drawAble;


		bool isSkinMesh;
		vector<Skeleton*>* skeletonVec;//means modifier in 3dssmax

		void setParent(MeshObject* p){ parent = p;}

		vector<MeshObject*>* children;
		void addChild(MeshObject* c){children->push_back(c);}

		inline Matrix4f getWorldTM() const 
		{return parent ? localTM * parent->getWorldTM() : localTM;}


		MeshObject(string& nm)
			:RenderObject(TypeCast::stringToString(nm))
			,meshObjecNode(new Node(TypeCast::stringToString(nm) + TEXT("_meshObjNode")))
			,localTMAnim(new KeyFrameContainer()),parent(NULL),isSkinMesh(false)
			,skeletonVec(NULL),children(NULL)
		{}

		~MeshObject()
		{
			delete localTMAnim;

			/*for(size_t i =0;i<matVec->size();++i)
			{
			delete (*matVec)[i];
			}
			delete matVec;*/

			if(batchVec){
				for (size_t i =0; i<batchVec->size();++i)
				{
					delete (*batchVec)[i];
				}
				delete batchVec;
			}

			if(children){
				for (size_t i =0; i<children->size();++i)
				{
					delete (*children)[i];
				}
				delete children;
			}

			if(skeletonVec){
				for (size_t i =0; i<skeletonVec->size();++i)
				{
					delete (*skeletonVec)[i];
				}
				delete skeletonVec;
			}


			 

		}
	};

	struct Bone
	{
		//similar to the animation
		KeyFrame* world2Bone; //this is the world to bone transformation stored in the keyframe struct
		KeyFrameContainer* boneAnim;
		Bone* parent;
		Bone():boneAnim(new KeyFrameContainer()),parent(NULL)
		{}
		~Bone()
		{
			delete boneAnim;
			delete world2Bone;
		}

		inline Matrix4f getAnimWorldTM(int frameIndex) const 
		{return parent ? boneAnim->frames[frameIndex].toMax4x4() * parent->getAnimWorldTM(frameIndex) :  boneAnim->frames[frameIndex].toMax4x4() ;}

	};

	struct Mesh
	{
		vector<MeshObject*>* objectVec;
		vector<Material*>* matVec; 
		vector<Bone*>* boneVec;

		Mesh();
		~Mesh();
		Material* getMaterailByName( string& matName );
		MeshObject* getObjInMeshByName(char* name);
		MeshObject* getChildrenObjByName(char* name, MeshObject* obj);

		Bone* getBoneByIdx(int idx);


	};




	class NBE_API MeshManager :public Singleton<MeshManager>
	{
	public:
		MeshManager();
		~MeshManager();
		Mesh* loadMeshFromFile(const String& fullPath);
		//Mesh* loadMeshFromFile_stream(wchar_t* fileName);
	private:
		vector<Mesh*> m_pMeshVec;//store all the mesh data
		vector<char*> MemoryVec;
	};
}