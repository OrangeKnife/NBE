#include "NBEWindow.h"
namespace NBE
{
	NBEWindow::NBEWindow(RenderInfo* ri, HINSTANCE h)
		:RendererWindow(h), m_renderInfo(ri)
	{
		registerClass(h);

		DWORD style, exStyle;
		RECT _windowRect = { 0, 0, LONG(ri->width), LONG(ri->height) };

		if (ri->fullScreen)
		{
			style = WS_POPUP;
			exStyle = WS_EX_TOPMOST | WS_EX_APPWINDOW;
			HDC screen = GetDC(0);
			_windowRect.right = GetDeviceCaps(screen, HORZRES);
			_windowRect.bottom = GetDeviceCaps(screen, VERTRES);
		}
		else
		{
			style = WS_OVERLAPPEDWINDOW;
			exStyle = WS_EX_APPWINDOW;
		}

		AdjustWindowRectEx(&_windowRect, style, false, exStyle);

		m_hwnd = CreateWindowEx(exStyle, ri->className, ri->title, style,
			CW_USEDEFAULT, CW_USEDEFAULT, (_windowRect.right - _windowRect.left),
			(_windowRect.bottom - _windowRect.top), NULL, NULL, h, NULL);


		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);



		if (!m_hwnd)
		{
			throw NException(CreateWindowError, String(TEXT("Create Window Error")));
		}


	}
	NBEWindow::~NBEWindow()
	{
		//delete m_renderInfo;
	}
	LRESULT CALLBACK NBEWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//static func
		PAINTSTRUCT ps;
		HDC hdc;
		//if(s_win!=nullptr)
		//{
		//	s_win->procMsg(hWnd,message,wParam,lParam);// custom msg processing
		//}

		procMsg(hWnd, message, wParam, lParam);


		switch (message)
		{
		case WM_CREATE:
			return 0;
		case WM_SIZE:
			return 0;
		case WM_COMMAND:
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	ATOM NBEWindow::registerClass(HINSTANCE hInstance)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;//MAKEINTRESOURCE(IDC_THREADING);
		wcex.lpszClassName = m_renderInfo->className;
		wcex.hIconSm = nullptr;

		return RegisterClassEx(&wcex);
	}

	void NBEWindow::procMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		for (size_t i = 0; i < Input::getTotalInputNum(); ++i)
		{
			Input::getInput(i)->processMessages(hWnd, message, wParam, lParam);
		}
	}
	 
}