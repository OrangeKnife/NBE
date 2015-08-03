#pragma once
#include "NBE.h"
namespace NBE
{
    enum EventType
    {
        NOEVENT = 0,
        MOUSE_MOVE,
		MOUSE_LEFTDRAG,
		MOUSE_RIGHTDRAG,
		MOUSE_MIDDLEDRAG,
        MOUSE_BUTTON_DOWN,
        MOUSE_BUTTON_PRESS,
        MOUSE_BUTTON_RELEASE,
        MOUSE_WHEEL,
        KEY_DOWN,
        KEY_PRESS,
		KEY_CHAR,
        KEY_RELEASE
    };

    class NBE_API Event
    {
    public:
        Event(EventType _t):type(_t)
        {}
        virtual ~Event()
        {}
        EventType type;
    };


}