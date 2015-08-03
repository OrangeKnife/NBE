#include "MyAssert.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"


#include "Vector3.h"
#include "Vector2.h"
#include "Listener.h"
#include "Input.h"
#include "Mouse.h"

namespace NBE
{
	Mouse::Mouse():Input(MOUSE)
	{
		for(UINT i = 0; i < MAX_BUTTONS; ++i)
		{
			m_buttonStates[i] = m_lastButtonStates[i] = false;
		}
		wheelDelta = 0;
	}
	Mouse::~Mouse()
	{}
	void Mouse::init(HWND hwnd)
	{
		m_hWnd = hwnd;
	}
	void Mouse::calculateCenter(HWND hwnd)
	{

		
		GetClientRect( hwnd, &cRect );
		GetWindowRect(hwnd, &wRect);
		border_thickness = (wRect.right - wRect.left - cRect.right) /2;

		LONG MOUSE_CENTER_X = cRect.right / 2 + border_thickness + wRect.left; 
		LONG MOUSE_CENTER_Y = (cRect.bottom - cRect.top +45) / 2 + wRect.top;
		setCent(MOUSE_CENTER_X, MOUSE_CENTER_Y);
		//::SetCursorPos(MOUSE_CENTER_X,MOUSE_CENTER_Y);
		//::ShowCursor(false);
	}

	void Mouse::setCent(LONG _X, LONG _Y)
	{
		m_lastX = _X;
		m_lastY = _Y;
	}

	void Mouse::update()
	{
		POINT mousePos;
		::GetCursorPos(&mousePos);

		//how fast did the mouse move?

		if(didGoDown(RIGHT_BUTTON) || didGoDown(MIDDLE_BUTTON) || didGoDown(LEFT_BUTTON))
		{
			m_lastY = mousePos.y;
			m_lastX = mousePos.x;
		}

		long x = (long)(mousePos.x - ( wRect.left + border_thickness)); 
		long y = (long)(mousePos.y - (wRect.top + 30 )) ;

		const float MOUSE_ROTATION_SCALAR =  -0.1f ;
		vec3f mouseDelta( MOUSE_ROTATION_SCALAR * (mousePos.y-m_lastY),
			MOUSE_ROTATION_SCALAR * (mousePos.x-m_lastX),0);

		if(isDown(RIGHT_BUTTON) && (mouseDelta.x != 0 || mouseDelta.y != 0))
		{
			addEvent(new MouseEvent(MOUSE_RIGHTDRAG,mouseDelta,x,y)); //move event
			::SetCursorPos(m_lastX,m_lastY);
		}
		else if(isDown(MIDDLE_BUTTON) && (mouseDelta.x != 0 || mouseDelta.y != 0))
		{
			addEvent(new MouseEvent(MOUSE_MIDDLEDRAG,mouseDelta * 0.1f,x,y)); //move event
			::SetCursorPos(m_lastX,m_lastY);
		}
#if 0
		else if(isDown(LEFT_BUTTON) && (mouseDelta.x != 0 || mouseDelta.y != 0))
		{
			addEvent(new MouseEvent(MOUSE_LEFTDRAG,mouseDelta  ,x,y)); //move event
			::SetCursorPos(m_lastX,m_lastY);
		}
#endif
		else if(mouseDelta.x != 0 || mouseDelta.y != 0)
		{
			addEvent(new MouseEvent(MOUSE_MOVE,mouseDelta,x,y)); //move event
		}
		
		
		




		//

		for(UINT i = 0; i < MAX_BUTTONS; ++i)
		{
			if(m_buttonStates[ i ] && !m_lastButtonStates[ i ])
			{
				 addEvent(new MouseEvent(MOUSE_BUTTON_PRESS,i,x,y)); // press button once
			}
		
			if(m_buttonStates[ i ])
				addEvent(new MouseEvent(MOUSE_BUTTON_DOWN,i,x,y));
			else if(!m_buttonStates[ i ] && m_lastButtonStates[ i ])
				addEvent(new MouseEvent(MOUSE_BUTTON_RELEASE,i,x,y));


			m_lastButtonStates[i] = m_buttonStates[i];
		}


		//wheel
		if(wheelDelta!=0){
			addEvent(new MouseEvent(MOUSE_WHEEL,vec3f(wheelDelta,0,0),x,y));
			wheelDelta = 0;
		}
	}
	void Mouse::processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int bt = -1;
		bool btV;
		switch( message )
		{
		case WM_LBUTTONDOWN:
			bt = LEFT_BUTTON;btV = true;
			break;
		case WM_LBUTTONUP:
			bt = LEFT_BUTTON;btV = false;
			break;
		case WM_RBUTTONDOWN:
			bt = RIGHT_BUTTON;btV = true;
			break;
		case WM_RBUTTONUP:
			bt = RIGHT_BUTTON;btV = false;
			break;
		case WM_MBUTTONDOWN:
			bt = MIDDLE_BUTTON;btV = true;
			break;
		case WM_MBUTTONUP:
			bt = MIDDLE_BUTTON;btV = false;
			break;
		case WM_MOUSEWHEEL:
			wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		//TODO :(, need to move this 2 msg out of here :(
		case WM_SIZE:
		case WM_MOVE:
			calculateCenter(hWnd);
			return;
		default:
			return;
		}
		if(bt>=0)
		 m_buttonStates[bt] = btV;

		   
	}

	unsigned int Mouse::getNumOfButtons() const
	{
		return MAX_BUTTONS;
	}
	
	vec2f Mouse::getPosition()
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(m_hWnd,&mousePos);
		currentPos.x = static_cast<float>(mousePos.x);
		currentPos.y = static_cast<float>(mousePos.y);
		return currentPos;
	}
	bool Mouse::didGoUp( MouseButton button) const
	{
		myassert( button < MAX_BUTTONS );
		return !m_buttonStates[ button ] && m_lastButtonStates[ button ];
	}
	bool Mouse::didGoDown( MouseButton button ) const
	{
		myassert( button < MAX_BUTTONS );
		return m_buttonStates[ button ] && !m_lastButtonStates[ button ];
	}
	bool Mouse::isDown( MouseButton button ) const
	{
		myassert( button< MAX_BUTTONS);
		return m_buttonStates[ button ];
	}


	////----Mouse related event implementation
	


}