#pragma once
//I dont think this will work for another project, so just ignore it
/*
#include "NBE.h"
#include <windows.h>
namespace NBE
{
	class NBE_API DllLoader
	{
	public:
		DllLoader():
		  m_dll(nullptr)
		{}
		~DllLoader()
		{
		FreeLibrary(m_dll);
		}
		bool load(String& dllName)
	{
		m_dll = LoadLibrary(dllName);//load dll
		return m_dll != nullptr;
	}

		void* getProcAddress(char* funcName)
	{
		typedef void* (__cdecl* FuncPointer)(HINSTANCE h);
		FuncPointer ptr_someFunc;
		ptr_someFunc = (FuncPointer) GetProcAddress(m_dll,funcName);
		if(ptr_someFunc == nullptr)
		{
			throw NException(CantGetFuncInDll);
		}

		return ptr_someFunc;
	}
	private:
		HINSTANCE m_dll;
	};
}

*/