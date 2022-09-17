#pragma once
#include <list>

class StraightLine
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
	StraightLine();
	~StraightLine();
public:
	void Line(int srcX, int srcY, int dstX, int dstY);
	void NextPoint();
	bool GetPoint(Point* point);
private:

private:
	std::list<Point> route;
};
