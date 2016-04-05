#pragma once
namespace NBE
{
	struct AstarNode
	{	
		AstarNode():G(0),H(0),parent(0)
		{}
		~AstarNode()
		{} 

		AstarNode* parent;

		int G;//to the start node
		int H;//to the end node
	};

}