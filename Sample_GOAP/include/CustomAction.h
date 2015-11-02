#pragma once
#include "AIState.h"
namespace NBE
{
	class CustomAction : public Action
	{
	public:
		CustomAction(std::string& name, int c):Action(name,c),needNodeType(-1)
		{}
		bool preCondition(AIState* st)
		{
			for (unsigned int i = 0; i < operations.size(); ++i)
			{
				if ( st->m_variableVec[ operations[i].idx ] < - operations[i].ope )
					return false;
			}
			
			return true;
		}

		int needNodeType;
		// like money , wolf or anything else in the astar map, so the ai knows need to go there to do this action
		// this variable is specific to this sample
	};


 
 
}