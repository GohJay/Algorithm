#pragma once
#include <Windows.h>

class RedblackTree
{
protected:
	struct Node
	{
		enum COLOR
		{
			RED,
			BLACK
		};
		Node(int data, Node* parent, Node* left, Node* right, COLOR color)
			: _data(data), _parent(parent), _left(left), _right(right), _color(color)
		{
		}
		int _data;
		Node* _parent;
		Node* _left;
		Node* _right;
		COLOR _color;
	};
public:
	RedblackTree();
	~RedblackTree();
public:
	void Insert(int data);
	void Remove(int data);
	void RemoveAll();
	void Draw(HDC hdc, int x, int y);
	int size();
private:
	void InsertNode(Node** tree, Node* node);
	Node* RemoveNode(Node** tree, int data);
	void DestroyTree(Node* tree);
	void RebuildAfterInsert(Node* node);
	void RebuildAfterRemove(Node* node);
	void RotateRight(Node* parent);
	void RotateLeft(Node* parent);
	Node* SearchMinNode(Node* tree);
	Node* SearchMaxNode(Node* tree);
	Node* SearchNode(Node* tree, int data);
	void DrawNode(Node* tree, HDC hdc, int x, int y, int width, int height);
protected:
	Node* _root;
	Node* _Nil;
	int _count;
};
