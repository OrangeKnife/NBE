#include "NBE.h"

#include "Vector2.h"
#include "Vector3.h"
#include "CustomEvent.h"
namespace NBE
{
	KeyEvent::KeyEvent(EventType _t, int _key):Event(_t),virtualKey(_key){}
	KeyEvent::~KeyEvent(){}

	MouseEvent::MouseEvent(EventType _t,vec3f& delta, long currentX=0, long currentZ=0):Event(_t), deltaMove(delta),
		pos(float(currentX),float(currentZ)){}
	MouseEvent::MouseEvent(EventType _t,unsigned int _bt,long currentX=0, long currentZ=0):Event(_t),bt(_bt),pos(float(currentX),float(currentZ)){}
	MouseEvent::~MouseEvent(){}
}