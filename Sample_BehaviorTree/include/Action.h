#pragma once
#include <string>
#include <vector>
namespace NBE
{
	class AIState;
	class Action
	{
	public:

		struct Operation 
		{
			Operation():idx(-1),ope(0)
			{}
			Operation(int i,int o):idx(i),ope(o)
			{}
			int idx; // indexing the variable in aistate 
			int ope; // like -1 +1 + 3
		};
		Action(std::string& name, int c):actionName(name),cost(c)
		{}
		~Action()
		{}
		virtual bool preCondition(AIState* st) = 0; //it is a function pointer 
		
		std::string actionName;
		std::vector<Operation> operations;
		int cost;
	};
}