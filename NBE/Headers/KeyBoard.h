#pragma once
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <windows.h>
#endif
namespace NBE
{
    class NBE_API KeyBoard: public Input
    {
    public:
        KeyBoard();
        virtual ~KeyBoard();
        virtual void update();
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        virtual void processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
        inline bool isKeyDown(int virtualKey)
        {
            return m_keyMap[virtualKey];
            //return (GetAsyncKeyState(virtualKey) & 0x8000) == 0x8000;
        }
        inline bool isKeyUp(int virtualKey)
        {
            return !m_keyMap[virtualKey];
            //return (GetAsyncKeyState(virtualKey) & 0x8000) != 0x8000;
        }
        inline void keyDown(int virtualKey)
        {
            m_keyMap[virtualKey] = true;
        }
    private:
		bool m_keyMap[256] = {0};
        //std::unordered_map<int, bool> ; 
    };
}