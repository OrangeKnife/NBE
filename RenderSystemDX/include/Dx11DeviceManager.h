#pragma once
#include "DXRequirements.h"
#include <vector>
#define uint unsigned int
using namespace std;
namespace NBE
{
	class Dx11DeviceManager
	{
	public:
		~Dx11DeviceManager();
		uint addDevice(ID3D11Device* dev);
		ID3D11Device* getDevice(uint index);
	private:
		vector<ID3D11Device*> m_deviceVec;
	};
}