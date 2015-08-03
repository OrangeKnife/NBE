#pragma once
namespace NBE
{
    enum MouseButton
    {
        LEFT_BUTTON = 0,
        RIGHT_BUTTON,
        MIDDLE_BUTTON,
        MAX_BUTTONS
    };

    class NBE_API Mouse:public Input
    {
    public:
        Mouse();
        virtual ~Mouse();

        void init( HWND hwnd );
        virtual void update();
        virtual void processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        unsigned int getNumOfButtons() const;
        vec2f getPosition();
        bool didGoUp( MouseButton button) const;
        bool didGoDown( MouseButton button ) const;
        bool isDown( MouseButton button ) const;

        void calculateCenter( HWND hwnd );


        void setCent(LONG _X, LONG _Y);

    private:
        HWND m_hWnd;
        unsigned int m_buttonID;
        bool m_buttonStates[ MAX_BUTTONS ];		// true is down
        bool m_lastButtonStates[ MAX_BUTTONS ];
        LONG m_lastX,m_lastY;
		RECT cRect,wRect;
		int border_thickness;
		short wheelDelta;
		vec2f currentPos;
    };
}
