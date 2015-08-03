#pragma once
#include "DataTypes.h"
#include <cassert>
#include "GLheader.h"
namespace NBE
{
	class GLTypes
	{
	public:
		static unsigned int Map(unsigned int NBE_val)
		{
			switch (NBE_val)
			{
			case NBE_COLOR_RGB:return GL_RGB;
			case NBE_COLOR_RGBA:return GL_RGBA;
			case NBE_UNSIGNED_INT:return GL_UNSIGNED_INT;
			case NBE_UNSIGNED_BYTE:return GL_UNSIGNED_BYTE;
			case NBE_FLOAT:return GL_FLOAT;
			case NBE_INT: return GL_INT;
			default:
				assert(0);//define your types!!
			}
			return -1;//impossible
		}
	};

}