#pragma once

#ifndef NBEIMPORT
#define NBE_API __declspec(dllexport)
#else
#define NBE_API __declspec(dllimport)
#endif

#include <string>
#ifdef UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif

#pragma warning(disable:4251)

#if 1//USE_LOG
#include "Log.h"
#endif

//pre declare
namespace NBE
{
class Node;
class Object;
class RenderObject;
class RenderSystem;
class RendererGL;
}