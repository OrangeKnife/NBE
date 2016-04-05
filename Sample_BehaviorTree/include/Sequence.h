#pragma once
#include "Composite.h"
namespace NBE
{
	namespace BT
	{
		class Sequence : public Compostite
		{
			//means run through the nodes one by one
		public:
			BTNodeStatus Process(float DeltaTime)
			{
				for (auto child : children)
				{
					auto childStatus = child->Process(DeltaTime);
					if (childStatus != Success)
						return childStatus;
				}
				return Success;
			}
		};

		class SequenceUntilSuccess : public Compostite
		{
		public:
			BTNodeStatus Process(float DeltaTime)
			{
				for (auto child : children)
				{
					auto childStatus = child->Process(DeltaTime);
					if (childStatus != Failure)
						return childStatus;
				}
				return Success;
			}
		};

	}
}