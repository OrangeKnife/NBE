#include "Dx11DeviceManager.h"
#include "MyAssert.h"
#define REQUIRES myassert

namespace NBE
{


	uint Dx11DeviceManager::addDevice(ID3D11Device* dev)
	{
		REQUIRES(dev!=NULL);
		m_deviceVec.push_back(dev);
		return m_deviceVec.size()-1;
	}

	ID3D11Device* Dx11DeviceManager::getDevice( uint index )
	{
		REQUIRES(index<m_deviceVec.size());
		return m_deviceVec[index];
	}

	Dx11DeviceManager::~Dx11DeviceManager()
	{
		//release all devices
		vector<ID3D11Device*>::iterator it = m_deviceVec.begin();
		for (;it!=m_deviceVec.end();)
		{
			//delete (*it);
			(*it)->Release();
			it = m_deviceVec.erase(it);
		}

	}

}