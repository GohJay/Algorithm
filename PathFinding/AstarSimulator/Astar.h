#pragma once
#include "../Common/IPathFinding.h"
#include "../Common/ObjectPool.h"
#include <functional>
#include <set>

class Astar : public IPathFinding
{
private:
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
	Astar(std::function<bool(int, int)> callback);
	~Astar();
public:
	bool FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route);
private:
	void DestroyList();
	void MakeNode(NODE* parent, int x, int y, bool diagonal);
	void MakeEightDirectionNode(NODE* parent);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
private:
	std::function<bool(int, int)> _IsMovableCB;
	Jay::ObjectPool<NODE> _objectPool;
	std::multiset<NODE*, NODE> _openList;
	std::list<NODE*> _closeList;
	Point _destination;
};
