#include "StraightLine.h"

StraightLine::StraightLine()
{
}
StraightLine::~StraightLine()
{
}
void StraightLine::Line(int srcX, int srcY, int dstX, int dstY)
{
	route.clear();
	route.emplace_back(srcX, srcY);

	// 시작점과, 끝점의 차를 구하여 직선의 X, Y 축의 변화량을 구한다.
	int dx = dstX - srcX;
	int dy = dstY - srcY;

	// 직선의 방향에 따라 선이 나아갈 좌표를 정한다.
	int nx = (dx > 0) ? 1 : ((dx == 0) ? 0 : -1);
	int ny = (dy > 0) ? 1 : ((dy == 0) ? 0 : -1);

	// 절대값을 구한다.
	dx = abs(dx);
	dy = abs(dy);

	// X 축이 더 긴 경우와, Y축이 더 긴 경우 두가지 경우로 나뉜다.
	int err = 0;
	if (dx > dy)
	{
		// X 축이 더 길으면, X 축을 기준으로
		for (int i = dx; i; i--)
		{
			err += dy;
			if (err >= (dx + dy) / 2)
			{
				err -= dx;
				srcY += ny;
			}
			srcX += nx;
			route.emplace_back(srcX, srcY);
		}
	}
	else
	{
		// Y 축이 더 길으면, Y 축을 기준으로
		for (int i = dy; i; i--)
		{
			err += dx;
			if (err >= (dx + dy) / 2)
			{
				err -= dy;
				srcX += nx;
			}
			srcY += ny;
			route.emplace_back(srcX, srcY);
		}
	}

	route.emplace_back(dstX, dstY);
}
void StraightLine::NextPoint()
{
	auto iter = route.begin();
	if (iter != route.end())
		route.erase(iter);
}
bool StraightLine::GetPoint(Point * point)
{
	auto iter = route.begin();
	if (iter != route.end())
	{
		point->xPos = iter->xPos;
		point->yPos = iter->yPos;
		return true;
	}
	return false;
}
