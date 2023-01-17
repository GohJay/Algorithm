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
	struct NODE
	{
		bool operator() (const NODE* a, const NODE* b) const
		{
			return a->F < b->F;
		}
		float G;
		float H;
		float F;
		int xPos;
		int yPos;
		unsigned char direction;
		NODE* parent;
	};
public:
	JumpPointSearch(std::function<bool(int, int)> callback);
	~JumpPointSearch();
public:
	bool FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route);
private:
	void DestroyList();
	void JumpProc(NODE* node);
	void MakeNode(NODE* parent, const JumpPoint& point);
	bool SearchLLCorner(int x, int y, JumpPoint* point);
	bool SearchLUCorner(int x, int y, JumpPoint* point);
	bool SearchUUCorner(int x, int y, JumpPoint* point);
	bool SearchRUCorner(int x, int y, JumpPoint* point);
	bool SearchRRCorner(int x, int y, JumpPoint* point);
	bool SearchRDCorner(int x, int y, JumpPoint* point);
	bool SearchDDCorner(int x, int y, JumpPoint* point);
	bool SearchLDCorner(int x, int y, JumpPoint* point);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
	bool IsMovable(int x, int y);
	void OptimizeStraightPath(std::list<Point>& route);
private:
	std::function<bool(int, int)> _callback;
	Jay::ObjectPool<NODE> _objectPool;
	std::multiset<NODE*, NODE> _openList;
	std::list<NODE*> _closeList;
	Point _destination;
	StraightLine _bresenham;
};
inline bool JumpPointSearch::IsMovable(int x, int y)
{
	return _callback(x, y);
}
typedef JumpPointSearch JPS;
