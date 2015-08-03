#pragma once
#include <sstream>
#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#define  errorMsg( _text, _title )\
{\
	std::wstringstream text,title;\
	text<< _text << std::endl;\
	title<< _title << std::endl;\
	MessageBox(NULL, text.str().c_str(), title.str().c_str(),MB_OK|MB_ICONERROR);\
}
#else
#define  errorMsg( _text, _title )\
{\
	std::wstringstream text, title; \
	text << _text << std::endl; \
	title << _title << std::endl; \
	std::cout<<_title<<_text;\
}
#endif
//TODO exception