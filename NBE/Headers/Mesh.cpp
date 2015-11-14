#define NOMINMAX
#include "Mesh.h"
#include "File.h"
#include <sstream>
#include "TextureManager.h"

using namespace NBE;

MeshManager::MeshManager()
{
	//m_pMeshVec = new vector<Mesh*>();
}

MeshManager::~MeshManager()
{
	vector<Mesh*>::iterator it = m_pMeshVec.begin();
	for (;it!=m_pMeshVec.end();)
	{
		delete (*it);
		it = m_pMeshVec.erase(it);
	}
	//delete m_pMeshVec;
}


/*
Mesh* MeshManager::loadMeshFromFile_stream(wchar_t* fileName)
{



	ifstream* fs = FileIO::readFile(wstring(fileName));

	size_t totalSize;
	fs->read((char*)&totalSize, sizeof(size_t));

	char* totalMemory = new char[totalSize];
	MemoryVec.push_back(totalMemory);
	fs->read((char*)totalMemory, totalSize);
	size_t* headerOffsets = (size_t*)&totalMemory[0];

	size_t offset = sizeof(size_t) * 9;

	Mesh* ms = (Mesh*)&totalMemory[offset + headerOffsets[0]];
	ms->fix(totalMemory + offset, headerOffsets);

	fs->close();
	delete fs;


	//FileIO::parseLoop(wstring(fileName),function<void(string&)>(parseFunc));
	m_pMeshVec.push_back(ms);//support multiple Mesh file

	return ms;
}
*/
/*
Mesh* MeshManager::loadMeshFromFile(const String& fullPath)
{
	String currentPath = fullPath.substr(0,fullPath.find_last_of('\\')+1);

	Mesh* ms = NULL;


	string tag;// [tag]


	vector<Batch*>* batchVec;
	vector<MeshObject*>* objectVec = new vector<MeshObject*>();
	vector<Material*>* matVec = new vector<Material*>();
	vector<Bone*>* boneVec = new vector<Bone*>();



	ms = new Mesh();
	ms->objectVec = objectVec;
	ms->matVec = matVec;
	ms->boneVec = boneVec;


	ifstream* fs = FileIO::readFile(fullPath);
	char* buffer = new char[128];//this buffer used to read tags
	auto readAline =
		[&]()
	{
		fs->getline(buffer,128);
	};



	while (!fs->eof())
	{
		readAline();

		string line = string(buffer); 
		if (buffer[0] == '[')
		{
			size_t blacket = line.find_last_of(']');
			tag = line.substr(1,blacket-1);// ] & chr

			if (tag.length()>1)
			{
				stringstream lineStream;

				if(tag == string("object"))
				{
					readAline();

					MeshObject* obj = new MeshObject(string(buffer));
					
					obj->meshObjecNode->attachObject(obj);
					//matVec = new vector<Material*>();
					

					//obj->matVec = matVec;
					batchVec = obj->batchVec;

					obj->children = new vector<MeshObject*>();

					obj->skeletonVec = new vector<Skeleton*>();

					readAline();//read a tag

					if (memcmp(buffer,"[parent]",8) == 0)
					{
						readAline();
						auto findParent = ms->getObjInMeshByName(buffer);
						if(  findParent == NULL )
						{
							throw NException(MeshLoadError,String(TEXT("MeshObject has a unknown parent")));
						}
						else
						{
							obj->setParent(findParent);
							findParent->addChild(obj);
						}
						//auto it = objectVec->begin();
						//for (;it!= objectVec->end();++it)
						//{
						//	if ((*it)->objectName == string(buffer))
						//	{
						//		obj->setParent(*it);
						//		(*it)->addChild(obj);
						//		break;
						//	}
						//}
						//if(it == objectVec->end())
						//	errorMsg("MeshObject has a unknown parent","mesh data error");
						readAline();


					}
					else
					{
						objectVec->push_back(obj);//ok done
					}

					//[animation] means this MeshObject is a normal one
					if(memcmp(buffer,"[animation]",11) == 0)
					{
						fs->read(buffer,sizeof(int));
						int framesCount = *(int*)buffer;
						obj->localTMAnim->framesCount = framesCount;
						fs->read(buffer,sizeof(int));
						int realFrameDataCount = *(int*)buffer;
						obj->localTMAnim->framesPerSecond = 30;
						obj->localTMAnim->frames.reserve(realFrameDataCount);
						for (int i = 0; i<realFrameDataCount; ++i)
						{
							fs->read(buffer,sizeof(KeyFrame));

							obj->localTMAnim->frames.push_back(KeyFrame((float*)buffer));
						}


						obj->meshObjecNode->setLocalTM(obj->localTMAnim->frames[0].toMax4x4());
						//	vertices num indices Num
						uint vcount = 0;
						uint icount = 0;

						fs->read((char*)&obj->VertexNum,sizeof(uint));
						fs->read((char*)&obj->IndexNum,sizeof(uint));
						uint verticesNum = obj->VertexNum;
						uint indicesNum = obj->IndexNum;
						obj->vbo = new PNCT_Vertex[verticesNum];
						//obj->indicesList = new uint[indicesNum];
						PNCT_Vertex* pStart = static_cast<PNCT_Vertex*>(obj->vbo);
						while(vcount < (unsigned int)verticesNum)
						{
							fs->read((char*)(&pStart[vcount].pos),sizeof(vec3f));
							fs->read((char*)(&pStart[vcount].normal),sizeof(vec3f));
							fs->read((char*)(&pStart[vcount].color),sizeof(vec4f));
							fs->read((char*)(&pStart[vcount].texcoord),sizeof(vec2f));
							++vcount;
						}

						uint tempIntArray[3];
						while (icount < (uint)indicesNum)
						{
							fs->read((char*)tempIntArray,sizeof(uint)*3);

							if((int)tempIntArray[0] == -1)//means use mtl
							{
								Batch* bat = new Batch();  
								int MtlIdxInMtlLib = (int)tempIntArray[1];
								if(MtlIdxInMtlLib >= 0)
								{
									bat->pMtl = (*ms->matVec)[MtlIdxInMtlLib];
								}
								else
								{
									bat->pMtl = nullptr;
								}


								bat->startIndex = icount;
								bat->size = indicesNum - bat->startIndex;
								if (batchVec->size()>0)
								{
									Batch* tempFp = (*batchVec)[batchVec->size()-1];
									tempFp->size = icount - tempFp->startIndex;
									//this is a little tricky, but I don't need to count the number of indices using in the current material(in one obj)
									//every time add a batch, change the former batch size; so only when next batch is recoreded,the former
									//size is correct; the last batch size is right cuz indicesNum 
								}
								batchVec->push_back(bat); // this batchVec belongs to an obj, which get from -2 see codes down

							}
							//else if ((int)tempIntArray[0] == -2)
							//{
							//	int lenOfObjName = (int)tempIntArray[1];
							//	fs->read(buffer,lenOfObjName);
							//	buffer[lenOfObjName] = 0;

							//	if (batchVec->size()>0)
							//	{
							//		MeshBatch* tempFp = (*batchVec)[batchVec->size()-1];
							//		tempFp->size = icount - tempFp->startIndex;
							//		//this is a little tricky, but I don't need to count the number of indices using in the current material(in one obj)
							//		//every time add a batch, change the former batch size; so only when next batch is recoreded,the former
							//		//size is correct; the last batch size is right cuz indicesNum 
							//	}
							//	batchVec = ms->getObjInMeshByName(buffer)->batchVec ;
							//}
							else
							{
								obj->ibo.push_back(tempIntArray[0]);
								obj->ibo.push_back(tempIntArray[1]);
								obj->ibo.push_back(tempIntArray[2]);
								//memcpy_s(&(obj->indicesList[icount]),3*sizeof(uint),tempIntArray,3*sizeof(uint));
								icount += 3;

							}

						}

					}

					//read all the skeleton if this MeshObject is a skin mesh object
					if (memcmp(buffer,"[skeleton]",11) == 0)
					{
						//skeleton means it is a skin mesh, so read the index of the bones
						obj->isSkinMesh = true;

						Skeleton* sk = new Skeleton();
						obj->skeletonVec->push_back(sk);
						//	vertices num indices Num
						uint vcount = 0;
						uint icount = 0;

						fs->read((char*)&obj->VertexNum,sizeof(uint));
						fs->read((char*)&obj->IndexNum,sizeof(uint));
						uint verticesNum = obj->VertexNum;
						uint indicesNum = obj->IndexNum;
						obj->vbo = new SkinVertex[verticesNum];
						//obj->indicesList = new uint[indicesNum];
						SkinVertex* pStart = static_cast<SkinVertex*>(obj->vbo);
						while(vcount < (unsigned int)verticesNum)
						{
							

							fs->read((char*)(&pStart[vcount].pos),sizeof(vec3f));
							fs->read((char*)(&pStart[vcount].normal),sizeof(vec3f));
							fs->read((char*)(&pStart[vcount].color),sizeof(vec4f));
							fs->read((char*)(&pStart[vcount].texcoord),sizeof(vec2f));

							bonesInVertex biv;
							fs->read((char*)(biv.idx4),sizeof(int) * 4);
							fs->read((char*)(biv.wt4),sizeof(float) * 4);
							sk->boneIdxVec.push_back(biv);

							pStart[vcount].boneIdx = vec4<int>(biv.idx4);
							pStart[vcount].weight = vec4f(biv.wt4);

							++vcount;
						}

						uint tempIntArray[3];
						while (icount < (uint)indicesNum)
						{
							fs->read((char*)tempIntArray,sizeof(uint)*3);

							if((int)tempIntArray[0] == -1)//means use mtl
							{
								Batch* bat = new Batch();  

								int MtlIdxInMtlLib = (int)tempIntArray[1];
								if(MtlIdxInMtlLib >= 0)
								{
								 
									bat->pMtl = (*ms->matVec)[MtlIdxInMtlLib];
								}
								else
								{
									bat->pMtl = nullptr;
								}



								bat->startIndex = icount;
								bat->size = indicesNum - bat->startIndex;
								if (batchVec->size()>0)
								{
									Batch* tempFp = (*batchVec)[batchVec->size()-1];
									tempFp->size = icount - tempFp->startIndex;
									//this is a little tricky, but I don't need to count the number of indices using in the current material(in one obj)
									//every time add a batch, change the former batch size; so only when next batch is recoreded,the former
									//size is correct; the last batch size is right cuz indicesNum 
								}
								batchVec->push_back(bat); // this batchVec belongs to an obj, which get from -2 see codes down

							}
							else
							{
								obj->ibo.push_back(tempIntArray[0]);
								obj->ibo.push_back(tempIntArray[1]);
								obj->ibo.push_back(tempIntArray[2]);
								//memcpy_s(&(obj->indicesList[icount]),3*sizeof(uint),tempIntArray,3*sizeof(uint));
								icount += 3;

							}
						}

					}










				}
				else if (tag == string("material"))
				{
					//build the material lib
					readAline(); 
					Material* mat = new Material(string(buffer));  
					matVec->push_back(mat);

					int numOfbmp;
					fs->read((char*)(&numOfbmp),sizeof(int));

					for (int i = 0; i < numOfbmp; ++i)
					{
						readAline();
						line = string(buffer);

						string tagID = line.substr(1,line.find_last_of(']')-1);// ] & chr
						if (tagID.find(string("ID_")) != string::npos) // this is a texture! ID_ as the beginning
						{
							//ID_XX_Y
							//XX is the type of the texture, Y is the enum of the texture
							readAline();
							lineStream = stringstream(buffer);

							TexMap* mp = new TexMap();
							mp->texMapFileName = string(buffer);
							int idxOf_ = tagID.find_last_of('_')+1;
							string IDofTexSlot = tagID.substr(idxOf_);
							mp->texMapSlot = atoi(IDofTexSlot.c_str());
							String texMapFileNameStr = TypeCast::stringToString(mp->texMapFileName);
							mp->texId = TextureManager::getInstancePtr()->LoadFromFile(currentPath + texMapFileNameStr, texMapFileNameStr)->textureIdx;
							mat->texMapVec->push_back(mp);
						}
					}
				}
				else if (tag == string("bone"))
				{
					//read every bone animation in the scene like the animation for the object
					//[bone]
					Bone* bo = new Bone();

					fs->read(buffer,sizeof(int));
					bo->parent =  *(int*)buffer >= 0 ? ms->getBoneByIdx( *(int*)buffer ): NULL; // -1 means no parent

					fs->read(buffer,sizeof(int));
					int framesCount = *(int*)buffer;
					bo->boneAnim->framesPerSecond = 30;
					bo->boneAnim->frames.reserve(framesCount);

					fs->read(buffer,sizeof(KeyFrame));
					bo->world2Bone = new KeyFrame((float*)buffer);//rest TM

					for (int i = 0; i<framesCount; ++i)
					{
						fs->read(buffer,sizeof(KeyFrame));
						bo->boneAnim->frames.push_back(KeyFrame((float*)buffer));
					}
					ms->boneVec->push_back(bo);
				}


			}
		}
	}	
	fs->close();
	delete fs;
	delete[] buffer;

	//FileIO::parseLoop(wstring(fileName),function<void(string&)>(parseFunc));
	m_pMeshVec.push_back(ms);

	return ms;
}
*/

