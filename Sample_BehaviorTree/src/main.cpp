#include "BehaviorTree.h"
#include <tchar.h>
#include <windows.h>
using namespace std;
using namespace NBE;


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	//_CrtSetBreakAlloc(817);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	BehaviorTreeApp* app = nullptr;
	try
	{
		app = new BehaviorTreeApp(hInstance);
		app->initTex();
		app->loadMaps();//load the simulation.txt, load the num of all the resources in the map
		app->run();
	}
	catch (NBEException& e)
	{
		if(app)
			delete app;
		::MessageBox(nullptr, e.getInfo().c_str(), TEXT("Exception"), MB_OK | MB_ICONERROR);
	}
	catch (exception& e)
	{
		if (app)
			delete app;
		::MessageBoxA(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR);
	}
	if (app)
		delete app;
	return 0;
}


