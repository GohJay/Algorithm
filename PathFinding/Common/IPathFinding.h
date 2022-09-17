#pragma once
#include <list>

class IPathFinding
{
public:
	struct Point
	{
		Point()
		{
		}
		Point(int x, int y) : xPos(x), yPos(y)
		{
		}
		int xPos;
		int yPos;
	};
public:
	virtual ~IPathFinding() {}
public:
	virtual bool FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route) = 0;
};
