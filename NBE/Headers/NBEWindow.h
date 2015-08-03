#pragma once
#include "Listener.h"
#include "Input.h"
#include "RendererWindow.h"
#include "RenderInfo.h"

#include "Exception.h"
namespace NBE
{
	class NBE_API NBEWindow : public RendererWindow
	{
	public:
		NBEWindow(RenderInfo* ri, HINSTANCE h); 
		~NBEWindow();
	private:
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		
		ATOM registerClass(HINSTANCE hInstance);
		static void procMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		RenderInfo* m_renderInfo;
	};
	




}