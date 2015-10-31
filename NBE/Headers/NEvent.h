#pragma once
namespace NBE
{
	struct NEvent
	{
		NEvent(){}
		virtual ~NEvent() {}
	};

	struct NEvent_Key : NEvent
	{
		enum NEvent_Key_Type {
			KEY_NONE = 0,
			KEY_DOWN,
			KEY_UP,
			KEY_PRESSED //down and up
		};
		int KeyValue;
		NEvent_Key_Type KeyType;
		NEvent_Key(int v = 0, NEvent_Key_Type t = KEY_NONE) : KeyValue(v), KeyType(t) {}
		bool operator==(NEvent_Key& rh)
		{
			return KeyValue == rh.KeyValue && KeyType == rh.KeyType;
		}
	};
}