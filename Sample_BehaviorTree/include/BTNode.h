#pragma once
#include <vector>
#include <algorithm>
#include "Blackboard.h"
using namespace std;
namespace NBE
{
	namespace BT
	{
		enum BTNodeStatus
		{
			Success,
			Failure,
			Running
		};
		class BTNode
		{
		protected:
			vector<BTNode*> children;
			BTNodeStatus currentStatus;
		public:
			BTNode()
			{}
			virtual ~BTNode()
			{
				for (auto it : children)
				{
					if (it)
						delete it;
				}
			}
			virtual BTNodeStatus Process(float DeltaTime) = 0;
			void AddChild(BTNode* node)
			{
				children.push_back(node);
			}
			void RemoveChild(BTNode* node)
			{
				for (auto it = children.begin(); it < children.end();)
				{
					if (node == *it)
						children.erase(it);
					else
						it++;
				}
			}

		};
	}
}
