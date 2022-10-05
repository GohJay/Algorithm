#pragma once
#include "JumpPointSearch.h"
#include "StraightLine.h"
#include <Windows.h>
#include <set>

#define MAX_WIDTH	100
#define MAX_HEIGHT	50

class JumpPointSearch_GDI
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
	enum TRAVEL_STATE
	{
		DEPARTURE = 0,
		TRAVLING,
		ARRIVAL,
		NOWAY,
	};
	enum GDI_PEN
	{
		GRID = 0,
		TRACKING,
		OPTIMIZE
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
	JumpPointSearch_GDI(char map[][MAX_WIDTH], int width, int height);
	~JumpPointSearch_GDI();
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
	void RenderNodeInfo(HDC hdc, INT screanX, INT screanY, INT scale, Node* node);
	void RenderTileColor(HDC hdc, INT screanX, INT screanY, INT scale);
	void InitColorTable();
	void InitColorMap();
	void ResetColorMap();
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
	void OptimizeStraightPath(std::list<JPS::Point> & route);
	bool IsMovable(int x, int y);
private:
	TRAVEL_STATE _state;
	Jay::ObjectPool<Node> _objectPool;
	std::multiset<Node*, Node> _openList;
	std::list<Node*> _closeList;
	Node* _traveler;
	Node* _tracker;
	std::list<JPS::Point> _route;
	StraightLine _bresenham;
	IPathFinding* _JPS;
	IPathFinding::Point _source;
	IPathFinding::Point _destination;
	COLORREF* _colorTable;
	int** _colorMap;
	int _selColor;
	int _width;
	int _height;
	char (*_map)[MAX_WIDTH];
private:
	HPEN _hPen[3];
	HBRUSH _hBrush[6];
};
inline
	bool JumpPointSearch_GDI::IsMovable(int x, int y)
{
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return false;
	if (_map[y][x])
		return false;
	return true;
}
typedef JumpPointSearch_GDI JPS_GDI;
