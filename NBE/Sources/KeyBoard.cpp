#include "Listener.h"
#include "Input.h"
#include "KeyBoard.h"
#include <algorithm>
#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"
namespace NBE
{
	KeyBoard::KeyBoard():Input(KEYBOARD)
	{}
	KeyBoard::~KeyBoard()
	{
		m_keyMap.clear();
	}

	void KeyBoard::update()
	{
		//check the pressed key and add event every frame
		std::for_each(begin(m_keyMap),end(m_keyMap),[&](std::pair<int, bool> item){
			if(item.second) 
				addEvent(new KeyEvent(  KEY_DOWN,item.first));//key down every frame
		});
		
	}
	void KeyBoard::processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch( message )
		{
		case WM_KEYDOWN:
			if(!m_keyMap[(int)wParam]) 
			{
				addEvent(new KeyEvent(KEY_PRESS,(int)wParam));//key pressed once
			}
			m_keyMap[(int)wParam] = true;
			break;
		case WM_KEYUP:
			m_keyMap[(int)wParam] = false;
			addEvent(new KeyEvent(KEY_RELEASE,(int)wParam));//key release once
			break;
		case WM_CHAR:
			addEvent(new KeyEvent(KEY_CHAR,(int)wParam));//key pressed once
			break;
		}
		  

	}
	////---- Keyboard related event implementation

}