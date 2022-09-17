#pragma once
#include "../Common/IPathFinding.h"
#include "../Common/ObjectPool.h"
#include "StraightLine.h"
#include <functional>
#include <set>

class JumpPointSearch : public IPathFinding
{
private:
	struct JumpPoint
	{
		int xPos;
		int yPos;
		unsigned char direction;
	};
	struct Node
	{
		bool operator() (const Node* a, const Node* b) const
		{
			return a->F < b->F;
		}
		float G;
		float H;
		float F;
		int xPos;
		int yPos;
		unsigned char direction;
		Node* parent;
	};
public:
	template<typename T>
	JumpPointSearch(bool(T::* mem_func)(int, int), T* obj);
	JumpPointSearch(bool(*func)(int, int));
	~JumpPointSearch();
public:
	bool FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route);
private:
	void DestroyList();
	void JumpProc(Node* node);
	void MakeNode(Node* parent, const JumpPoint& point);
	bool SearchLLCorner(int x, int y, JumpPoint* point);
	bool SearchLUCorner(int x, int y, JumpPoint* point);
	bool SearchUUCorner(int x, int y, JumpPoint* point);
	bool SearchRUCorner(int x, int y, JumpPoint* point);
	bool SearchRRCorner(int x, int y, JumpPoint* point);
	bool SearchRDCorner(int x, int y, JumpPoint* point);
	bool SearchDDCorner(int x, int y, JumpPoint* point);
	bool SearchLDCorner(int x, int y, JumpPoint* point);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
	void OptimizeStraightPath(std::list<Point>& route);
private:
	std::function<bool(int, int)> _IsMovableCB;
	Jay::ObjectPool<Node> _objectPool;
	std::multiset<Node*, Node> _openList;
	std::list<Node*> _closeList;
	Point _destination;
	StraightLine _bresenham;
};
template<typename T>
JumpPointSearch::JumpPointSearch(bool(T::* mem_func)(int, int), T* obj) : _objectPool(0, false)
{
	_IsMovableCB = std::bind(mem_func, obj, std::placeholders::_1, std::placeholders::_2);
}
typedef JumpPointSearch JPS;
