#pragma once

#include <unordered_map>
namespace NBE
{
    class NBE_API KeyBoard: public Input
    {
    public:
        KeyBoard();
        virtual ~KeyBoard();
        virtual void update();
        virtual void processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
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
        std::unordered_map<int, bool> m_keyMap; 
    };
}