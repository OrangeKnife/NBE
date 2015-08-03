#pragma once

#ifndef NBEIMPORT
#define NBE_API __declspec(dllexport)
#else
#define NBE_API __declspec(dllimport)
#endif

#include <string>
typedef std::wstring String;


#pragma warning(disable:4251)



//pre declare
namespace NBE
{
class Node;
class Object;
class RenderObject;
class RenderSystem;
class RendererGL;
}