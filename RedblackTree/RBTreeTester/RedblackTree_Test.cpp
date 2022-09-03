#include "RedblackTree_Test.h"
#include <iostream>

using namespace std;
RedblackTree_Test::RedblackTree_Test()
{
}
RedblackTree_Test::~RedblackTree_Test()
{
}
void RedblackTree_Test::Print(ORDER_TYPE type)
{
	if (_root == _Nil)
		return;

	switch (type)
	{
	case PREORDER:
		PrintPreorder(_root);
		break;
	case INORDER:
		PrintInorder(_root);
		break;
	case POSTORDER:
		PrintPostorder(_root);
		break;
	default:
		break;
	}
}
void RedblackTree_Test::Verify()
{
	cout << "[검증1] 루트 노드는 검은색이어야 한다." << endl;
	cout << "결과: " << (VerifyRootNode() ? "Success" : "Fail") << endl << endl;

	cout << "[검증2] 리프 노드는(Nil) 검은색이어야 한다." << endl;
	cout << "결과: " << (VerifyLeafNode() ? "Success" : "Fail") << endl << endl;

	cout << "[검증3] 레드 노드의 자식은 검은색이어야 한다." << endl;
	cout << "오류 개수: " << VerifyDoubleRedNode(_root) << endl << endl;

	cout << "[검증4] 루트 로드로부터 모든 리프 노드까지의 블랙 노드 깊이가 같아야 한다." << endl;
	cout << "오류 개수: " << VerifyBlackNodeDepth(_root) << endl << endl;
}
void RedblackTree_Test::PrintPreorder(Node* tree)
{
	if (tree == _Nil)
		return;

	cout << tree->_data << " ";
	PrintPreorder(tree->_left);
	PrintPreorder(tree->_right);
}
void RedblackTree_Test::PrintInorder(Node* tree)
{
	if (tree == _Nil)
		return;

	PrintInorder(tree->_left);
	cout << tree->_data << " ";
	PrintInorder(tree->_right);
}
void RedblackTree_Test::PrintPostorder(Node* tree)
{
	if (tree == _Nil)
		return;

	PrintPostorder(tree->_left);
	PrintPostorder(tree->_right);
	cout << tree->_data << " ";
}
bool RedblackTree_Test::VerifyRootNode()
{
	return _root->_color == Node::COLOR::BLACK;
}
bool RedblackTree_Test::VerifyLeafNode()
{
	return _Nil->_color == Node::COLOR::BLACK;
}
int RedblackTree_Test::VerifyDoubleRedNode(Node * tree)
{
	if (tree == _Nil)
		return 0;

	int ret = 0;
	if (tree->_color == Node::COLOR::RED)
	{
		if (tree->_left->_color != Node::COLOR::BLACK ||
			tree->_right->_color != Node::COLOR::BLACK)
		{
			ret++;
		}
	}

	ret += VerifyDoubleRedNode(tree->_left);
	ret += VerifyDoubleRedNode(tree->_right);
	return ret;
}
int RedblackTree_Test::VerifyBlackNodeDepth(Node * tree)
{
	if (tree == _Nil)
		return 0;

	int ret = 0;
	int lDepth = GetLeftBlackNodeDepth(tree->_left);
	int rDepth = GetRightBlackNodeDepth(tree->_right);
	if (lDepth != rDepth)
		ret++;

	ret += VerifyBlackNodeDepth(tree->_left);
	ret += VerifyBlackNodeDepth(tree->_right);
	return ret;
}
int RedblackTree_Test::GetLeftBlackNodeDepth(Node * tree)
{
	int ret = 0;
	if (tree->_color == Node::COLOR::BLACK)
		ret++;

	if (tree != _Nil)
		ret += GetLeftBlackNodeDepth(tree->_left);
	return ret;
}
int RedblackTree_Test::GetRightBlackNodeDepth(Node * tree)
{
	int ret = 0;
	if (tree->_color == Node::COLOR::BLACK)
		ret++;

	if (tree != _Nil)
		ret += GetRightBlackNodeDepth(tree->_right);
	return ret;
}