StaticMesh* MeshManager::loadStaticMeshFromFile(const String& fullPath)
{
	String currentPath = fullPath.substr(0, fullPath.find_last_of('\\') + 1);
	ifstream* fs = FileIO::readFile(fullPath);
 
	std::streampos fileSize;

	fs->seekg(0, std::ios::end);
	fileSize = fs->tellg();

	StaticMesh* newMesh = new StaticMesh("");
	newMesh->rawData = new char[(unsigned int)fileSize];

	fs->seekg(0, std::ios::beg);
	fs->read(newMesh->rawData, fileSize*sizeof(char));
	fs->close();
		
	newMesh->data = NBESTATICMESH::GetMutableStaticMeshData(newMesh->rawData);
	newMesh->reName(newMesh->data->staticMeshName()->str());

	for (uint i = 0; i < newMesh->data->materials()->size(); ++i)
	{
		auto currentMat = newMesh->data->materials()->Get(i);
		for (uint j = 0; j < currentMat->textureMaps()->size(); ++j)
		{
			auto currentTex = currentMat->textureMaps()->Get(j);
			String texMapFileName = TypeCast::stringToString( currentTex->texMapFileName()->str() );
			const_cast<NBESTATICMESH::TexMap*>(currentTex)->mutate_texId( TextureManager::getInstancePtr()->LoadFromFile(currentPath + texMapFileName, texMapFileName)->textureIdx );
			auto tsta=currentTex->texId();
		}
	}
	
	delete fs;

	m_pMeshVec.push_back(newMesh);

	return newMesh;
}

