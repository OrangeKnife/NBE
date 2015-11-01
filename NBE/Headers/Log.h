#pragma once
namespace NBE
{
#define NLOG(strformat,arg) \
	{char hahatemp[256]; sprintf_s(hahatemp,256, strformat, arg); \
	::OutputDebugStringA(hahatemp);}
}