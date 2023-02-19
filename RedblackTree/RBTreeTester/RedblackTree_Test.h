#pragma once
#include "../RBTreeBuilder/RedblackTree.h"

class RedblackTree_Test : public RedblackTree
{
public:
	enum ORDER_TYPE
	{
		PREORDER = 0,
		INORDER,
		POSTORDER
	};
public:
	RedblackTree_Test();
	~RedblackTree_Test();
public:
	void Print(ORDER_TYPE type = INORDER);
	void Verify();
private:
	void PrintPreorder(NODE* tree);
	void PrintInorder(NODE* tree);
	void PrintPostorder(NODE* tree);
	bool VerifyRootNode();
	bool VerifyLeafNode();
	int VerifyDoubleRedNode(NODE* tree);
	int VerifyBlackNodeDepth(NODE* tree);
	int GetLeftBlackNodeDepth(NODE* tree);
	int GetRightBlackNodeDepth(NODE* tree);
};
