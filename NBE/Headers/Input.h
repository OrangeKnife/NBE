#pragma once

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <windows.h>
#endif
#include "NBE.h"
#include "Constant.h"
#include "MyAssert.h"
#include <memory>
namespace NBE
{
    enum InputType//devices
    {
		NODEVICD = 0,
        MOUSE,
        KEYBOARD,
        JOYSTICK,
        NUM_DEVICE
    };
    class NBE_API Input
    {
    public:
		static Input* getInput(size_t id)
		{
			myassert(id < MAX_INPUTS);
			return Inputs[id].get();
		}
		Input::Input(InputType t) :m_inputType(t) {
			Inputs[totalInputNum++].reset(this);
		}

		static size_t getTotalInputNum()
		{
			return totalInputNum;
		}
        virtual ~Input(){}
        virtual void update()=0;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        virtual void processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)=0;
#endif

		
    private:
        InputType m_inputType;
		static std::shared_ptr<Input> Inputs[MAX_INPUTS];
		static size_t totalInputNum;

    };
}