/*

Mesh::Mesh():boneVec(NULL),matVec(NULL),objectVec(NULL)
{}
Mesh::~Mesh()
{
	if(matVec)
		for(size_t i =0;i<matVec->size();++i)
		{
			delete (*matVec)[i];
		}
		delete matVec;

		if(objectVec)
			for(size_t i =0;i<objectVec->size();++i)
			{
				if((*objectVec)[i]->parent == NULL)
				{
					delete (*objectVec)[i];
				}
			}
			delete objectVec;
			objectVec = NULL;

			if(boneVec)
				for (size_t i = 0; i < boneVec->size();++i)
				{
					delete (*boneVec)[i];
				}
				delete boneVec;

}

Material* Mesh::getMaterailByName( string& matName )
{
	auto matIt = matVec->begin();
	for (;matIt!= matVec->end();++matIt)
	{
		if((*matIt)->matName == string(matName))
			return (*matIt);
	}
	return NULL;
}

MeshObject* Mesh::getChildrenObjByName(char* name, MeshObject* obj)
{
	MeshObject* rt = NULL;
	auto it = obj->children->begin();
	for (;it!= obj->children->end();++it)
	{
		if ((*it)->objectName == string(name))
		{
			return (*it);
		}

		if ((*it)->children->size() > 0)
		{
			rt = getChildrenObjByName( name,(*it));
		}
	}
	return rt;
}

MeshObject* Mesh::getObjInMeshByName( char* name )
{
	MeshObject* rt = NULL;
	auto it = objectVec->begin();
	for (;it!= objectVec->end();++it)
	{
		if ((*it)->objectName == string(name))
		{
			return (*it);
		}
		//check children if itself is not the node
		if ((*it)->children->size() > 0)
		{
			rt = getChildrenObjByName( name,(*it));
		}
	}
	return rt;
}

Bone* Mesh::getBoneByIdx( int idx )
{
	if(idx < (int)boneVec->size())
		return (*boneVec)[idx];
	else
		return NULL;
}

*/