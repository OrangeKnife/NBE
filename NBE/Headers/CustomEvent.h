#pragma once
#include "Event.h"
namespace NBE
{
	struct NEvent_Mouse: NEvent
    {
		enum NEvent_Mouse_Type {
			MOUSE_NONE = 0,
			MOUSE_MOVE,
			MOUSE_LEFTDRAG,
			MOUSE_RIGHTDRAG,
			MOUSE_MIDDLEDRAG,
			MOUSE_BUTTON_DOWN,
			MOUSE_BUTTON_PRESS,
			MOUSE_BUTTON_RELEASE,
			MOUSE_WHEEL
		};
		NEvent_Mouse() {}
		NEvent_Mouse(NEvent_Mouse_Type _t, vec3f& delta, long currentX = 0, long currentZ = 0) : mouseEventType(_t), deltaMove(delta), bt(0),
			pos(float(currentX), float(currentZ)) {}
		NEvent_Mouse(NEvent_Mouse_Type _t,unsigned int _bt,long currentX, long currentZ) : mouseEventType(_t), deltaMove(vec3f()), bt(_bt),
			pos(float(currentX), float(currentZ)) {}
		bool operator==(NEvent_Mouse& rh)
		{
			return mouseEventType == rh.mouseEventType && bt == rh.bt;
		}
		NEvent_Mouse_Type mouseEventType;
        vec3f deltaMove;
        unsigned int bt;
        vec2f pos;
    };

	struct NEvent_Key : NEvent
	{
		enum NEvent_Key_Type {
			KEY_NONE = 0,
			KEY_DOWN,
			KEY_UP,
			KEY_PRESSED, //down and up
			KEY_CHAR
		};
		int KeyValue;
		NEvent_Key_Type KeyType;
		NEvent_Key(int v = 0, NEvent_Key_Type t = KEY_NONE) : KeyValue(v), KeyType(t) {}
		bool operator==(NEvent_Key& rh)
		{
			return KeyValue == rh.KeyValue && KeyType == rh.KeyType;
		}
	};
}