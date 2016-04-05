#pragma once
#include "Decorator.h"
#include "MyAssert.h"
namespace NBE
{
	namespace BT
	{
		class Inverter: public Decorator
		{
		public:
			BTNodeStatus Process(float DeltaTime)
			{
				myassert(children.size() != 0)
				auto ret = children[0]->Process(DeltaTime);
				if (ret == Success)
					return Failure;
				else if(ret == Failure)
					return Success;

				return ret;
			}
		};
	}
}