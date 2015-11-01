#include "Input.h"
#include "KeyBoard.h"

#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"
#include "EventHub.h"
namespace NBE
{
	KeyBoard::KeyBoard():Input(KEYBOARD)
	{}
	KeyBoard::~KeyBoard()
	{}

	void KeyBoard::update()
	{
		//check the pressed key and add event every frame
		for (auto i = 0; i < 255; ++i)
		{
			if (m_keyMap[i])
			{
				NEventHub<NEvent_Key>::getPtr()->queueEvent(NEvent_Key(i, NEvent_Key::KEY_DOWN)); //key down event
				//NLOG("here is : %d \n", i);
			}
		}

		NEventHub<NEvent_Key>::getPtr()->fireAllQueuedEvents();
		
	}

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	void KeyBoard::processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch( message )
		{
		case WM_KEYDOWN:
			if(!m_keyMap[(int)wParam]) 
			{
				NEventHub<NEvent_Key>::getPtr()->fire(NEvent_Key((int)wParam, NEvent_Key::KEY_PRESSED)); //key pressed event
			}
			m_keyMap[(int)wParam] = true;
			break;
		case WM_KEYUP:
			m_keyMap[(int)wParam] = false;
			NEventHub<NEvent_Key>::getPtr()->fire(NEvent_Key((int)wParam, NEvent_Key::KEY_UP)); //key up event
			break;
		case WM_CHAR:
			NEventHub<NEvent_Key>::getPtr()->fire(NEvent_Key((int)wParam, NEvent_Key::KEY_CHAR)); //key pressed event
			break;
		}
		  

	}
	////---- Keyboard related event implementation
#endif

}