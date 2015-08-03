#pragma once
#include <vector>

namespace NBE
{
	class NBE_API  RendererWindow
    {
    public:
        RendererWindow(HINSTANCE h)
        {}

        virtual ~RendererWindow()
        {}

        HWND getHWND(){return m_hwnd;};
        bool isActive(){return m_hwnd == ::GetFocus();}
    protected:
        HWND m_hwnd;
    };



}