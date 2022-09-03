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
	cout << "[����1] ��Ʈ ���� �������̾�� �Ѵ�." << endl;
	cout << "���: " << (VerifyRootNode() ? "Success" : "Fail") << endl << endl;

	cout << "[����2] ���� ����(Nil) �������̾�� �Ѵ�." << endl;
	cout << "���: " << (VerifyLeafNode() ? "Success" : "Fail") << endl << endl;

	cout << "[����3] ���� ����� �ڽ��� �������̾�� �Ѵ�." << endl;
	cout << "���� ����: " << VerifyDoubleRedNode(_root) << endl << endl;

	cout << "[����4] ��Ʈ �ε�κ��� ��� ���� �������� �� ��� ���̰� ���ƾ� �Ѵ�." << endl;
	cout << "���� ����: " << VerifyBlackNodeDepth(_root) << endl << endl;
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
