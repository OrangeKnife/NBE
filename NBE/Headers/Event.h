#pragma once
namespace NBE
{
	struct NEvent
	{
		NEvent()
		{
			static int64_t EventCount;
			EventCount++;
		}
		virtual ~NEvent() {}
	};
}