#include "AIState.h"
#include "MyAssert.h"
namespace NBE
{
	AIState::AIState(int numOfVar)
	{
		m_variableVec.resize(numOfVar);
		memset(&m_variableVec.front(),0,numOfVar * sizeof(int));
	}
	AIState::~AIState()
	{}

	int AIState::GetVariable(int varIdx)
	{
		myassert(varIdx < (int)m_variableVec.size());
		return m_variableVec[varIdx];
	}
	void AIState::SetVariable(int varIdx, int val)
	{
		myassert(varIdx < (int)m_variableVec.size());
		m_variableVec[varIdx] = val;
	}
 
}