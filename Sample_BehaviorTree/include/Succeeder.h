#pragma once
#include "Decorator.h"
#include "MyAssert.h"
namespace NBE
{
	namespace BT
	{
		class Succeeder : public Decorator
		{
		public:
			BTNodeStatus Process(float DeltaTime)
			{
				myassert(children.size() != 0)
				children[0]->Process(DeltaTime);
				return Success;
			}
		};
	}
}