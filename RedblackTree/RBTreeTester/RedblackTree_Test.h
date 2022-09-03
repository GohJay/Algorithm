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
	void PrintPreorder(Node* tree);
	void PrintInorder(Node* tree);
	void PrintPostorder(Node* tree);
	bool VerifyRootNode();
	bool VerifyLeafNode();
	int VerifyDoubleRedNode(Node* tree);
	int VerifyBlackNodeDepth(Node* tree);
	int GetLeftBlackNodeDepth(Node* tree);
	int GetRightBlackNodeDepth(Node* tree);
};
