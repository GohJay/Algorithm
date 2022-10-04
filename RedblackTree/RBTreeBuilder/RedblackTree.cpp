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
bool RedblackTree::Insert(int data)
{
	Node* newNode = InsertNode(&_root, data);
	if (newNode)
	{
		RebuildAfterInsert(newNode);
		_count++;
		return true;
	}
	return false;
}
bool RedblackTree::Remove(int data)
{
	Node* delNode = RemoveNode(&_root, data);
	if (delNode)
	{
		delete delNode;
		_count--;
		return true;
	}
	return false;
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
RedblackTree::Node* RedblackTree::InsertNode(Node** tree, int data)
{
	Node* node;
	if (*tree == _Nil)
	{
		node = new Node(data, _Nil, _Nil, _Nil, Node::COLOR::RED);
		*tree = node;
		return node;
	}

	if ((*tree)->_data > data)
	{
		if ((*tree)->_left == _Nil)
		{
			node = new Node(data, *tree, _Nil, _Nil, Node::COLOR::RED);
			(*tree)->_left = node;
			return node;
		}
		else
			return InsertNode(&(*tree)->_left, data);
	}
	else if ((*tree)->_data < data)
	{
		if ((*tree)->_right == _Nil)
		{
			node = new Node(data, *tree, _Nil, _Nil, Node::COLOR::RED);
			(*tree)->_right = node;
			return node;
		}
		else
			return InsertNode(&(*tree)->_right, data);
	}
	return nullptr;
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
			// 자식이 양쪽 다 있는 경우
			Node* min = SearchMinNode((*tree)->_right);
			(*tree)->_data = min->_data;
			return RemoveNode(&(*tree)->_right, min->_data);
		}
		else
		{
			// 자식이 하나 이하인 경우
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
	// 규칙을 위배하는 동안에는 루프를 반복한다.
	while (node != _root && node->_parent->_color == Node::COLOR::RED)
	{
		// 부모 노드가 할아버지 노드의 왼쪽 자식일 경우
		if (node->_parent == node->_parent->_parent->_left)
		{
			Node* uncle = node->_parent->_parent->_right;
			// 삼촌이 레드인 경우
			if (uncle->_color == Node::COLOR::RED)
			{
				node->_parent->_color = Node::COLOR::BLACK;
				uncle->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				node = node->_parent->_parent;
			}
			else
			{
				// 삼촌이 블랙이면서 node 가 부모의 오른쪽 자식일 때
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
			// 삼촌이 레드인 경우
			if (uncle->_color == Node::COLOR::RED)
			{
				node->_parent->_color = Node::COLOR::BLACK;
				uncle->_color = Node::COLOR::BLACK;
				node->_parent->_parent->_color = Node::COLOR::RED;
				node = node->_parent->_parent;
			}
			else
			{
				// 삼촌이 블랙이면서 node 가 부모의 왼쪽 자식일 때
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
	// 루트 노드는 반드시 블랙이어야 한다.
	_root->_color = Node::COLOR::BLACK;
}
void RedblackTree::RebuildAfterRemove(Node * node)
{
	Node* sibling;
	// 루트 노드가 되거나 레드 노드한테 블랙이 넘어가면 루프 종료
	while (node->_parent != _Nil && node->_color == Node::COLOR::BLACK)
	{
		if (node == node->_parent->_left)
		{
			sibling = node->_parent->_right;
			// 형제가 레드인 경우
			if (sibling->_color == Node::COLOR::RED)
			{
				sibling->_color = Node::COLOR::BLACK;
				node->_parent->_color = Node::COLOR::RED;
				RotateLeft(node->_parent);
			}
			else
			{
				// 양쪽 자식이 모두 블랙인 경우
				if (sibling->_left->_color == Node::COLOR::BLACK &&
					sibling->_right->_color == Node::COLOR::BLACK)
				{
					sibling->_color = Node::COLOR::RED;
					node = node->_parent;
				}
				else
				{
					// 왼쪽 자식이 레드인 경우
					if (sibling->_left->_color == Node::COLOR::RED)
					{
						sibling->_left->_color = Node::COLOR::BLACK;
						sibling->_color = Node::COLOR::RED;
						RotateRight(sibling);
						sibling = node->_parent->_right;
					}
					// 오른쪽 자식이 레드인 경우
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
			// 형제가 레드인 경우
			if (sibling->_color == Node::COLOR::RED)
			{
				sibling->_color = Node::COLOR::BLACK;
				node->_parent->_color = Node::COLOR::RED;
				RotateRight(node->_parent);
			}
			else
			{
				// 양쪽 자식이 모두 블랙인 경우
				if (sibling->_right->_color == Node::COLOR::BLACK &&
					sibling->_left->_color == Node::COLOR::BLACK)
				{
					sibling->_color = Node::COLOR::RED;
					node = node->_parent;
				}
				else
				{
					// 오른쪽 자식이 레드인 경우
					if (sibling->_right->_color == Node::COLOR::RED)
					{
						sibling->_right->_color = Node::COLOR::BLACK;
						sibling->_color = Node::COLOR::RED;

						RotateLeft(sibling);
						sibling = node->_parent->_left;
					}
					// 왼쪽 자식이 레드인 경우
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
