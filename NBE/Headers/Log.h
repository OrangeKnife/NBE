#pragma once
namespace NBE
{
#define NLOG(strformat,arg) \
	{char temp[256]; sprintf_s(temp,256, strformat, arg); \
	::OutputDebugStringA(temp);}
}