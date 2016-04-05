#pragma once
#include "Decorator.h"
#include "MyAssert.h"
namespace NBE
{
	namespace BT
	{
		class Repeater : Decorator
		{
		private:
			int maxCount;
		public:
			Repeater(int _maxCount) :Decorator(),
				maxCount(_maxCount)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				myassert(children.size() != 0);
				for (auto i = 0; i < maxCount || maxCount < 0; ++i)
				{
					children[0]->Process(DeltaTime);
				}
				
				return Success;
			}
		};
	}
}
