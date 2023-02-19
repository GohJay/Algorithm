#include "RedblackTree.h"

RedblackTree::RedblackTree() : _count(0)
{
	_nil = new NODE(0, nullptr, nullptr, nullptr, BLACK);
	_root = _nil;
}
RedblackTree::~RedblackTree()
{
	DestroyTree(_root);
	delete _nil;
}
bool RedblackTree::Insert(int data)
{
	NODE* newNode = InsertNode(&_root, data);
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
	NODE* delNode = RemoveNode(&_root, data);
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
	_root = _nil;
}
void RedblackTree::Draw(HDC hdc, int x, int y)
{
	DrawNode(_root, hdc, x, y, x, y);
}
int RedblackTree::size()
{
	return _count;
}
RedblackTree::NODE* RedblackTree::InsertNode(NODE** tree, int data)
{
	NODE* node;
	if (*tree == _nil)
	{
		node = new NODE(data, _nil, _nil, _nil, RED);
		*tree = node;
		return node;
	}

	if ((*tree)->_data > data)
	{
		if ((*tree)->_left == _nil)
		{
			node = new NODE(data, *tree, _nil, _nil, RED);
			(*tree)->_left = node;
			return node;
		}
		else
			return InsertNode(&(*tree)->_left, data);
	}
	else if ((*tree)->_data < data)
	{
		if ((*tree)->_right == _nil)
		{
			node = new NODE(data, *tree, _nil, _nil, RED);
			(*tree)->_right = node;
			return node;
		}
		else
			return InsertNode(&(*tree)->_right, data);
	}
	return nullptr;
}
RedblackTree::NODE* RedblackTree::RemoveNode(NODE** tree, int data)
{
	if (*tree == _nil)
		return nullptr;

	if ((*tree)->_data > data)
		return RemoveNode(&(*tree)->_left, data);
	else if ((*tree)->_data < data)
		return RemoveNode(&(*tree)->_right, data);
	else
	{
		if ((*tree)->_left != _nil && (*tree)->_right != _nil)
		{
			// 자식이 양쪽 다 있는 경우
			NODE* min = SearchMinNode((*tree)->_right);
			(*tree)->_data = min->_data;
			return RemoveNode(&min, min->_data);
		}
		else
		{
			// 자식이 하나 이하인 경우
			NODE* delNode = *tree;
			NODE* childNode;
			if (delNode->_left != _nil)
				childNode = delNode->_left;
			else
				childNode = delNode->_right;

			childNode->_parent = delNode->_parent;

			if (delNode->_parent == _nil)
				_root = childNode;
			else
			{
				if (delNode == delNode->_parent->_left)
					delNode->_parent->_left = childNode;
				else
					delNode->_parent->_right = childNode;
			}

			if (delNode->_color == BLACK)
				RebuildAfterRemove(childNode);

			return delNode;
		}
	}
}
void RedblackTree::DestroyTree(NODE* tree)
{
	if (tree == _nil)
		return;

	DestroyTree(tree->_left);
	DestroyTree(tree->_right);
	delete tree;
	_count--;
}
void RedblackTree::RebuildAfterInsert(NODE * node)
{
	// 규칙을 위배하는 동안에는 루프를 반복한다.
	while (node != _root && node->_parent->_color == RED)
	{
		// 부모 노드가 할아버지 노드의 왼쪽 자식일 경우
		if (node->_parent == node->_parent->_parent->_left)
		{
			NODE* uncle = node->_parent->_parent->_right;
			// 삼촌이 레드인 경우
			if (uncle->_color == RED)
			{
				node->_parent->_color = BLACK;
				uncle->_color = BLACK;
				node->_parent->_parent->_color = RED;
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
				node->_parent->_color = BLACK;
				node->_parent->_parent->_color = RED;
				RotateRight(node->_parent->_parent);
			}
		}
		else
		{
			NODE* uncle = node->_parent->_parent->_left;
			// 삼촌이 레드인 경우
			if (uncle->_color == RED)
			{
				node->_parent->_color = BLACK;
				uncle->_color = BLACK;
				node->_parent->_parent->_color = RED;
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
				node->_parent->_color = BLACK;
				node->_parent->_parent->_color = RED;
				RotateLeft(node->_parent->_parent);
			}
		}
	}
	// 루트 노드는 반드시 블랙이어야 한다.
	_root->_color = BLACK;
}
void RedblackTree::RebuildAfterRemove(NODE * node)
{
	NODE* sibling;
	// 루트 노드가 되거나 레드 노드한테 블랙이 넘어가면 루프 종료
	while (node->_parent != _nil && node->_color == BLACK)
	{
		if (node == node->_parent->_left)
		{
			sibling = node->_parent->_right;
			// 형제가 레드인 경우
			if (sibling->_color == RED)
			{
				sibling->_color = BLACK;
				node->_parent->_color = RED;
				RotateLeft(node->_parent);
			}
			else
			{
				// 양쪽 자식이 모두 블랙인 경우
				if (sibling->_left->_color == BLACK &&
					sibling->_right->_color == BLACK)
				{
					sibling->_color = RED;
					node = node->_parent;
				}
				else
				{
					// 왼쪽 자식이 레드인 경우
					if (sibling->_left->_color == RED)
					{
						sibling->_left->_color = BLACK;
						sibling->_color = RED;
						RotateRight(sibling);
						sibling = node->_parent->_right;
					}
					// 오른쪽 자식이 레드인 경우
					sibling->_color = node->_parent->_color;
					sibling->_parent->_color = BLACK;
					sibling->_right->_color = BLACK;
					RotateLeft(node->_parent);
					node = _root;
				}
			}
		}
		else
		{
			sibling = node->_parent->_left;
			// 형제가 레드인 경우
			if (sibling->_color == RED)
			{
				sibling->_color = BLACK;
				node->_parent->_color = RED;
				RotateRight(node->_parent);
			}
			else
			{
				// 양쪽 자식이 모두 블랙인 경우
				if (sibling->_right->_color == BLACK &&
					sibling->_left->_color == BLACK)
				{
					sibling->_color = RED;
					node = node->_parent;
				}
				else
				{
					// 오른쪽 자식이 레드인 경우
					if (sibling->_right->_color == RED)
					{
						sibling->_right->_color = BLACK;
						sibling->_color = RED;

						RotateLeft(sibling);
						sibling = node->_parent->_left;
					}
					// 왼쪽 자식이 레드인 경우
					sibling->_color = node->_parent->_color;
					node->_parent->_color = BLACK;
					sibling->_left->_color = BLACK;
					RotateRight(node->_parent);
					node = _root;
				}
			}
		}
	}
	node->_color = BLACK;
}
void RedblackTree::RotateRight(NODE * parent)
{
	NODE* leftChild = parent->_left;
	parent->_left = leftChild->_right;

	if (leftChild->_right != _nil)
		leftChild->_right->_parent = parent;

	leftChild->_parent = parent->_parent;

	if (parent->_parent == _nil)
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
void RedblackTree::RotateLeft(NODE * parent)
{
	NODE* rightChild = parent->_right;

	parent->_right = rightChild->_left;

	if (rightChild->_left != _nil)
		rightChild->_left->_parent = parent;

	rightChild->_parent = parent->_parent;

	if (parent->_parent == _nil)
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
RedblackTree::NODE * RedblackTree::SearchMinNode(NODE * tree)
{
	if (tree == _nil)
		return _nil;

	if (tree->_left == _nil)
		return tree;
	else
		return SearchMinNode(tree->_left);
}
RedblackTree::NODE * RedblackTree::SearchMaxNode(NODE * tree)
{
	if (tree == _nil)
		return _nil;

	if (tree->_right == _nil)
		return tree;
	else
		return SearchMaxNode(tree->_right);
}
RedblackTree::NODE * RedblackTree::SearchNode(NODE * tree, int data)
{
	if (tree == _nil)
		return _nil;

	if (tree->_data > data)
		return SearchNode(tree->_left, data);
	else if (tree->_data < data)
		return SearchNode(tree->_right, data);
	else
		return tree;
}
void RedblackTree::DrawNode(NODE* tree, HDC hdc, int x, int y, int width, int height)
{
	if (tree == _nil)
		return;
	
	// Draw Line
	if (tree->_left != _nil)
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x - (width / 2), y + height);
	}
	if (tree->_right != _nil)
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x + (width / 2), y + height);
	}

	RECT rect = { x - 25, y - 25, x + 25, y + 25 };

	// Draw Ellipse
	COLORREF rgb = (tree->_color == RED) ? RGB(255, 0, 0) : RGB(0, 0, 0);
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
