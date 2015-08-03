#pragma once
#ifdef _DEBUG
#include "NBE.h"

#if defined(_WIN32) || defined(WIN32)

#include <Windows.h>

#define myassert(exp)\
	if(!(exp))MyAssert::Msg(__FILE__,__LINE__,#exp);

namespace NBE
{
	class NBE_API MyAssert
	{
	public:
		static void Msg(char* file, int line, const char* str)
		{
			char* linestr = new char[11];
			sprintf_s(linestr,10,"%d",line); 
			 
			std::string output = std::string("file: ") + std::string(file) + std::string("\nline: ") +
				std::string(linestr) + std::string("\n") + std::string(str);
			delete linestr;

			::MessageBoxA(nullptr , output.c_str(),"Assert",MB_OK|MB_ICONERROR);
		}
	};
}
#else 
#define myassert(exp) 
#endif

#endif