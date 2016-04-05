#pragma once
#include "BTNode.h"
#include "Blackboard.h"
#include "AIBot.h"
namespace NBE
{
	namespace BT
	{
		class Leaf : public BTNode
		{
		public:
			BTNodeStatus Process(float DeltaTime)
			{
				return Success;
			}
		};

		class Leaf_MoveTo : public BTNode
		{
		private:
			AIBot* bot;
			int endLocation;
		public:
			Leaf_MoveTo(AIBot* _bot, int _endLocation): BTNode(),
				bot(_bot), endLocation(_endLocation)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot->getCurrentNode() != endLocation)
				{
					if(bot->getTargetLocation() != endLocation)
						bot->setTargetLocation(endLocation);
					return Running;
				}
				
				return Success;
			}
		};
		
		class Leaf_MoveToTarget : public BTNode
		{
		private:
			AIBot* bot;
		public:
			Leaf_MoveToTarget(AIBot* _bot) : BTNode(),
				bot(_bot)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot->getCurrentNode() != bot->getTargetLocation())
				{
					if (bot->getTargetLocation() >= 0)
						return Running;
					else
						return Failure;
				}

				return Success;
			}
		};

		class Leaf_GetItem : public BTNode
		{
		private:
			AIStateVariable variable;
			int amount;
			AIBot* bot;
		public:
			Leaf_GetItem(AIBot* _bot, AIStateVariable _variable, int _amount) :BTNode(),
				bot(_bot), variable(_variable), amount(_amount)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot && bot->getAIStateVariable(variable) >= amount)
					return Success;

				return Failure;
			}
		};

		class Leaf_ConsumeItem : public BTNode
		{
		private:
			AIStateVariable variable;
			int amount;
			AIBot* bot;
		public:
			Leaf_ConsumeItem(AIBot* _bot, AIStateVariable _variable, int _amount) :BTNode(),
				bot(_bot), variable(_variable), amount(_amount)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot)
				{
					bot->setAIStateVariable(variable, bot->getAIStateVariable(variable) - amount);
					return Success;
				}

				return Failure;
			}
		};

		class Leaf_GetResource : public BTNode
		{
		private:
			int resourceLocation;
			AstarNodeType res;
			AIBot* bot;
		public:
			Leaf_GetResource(AIBot* _bot, AstarNodeType _resource) :BTNode(),
				bot(_bot), res(_resource), resourceLocation(-1)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot)
				{
					resourceLocation = bot->getResource(res);
					if (resourceLocation >= 0)
					{
						bot->getBlackboard().SetData<int>("resourceLocation", resourceLocation);
						return Success;
					}
				}

				return Failure;
			}
		};

		class Leaf_SetResourceTarget : public BTNode
		{
		private:
			AIBot* bot;
		public:
			Leaf_SetResourceTarget(AIBot* _bot) :BTNode(),
				bot(_bot)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot)
				{
					int* pResourceLocation = bot->getBlackboard().GetData<int>("resourceLocation");
					if (pResourceLocation)
					{
						bot->setTargetLocation(*pResourceLocation);
						return Success;
					}
				}
				
				return Failure;
			}
		};

		class Leaf_ConsumeResource : public BTNode
		{
		private:
			AstarNodeType res;
			AIBot* bot;
		public:
			Leaf_ConsumeResource(AIBot* _bot, AstarNodeType _resource) :BTNode(),
				bot(_bot),res(_resource)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot)
				{
					auto resourceLocation = bot->getCurrentNode();
					bot->removeResource(res, resourceLocation);
					return Success;
				}

				return Failure;
			}
		};
		
		class Leaf_CreateItem : public BTNode
		{
		private:
			AIStateVariable res;
			int amount;
			AIBot* bot;
		public:
			Leaf_CreateItem(AIBot* _bot, AIStateVariable _resource, int _amount) :BTNode(),
				bot(_bot), res(_resource), amount(_amount)
			{}
			BTNodeStatus Process(float DeltaTime)
			{
				if (bot)
				{
					bot->setAIStateVariable(res, bot->getAIStateVariable(res) + amount);
					return Success;
				}

				return Failure;
			}
		};
	}
}