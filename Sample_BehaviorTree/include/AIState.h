#pragma once
#include "AstarNode.h"
#include <string>
#include <vector>
namespace NBE
{

	class AIState: public AstarNode
	{
	public:
		AIState(int numOfVar);
		~AIState();
		int GetVariable(int varIdx);
		void SetVariable(int varIdx, int val);
	private:
		std::vector<int> m_variableVec;
	};


	
}