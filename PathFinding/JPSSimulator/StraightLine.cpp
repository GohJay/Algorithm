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

	// ��������, ������ ���� ���Ͽ� ������ X, Y ���� ��ȭ���� ���Ѵ�.
	int dx = dstX - srcX;
	int dy = dstY - srcY;

	// ������ ���⿡ ���� ���� ���ư� ��ǥ�� ���Ѵ�.
	int nx = (dx > 0) ? 1 : ((dx == 0) ? 0 : -1);
	int ny = (dy > 0) ? 1 : ((dy == 0) ? 0 : -1);

	// ���밪�� ���Ѵ�.
	dx = abs(dx);
	dy = abs(dy);

	// X ���� �� �� ����, Y���� �� �� ��� �ΰ��� ���� ������.
	int err = 0;
	if (dx > dy)
	{
		// X ���� �� ������, X ���� ��������
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
		// Y ���� �� ������, Y ���� ��������
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
