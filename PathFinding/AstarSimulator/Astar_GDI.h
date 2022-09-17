#pragma once
#include "Astar.h"
#include <Windows.h>
#include <set>

#define MAX_WIDTH	100
#define MAX_HEIGHT	50

class Astar_GDI
{
private:
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
		Node* parent;
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
	void MakeNode(Node* parent, int x, int y, bool diagonal);
	void MakeEightDirectionNode(Node* parent);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
	bool IsMovable(int x, int y);
private:
	TRAVLE_STATE _state;
	Jay::ObjectPool<Node> _objectPool;
	std::multiset<Node*, Node> _openList;
	std::list<Node*> _closeList;
	Node* _traveler;
	Node* _tracker;
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
