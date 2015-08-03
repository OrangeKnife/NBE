#pragma once
#include "Event.h"
namespace NBE
{
    class NBE_API MouseEvent: public Event
    {
    public:
        MouseEvent(EventType _t,vec3f& delta, long currentX, long currentZ);
        MouseEvent(EventType _t,unsigned int _bt,long currentX, long currentZ);
        ~MouseEvent();
        vec3f deltaMove;
        unsigned int bt;
        vec2f pos;
    };

    class NBE_API KeyEvent: public Event
    {
    public:
        KeyEvent(EventType _t, int _key);
        ~KeyEvent();
        int virtualKey;
    };
}