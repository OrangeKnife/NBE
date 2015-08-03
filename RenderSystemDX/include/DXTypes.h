#pragma once
#include "DataTypes.h"
#include "DXRequirements.h"
namespace NBE
{
	class DXTypes
	{
	public:
		static unsigned int Map(unsigned int NBE_val)
		{
			switch (NBE_val)
			{
			case NBE_COLOR_RGB:return NBE_COLOR_RGB;
			case NBE_COLOR_RGBA:return DXGI_FORMAT_R8G8B8A8_UNORM;
			case NBE_UNSIGNED_INT:return NBE_UNSIGNED_INT;
			case NBE_UNSIGNED_BYTE:return NBE_UNSIGNED_BYTE;
			case NBE_R32_FLOAT:return DXGI_FORMAT_R32_FLOAT;
			case NBE_R32G32B32A32_FLOAT:return DXGI_FORMAT_R32G32B32A32_FLOAT;
			default:
				return NBE_ERROR;
			}
			
		}
	};
}