#pragma once
#include <Windows.h>

class RedblackTree
{
protected:
	enum COLOR
	{
		RED,
		BLACK
	};
	struct NODE
	{
		NODE(int data, NODE* parent, NODE* left, NODE* right, COLOR color)
			: _data(data), _parent(parent), _left(left), _right(right), _color(color)
		{
		}
		int _data;
		NODE* _parent;
		NODE* _left;
		NODE* _right;
		COLOR _color;
	};
public:
	RedblackTree();
	~RedblackTree();
public:
	bool Insert(int data);
	bool Remove(int data);
	void RemoveAll();
	void Draw(HDC hdc, int x, int y);
	int size();
private:
	NODE* InsertNode(NODE** tree, int node);
	NODE* RemoveNode(NODE** tree, int data);
	void DestroyTree(NODE* tree);
	void RebuildAfterInsert(NODE* node);
	void RebuildAfterRemove(NODE* node);
	void RotateRight(NODE* parent);
	void RotateLeft(NODE* parent);
	NODE* SearchMinNode(NODE* tree);
	NODE* SearchMaxNode(NODE* tree);
	NODE* SearchNode(NODE* tree, int data);
	void DrawNode(NODE* tree, HDC hdc, int x, int y, int width, int height);
protected:
	NODE* _root;
	NODE* _nil;
	int _count;
};
