#pragma once
#include "Astar.h"
#include <Windows.h>
#include <set>

#define MAX_WIDTH	100
#define MAX_HEIGHT	50

class Astar_GDI
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
		NODE* parent;
	};
	enum TRAVLE_STATE
	{
		DEPARTURE = 0,
		TRAVLING,
		ARRIVAL,
		NOWAY
	};
	enum GDI_PEN
	{
		GRID = 0,
		TRACKING
	};
	enum GDI_BRUSH
	{
		OBSTACLE = 0,
		SOURCE,
		DEST,
		OPEN,
		CLOSE,
		TRAVEL
	};
public:
	Astar_GDI(char map[][MAX_WIDTH], int width, int height);
	~Astar_GDI();
public:
	void SetSource(int srcX, int srcY);
	void SetDestination(int dstX, int dstY);
	bool FindPathOnce();
	void Cleanup();
	void Render(HDC hdc, INT screanX, INT screanY, INT scale);
private:
	void RenderGrid(HDC hdc, INT screanX, INT screanY, INT scale);
	void RenderObstacle(HDC hdc, INT screanX, INT screanY, INT scale);
	void RenderPathfinding(HDC hdc, INT screanX, INT screanY, INT scale);
private:
	void DestroyList();
	void MakeNode(NODE* parent, int x, int y, bool diagonal);
	void MakeEightDirectionNode(NODE* parent);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
	bool IsMovable(int x, int y);
private:
	TRAVLE_STATE _state;
	Jay::ObjectPool<NODE> _objectPool;
	std::multiset<NODE*, NODE> _openList;
	std::list<NODE*> _closeList;
	NODE* _traveler;
	NODE* _tracker;
	IPathFinding* _Astar;
	IPathFinding::Point _source;
	IPathFinding::Point _destination;
	int _width;
	int _height;
	char (*_map)[MAX_WIDTH];
private:
	HPEN _hPen[2];
	HBRUSH _hBrush[6];
};
inline
	bool Astar_GDI::IsMovable(int x, int y)
{
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return false;
	if (_map[y][x])
		return false;
	return true;
}
