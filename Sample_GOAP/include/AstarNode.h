#pragma once
namespace NBE
{
	//this node is different from the rendering astar node , this only works for the plan finding
	class AstarNode
	{
	public:
		AstarNode():G(0),H(0),parent(0)
		{}
		~AstarNode()
		{} 

		AstarNode* parent;

		int G;//to the start node
		int H;//to the end node
	};

}