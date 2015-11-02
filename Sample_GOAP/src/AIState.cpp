#include "AIState.h"
namespace NBE
{
	AIState::AIState(int numOfVar)
	{
		m_variableVec.resize(numOfVar);
		memset(&m_variableVec.front(),0,numOfVar * sizeof(int));
	}

	AIState::~AIState()
	{}
 
	void AIState::act( Action* action )
	{
		for (unsigned int i = 0; i < action->operations.size();++i)
		{
			m_variableVec[ action->operations[i].idx ] += action->operations[i].ope;
		}
		formerAction = action;
	}

}