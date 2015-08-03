#include <tchar.h>
#include "game.h"
#include <cassert>
using namespace std;
using namespace NBEANIMATOIN;
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	//_CrtSetBreakAlloc( 359  );

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	try
	{
		shared_ptr<Game> GameInstance(new Game(hInstance));
		GameInstance->run();
	}
	catch(NBEException& e)
	{
		//inform user
		//shared_ptr<char> tempChar(TypeCast::intToChar( e.ErrorCode() ));
		//::MessageBoxA(nullptr, tempChar.get(),"Error Code",MB_OK|MB_ICONERROR);

		::MessageBoxW(nullptr,e.getInfo().c_str(),L"Exception",MB_OK|MB_ICONERROR); 
	}

	return 0;
}