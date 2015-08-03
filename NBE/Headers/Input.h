#pragma once
#include <queue>
#include <vector>
#include <Windows.h>
namespace NBE
{
    enum InputType//devices
    {
        MOUSE = 0,
        KEYBOARD,
        JOYSTICK,
        NUM_DEVICE
    };
    class NBE_API Input
    {
    public:
        Input(InputType t);
        virtual ~Input()
        {
            m_listener.clear();
        }
        virtual void update()=0;
        virtual void processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)=0;
        unsigned int addEvent(Event* theEvent);
        InputType getType();
        void handleAllEvents();
        void registerListener(Listener* lsn);
        void unRegisterListener(Listener* lsn);

    private:
        std::queue<Event*> m_eventQue;
        InputType m_inputType;
        std::vector<Listener*> m_listener;
    };
}