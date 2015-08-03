#pragma once
#include "NBE.h"
namespace NBE
{
    class Event;
    class NBE_API Listener // interface
    {
    public:
        Listener(){}
        virtual ~Listener(){}
        virtual void handleEvent(Event* e) = 0;
    };
}