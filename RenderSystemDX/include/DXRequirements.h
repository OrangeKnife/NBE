#pragma once


#include <d3d11.h>
//#include <d3dx11.h>
#include <d3dcompiler.h>


#pragma comment(lib,"d3d11.lib")
//#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dcompiler.lib")

//#ifdef _DEBUG
#include <cassert>
#define HV(rt) if(FAILED(rt)){assert(0);}
//#endif

#include "Error.h"
#define Exit(errorStr) { errorMsg(errorStr,"Exit"); exit(0);}
#include "d3dx11effect.h"
//#include "d3dx11dbg.h"
#pragma comment(lib,"Effects11.lib")


#pragma comment (lib, "NBE.lib")//important! compile the core engine, then the renderer
