#pragma once
#include "Decorator.h"
#include "MyAssert.h"
namespace NBE
{
	namespace BT
	{
		class RepeatUntilFail : public Decorator
		{
		private:
			int maxCount;
		public:
			RepeatUntilFail(int _maxCount):Decorator(),
				maxCount(_maxCount)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				myassert(children.size() != 0);
				for (auto i = 0; i < maxCount || maxCount < 0; ++i)
				{
					auto ret = children[0]->Process(DeltaTime);
					if (ret == Failure)
					{
						return Failure;
					}
				}
				return Success;
			}
		};
	}
}
