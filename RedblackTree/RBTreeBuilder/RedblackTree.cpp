#include "RedblackTree.h"

RedblackTree::RedblackTree() : _count(0)
{
	_Nil = new Node(0, nullptr, nullptr, nullptr, Node::COLOR::BLACK);
	_root = _Nil;
}
RedblackTree::~RedblackTree()
{
	DestroyTree(_root);
	delete _Nil;
}
void RedblackTree::Insert(int data)
{
	Node* newNode = new Node(data, _Nil, _Nil, _Nil, Node::COLOR::RED);
	InsertNode(&_root, newNode);
	RebuildAfterInsert(newNode);
	_count++;
}
void RedblackTree::Remove(int data)
{
	Node* delNode = RemoveNode(&_root, data);
	if (delNode)
	{
		delete delNode;
		_count--;
	}
}
void RedblackTree::RemoveAll()
{
	DestroyTree(_root);
	_root = _Nil;
}
void RedblackTree::Draw(HDC hdc, int x, int y)
{
	DrawNode(_root, hdc, x, y, x, y);
}
int RedblackTree::size()
{
	return _count;
}
void RedblackTree::InsertNode(Node** tree, Node* node)
{
	if (*tree == _Nil)
	{
		*tree = node;
		return;
	}

	if ((*tree)->_data >= node->_data)
	{
		if ((*tree)->_left == _Nil)
		{
			node->_parent = *tree;
			(*tree)->_left = node;
		}
		else
			InsertNode(&(*tree)->_left, node);
	}
	else
	{
		if ((*tree)->_right == _Nil)
		{
			node->_parent = *tree;
			(*tree)->_right = node;
		}
		else
			InsertNode(&(*tree)->_right, node);
	}
}
RedblackTree::Node* RedblackTree::RemoveNode(Node** tree, int data)
{
	if (*tree == _Nil)
		return nullptr;

	if ((*tree)->_data > data)
		return RemoveNode(&(*tree)->_left, data);
	else if ((*tree)->_data < data)
		return RemoveNode(&(*tree)->_right, data);
	else
	{
		if ((*tree)->_left != _Nil && (*tree)->_right != _Nil)
		{
			// �ڽ��� ���� �� �ִ� ���
			Node* min = SearchMinNode((*tree)->_right);
			(*tree)->_data = min->_data;
			return RemoveNode(&(*tree)->_right, min->_data);
		}
		else
		{
			// �ڽ��� �ϳ� ������ ���
			Node* delNode = *tree;
			Node* childNode;
			if (delNode->_left != _Nil)
				childNode = delNode->_left;
			else
				childNode = delNode->_right;

			childNode->_parent = delNode->_parent;

			if (delNode->_parent == _Nil)
				_root = childNode;
			else
			{
				if (delNode == delNode->_parent->_left)
					delNode->_parent->_left = childNode;
				else
					delNode->_parent->_right = childNode;
			}

			if (delNode->_color == Node::COLOR::BLACK)
				RebuildAfterRemove(childNode);

			return delNode;
		}
	}
}
void RedblackTree::DestroyTree(Node* tree)
{
	if (tree == _Nil)
		return;

	DestroyTree(tree->_left);
	DestroyTree(tree->_right);
	delete tree;
	_count--;
}
void RedblackTree::RebuildAfterInsert(Node * node)
{
	while (node != _root && node->_parent->_color == Node::COLOR::RED)
	{
		if (node->_parent == node->_parent->_parent->_left)
		{
			Node* uncle = node->_parent->_parent->_right;
			if (uncle->_color == Node::COLOR::RED)
			{
				node->_parent->_color = Node::COLOR::BLACK;
				uncle->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				node = node->_parent->_parent;
			}
			else
			{
				if (node == node->_parent->_right)
				{
					node = node->_parent;
					RotateLeft(node);
				}
				node->_parent->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				RotateRight(node->_parent->_parent);
			}
		}
		else
		{
			Node* uncle = node->_parent->_parent->_left;
			if (uncle->_color == Node::COLOR::RED)
			{
				node->_parent->_color = Node::COLOR::BLACK;
				uncle->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				node = node->_parent->_parent;
			}
			else
			{
				if (node == node->_parent->_left)
				{
					node = node->_parent;
					RotateRight(node);
				}
				node->_parent->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				RotateLeft(node->_parent->_parent);
			}
		}
	}
	_root->_color = Node::COLOR::BLACK;
}
void RedblackTree::RebuildAfterRemove(Node * node)
{
	Node* sibling;
	while (node->_parent != _Nil && node->_color == Node::COLOR::BLACK)
	{
		if (node == node->_parent->_left)
		{
			sibling = node->_parent->_right;
			if (sibling->_color == Node::COLOR::RED)
			{
				sibling->_color = Node::COLOR::BLACK;
				node->_parent->_color = Node::COLOR::RED;
				RotateLeft(node->_parent);
			}
			else
			{
				if (sibling->_left->_color == Node::COLOR::BLACK &&
					sibling->_right->_color == Node::COLOR::BLACK)
				{
					sibling->_color = Node::COLOR::RED;
					node = node->_parent;
				}
				else
				{
					if (sibling->_left->_color == Node::COLOR::RED)
					{
						sibling->_left->_color = Node::COLOR::BLACK;
						sibling->_color = Node::COLOR::RED;
						RotateRight(sibling);
						sibling = node->_parent->_right;
					}
					sibling->_color = node->_parent->_color;
					sibling->_parent->_color = Node::COLOR::BLACK;
					sibling->_right->_color = Node::COLOR::BLACK;
					RotateLeft(node->_parent);
					node = _root;
				}
			}
		}
		else
		{
			sibling = node->_parent->_left;
			if (sibling->_color == Node::COLOR::RED)
			{
				sibling->_color = Node::COLOR::BLACK;
				node->_parent->_color = Node::COLOR::RED;
				RotateRight(node->_parent);
			}
			else
			{
				if (sibling->_right->_color == Node::COLOR::BLACK &&
					sibling->_left->_color == Node::COLOR::BLACK)
				{
					sibling->_color = Node::COLOR::RED;
					node = node->_parent;
				}
				else
				{
					if (sibling->_right->_color == Node::COLOR::RED)
					{
						sibling->_right->_color = Node::COLOR::BLACK;
						sibling->_color = Node::COLOR::RED;

						RotateLeft(sibling);
						sibling = node->_parent->_left;
					}

					sibling->_color = node->_parent->_color;
					node->_parent->_color = Node::COLOR::BLACK;
					sibling->_left->_color = Node::COLOR::BLACK;
					RotateRight(node->_parent);
					node = _root;
				}
			}
		}
	}
	node->_color = Node::COLOR::BLACK;
}
void RedblackTree::RotateRight(Node * parent)
{
	Node* leftChild = parent->_left;
	parent->_left = leftChild->_right;

	if (leftChild->_right != _Nil)
		leftChild->_right->_parent = parent;

	leftChild->_parent = parent->_parent;

	if (parent->_parent == _Nil)
		_root = leftChild;
	else
	{
		if (parent->_parent->_left == parent)
			parent->_parent->_left = leftChild;
		else
			parent->_parent->_right = leftChild;
	}

	leftChild->_right = parent;
	parent->_parent = leftChild;
}
void RedblackTree::RotateLeft(Node * parent)
{
	Node* rightChild = parent->_right;

	parent->_right = rightChild->_left;

	if (rightChild->_left != _Nil)
		rightChild->_left->_parent = parent;

	rightChild->_parent = parent->_parent;

	if (parent->_parent == _Nil)
		_root = rightChild;
	else
	{
		if (parent->_parent->_left == parent)
			parent->_parent->_left = rightChild;
		else
			parent->_parent->_right = rightChild;
	}

	rightChild->_left = parent;
	parent->_parent = rightChild;
}
RedblackTree::Node * RedblackTree::SearchMinNode(Node * tree)
{
	if (tree == _Nil)
		return _Nil;

	if (tree->_left == _Nil)
		return tree;
	else
		return SearchMinNode(tree->_left);
}
RedblackTree::Node * RedblackTree::SearchMaxNode(Node * tree)
{
	if (tree == _Nil)
		return _Nil;

	if (tree->_right == _Nil)
		return tree;
	else
		return SearchMaxNode(tree->_right);
}
RedblackTree::Node * RedblackTree::SearchNode(Node * tree, int data)
{
	if (tree == _Nil)
		return _Nil;

	if (tree->_data > data)
		return SearchNode(tree->_left, data);
	else if (tree->_data < data)
		return SearchNode(tree->_right, data);
	else
		return tree;
}
void RedblackTree::DrawNode(Node* tree, HDC hdc, int x, int y, int width, int height)
{
	if (tree == _Nil)
		return;
	
	// Draw Line
	if (tree->_left != _Nil)
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x - (width / 2), y + height);
	}
	if (tree->_right != _Nil)
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x + (width / 2), y + height);
	}

	RECT rect = { x - 25, y - 25, x + 25, y + 25 };

	// Draw Ellipse
	COLORREF rgb = (tree->_color == Node::COLOR::RED) ? RGB(255, 0, 0) : RGB(0, 0, 0);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(rgb));
	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(SelectObject(hdc, hOldBrush));

	// Draw Text
	char data[32];
	_itoa_s(tree->_data, data, sizeof(data), 10);
	DrawTextA(hdc, data, strlen(data), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	
	// Recursive Draw
	DrawNode(tree->_left, hdc, x - (width / 2), y + height, width / 2, height);
	DrawNode(tree->_right, hdc, x + (width / 2), y + height, width / 2, height);
}