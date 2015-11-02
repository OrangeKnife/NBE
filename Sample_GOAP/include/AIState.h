#pragma once
#include "AstarNode.h"
#include <string>
#include <vector>
#include "Action.h"
namespace NBE
{

	class AIState: public AstarNode
	{
	public:
		AIState(int numOfVar);
		~AIState();
		
		void act(Action* action);
		bool operator!= (const AIState& other)const
		{
			if (other.formerAction != formerAction)
				return true;

			int cmp = memcmp(&m_variableVec[0], &other.m_variableVec[0], sizeof(int) * m_variableVec.size());

			return cmp != 0;
		}

		bool operator== (const AIState& other)const
		{
			if (other.formerAction != formerAction)
				return false;

			int cmp = memcmp(&m_variableVec[0], &other.m_variableVec[0], sizeof(int) * m_variableVec.size());

			return cmp == 0;
		}
		std::vector<int> m_variableVec;
		Action* formerAction;
	};


	
}