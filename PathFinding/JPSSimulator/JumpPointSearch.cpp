#include "JumpPointSearch.h"

#define NODE_DIRECTION_LL		1
#define NODE_DIRECTION_LU		NODE_DIRECTION_LL << 1
#define NODE_DIRECTION_UU		NODE_DIRECTION_LL << 2
#define NODE_DIRECTION_RU		NODE_DIRECTION_LL << 3
#define NODE_DIRECTION_RR		NODE_DIRECTION_LL << 4
#define NODE_DIRECTION_RD		NODE_DIRECTION_LL << 5
#define NODE_DIRECTION_DD		NODE_DIRECTION_LL << 6
#define NODE_DIRECTION_LD		NODE_DIRECTION_LL << 7
#define NODE_DIRECTION_ALL		0xff

JumpPointSearch::JumpPointSearch(bool(*func)(int, int)) : _IsMovableCB(func), _objectPool(0, false)
{
}
JumpPointSearch::~JumpPointSearch()
{
	DestroyList();
}
bool JumpPointSearch::FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route)
{
	// 길찾기를 위한 초기화 작업 수행
	_destination.xPos = dstX;
	_destination.yPos = dstY;

	// 시작 노드 생성
	Node* node = _objectPool.Alloc();
	node->G = 0.0f;
	node->H = abs(dstX - srcX) + abs(dstY - srcY);
	node->F = node->G + node->H;
	node->xPos = srcX;
	node->yPos = srcY;
	node->direction = NODE_DIRECTION_ALL;
	node->parent = nullptr;
	_openList.insert(node);

	while (!_openList.empty())
	{
		// 오픈리스트에서 노드 꺼내기
		auto iter = _openList.begin();
		Node* node = *iter;

		// 오픈리스트에서 꺼낸 노드를 클로즈리스트로 이동
		_openList.erase(iter);
		_closeList.push_back(node);

		// 노드 좌표가 도착지인지 확인
		if (node->xPos == dstX && node->yPos == dstY)
		{
			// 찾기 완료한 노선을 리스트에 적재
			while (node)
			{
				route.emplace_front(node->xPos, node->yPos);
				node = node->parent;
			}
			
			// 직선 경로에 대한 최적화 로직 수행 (브레즌햄 알고리즘 사용)
			OptimizeStraightPath(route);

			DestroyList();
			return true;
		}

		// 노드 좌표가 도착지가 아닐 경우 점프포인트서치 수행
		JumpProc(node);
	}
	DestroyList();
	return false;
}
void JumpPointSearch::DestroyList()
{
	// 오픈리스트와 클로즈리스트를 정리한다.
	for (auto iter = _openList.begin(); iter != _openList.end();)
	{
		Node* node = *iter;
		_objectPool.Free(node);
		iter = _openList.erase(iter);
	}
	for (auto iter = _closeList.begin(); iter != _closeList.end();)
	{
		Node* node = *iter;
		_objectPool.Free(node);
		iter = _closeList.erase(iter);
	}
}
void JumpPointSearch::JumpProc(Node * node)
{
	// 점프포인트서치를 수행한다.
	JumpPoint point;
	if (node->direction & NODE_DIRECTION_LL)
	{
		if (SearchLLCorner(node->xPos - 1, node->yPos, &point))
			MakeNode(node, point);
	}	
	if (node->direction & NODE_DIRECTION_LU)
	{
		if (SearchLUCorner(node->xPos - 1, node->yPos - 1, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_UU)
	{
		if (SearchUUCorner(node->xPos, node->yPos - 1, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_RU)
	{
		if (SearchRUCorner(node->xPos + 1, node->yPos - 1, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_RR)
	{
		if (SearchRRCorner(node->xPos + 1, node->yPos, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_RD)
	{
		if (SearchRDCorner(node->xPos + 1, node->yPos + 1, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_DD)
	{
		if (SearchDDCorner(node->xPos, node->yPos + 1, &point))
			MakeNode(node, point);
	}
	if (node->direction & NODE_DIRECTION_LD)
	{
		if (SearchLDCorner(node->xPos - 1, node->yPos + 1, &point))
			MakeNode(node, point);
	}
}
void JumpPointSearch::MakeNode(Node * parent, const JumpPoint& point)
{
	float g = parent->G;
	if (IsDiagonal(parent->xPos, parent->yPos, point.xPos, point.yPos))
		g += 1.5f;
	else
		g += 1.0f;

	// 생성하려는 노드가 클로즈리스트에 이미 존재할 경우 방문했던 타일이므로 생성하지 않고 나간다.
	for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		Node* node = *iter;
		if (node->xPos == point.xPos && node->yPos == point.yPos)
			return;
	}

	for (auto iter = _openList.begin(); iter != _openList.end(); ++iter)
	{
		Node* node = *iter;
		if (node->xPos == point.xPos && node->yPos == point.yPos)
		{
			// 생성하려는 노드가 오픈리스트에 이미 존재할 경우 G 값을 비교하여 최적값으로 갱신한다.
			if (node->G > g)
			{
				_openList.erase(iter);
				node->G = g;
				node->F = node->G + node->H;
				node->direction = point.direction;
				node->parent = parent;
				_openList.insert(node);
			}
			return;
		}
	}

	Node* node = _objectPool.Alloc();
	node->G = g;
	node->H = abs(_destination.xPos - point.xPos) + abs(_destination.yPos - point.yPos);
	node->F = node->G + node->H;
	node->xPos = point.xPos;
	node->yPos = point.yPos;
	node->direction = point.direction;
	node->parent = parent;
	_openList.insert(node);
}
bool JumpPointSearch::SearchLLCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LL;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}
		
		// LL 방향의 좌상측 코너를 확인한다.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LL 방향의 좌하측 코너를 확인한다.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		if (direction > NODE_DIRECTION_LL)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x--;
	}
	return false;
}
bool JumpPointSearch::SearchLUCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LU;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos	= y;
			return true;
		}
		
		// LU 방향의 우상측 코너를 확인한다.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// LU 방향의 좌하측 코너를 확인한다.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// LU 방향의 상측 코너를 탐색한다.
		if (SearchUUCorner(x, y - 1, point))
			direction |= NODE_DIRECTION_UU;

		// LU 방향의 좌측 코너를 탐색한다.
		if (SearchLLCorner(x - 1, y, point))
			direction |= NODE_DIRECTION_LL;

		if (direction > NODE_DIRECTION_LU)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x--;
		y--;
	}
	return false;
}
bool JumpPointSearch::SearchUUCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_UU;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}
		
		// UU 방향의 좌상측 코너를 확인한다.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// UU 방향의 우상측 코너를 확인한다.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		if (direction > NODE_DIRECTION_UU)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		y--;
	}
	return false;
}
bool JumpPointSearch::SearchRUCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RU;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RU 방향의 좌상측 코너를 확인한다.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// RU 방향의 우하측 코너를 확인한다.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		// RU 방향의 상측 코너를 탐색한다.
		if (SearchUUCorner(x, y - 1, point))
			direction |= NODE_DIRECTION_UU;

		// RU 방향의 우측 코너를 탐색한다.
		if (SearchRRCorner(x + 1, y, point))
			direction |= NODE_DIRECTION_RR;

		if (direction > NODE_DIRECTION_RU)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x++;
		y--;
	}
	return false;
}
bool JumpPointSearch::SearchRRCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RR;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RR 방향의 우상측 코너를 확인한다.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RR 방향의 우하측 코너를 확인한다.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		if (direction > NODE_DIRECTION_RR)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x++;
	}
	return false;
}
bool JumpPointSearch::SearchRDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RD;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RD 방향의 우상측 코너를 확인한다.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RD 방향의 좌하측 코너를 확인한다.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// RD 방향의 우측 코너를 탐색한다.
		if (SearchRRCorner(x + 1, y, point))
			direction |= NODE_DIRECTION_RR;

		// RD 방향의 하측 코너를 탐색한다.
		if (SearchDDCorner(x, y + 1, point))
			direction |= NODE_DIRECTION_DD;

		if (direction > NODE_DIRECTION_RD)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x++;
		y++;
	}
	return false;
}
bool JumpPointSearch::SearchDDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_DD;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// DD 방향의 좌하측 코너를 확인한다.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// DD 방향의 우상측 코너를 확인한다.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		if (direction > NODE_DIRECTION_DD)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		y++;
	}
	return false;
}
bool JumpPointSearch::SearchLDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LD;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		};
		
		// LD 방향의 좌상측 코너를 탐색한다.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LD 방향의 우하측 코너를 탐색한다.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		// LD 방향의 좌측 코너를 탐색한다.
		if (SearchLLCorner(x - 1, y, point))
			direction |= NODE_DIRECTION_LL;

		// LD 방향의 하측 코너를 탐색한다.
		if (SearchDDCorner(x, y + 1, point))
			direction |= NODE_DIRECTION_DD;

		if (direction > NODE_DIRECTION_LD)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		x--;
		y++;
	}
	return false;
}
bool JumpPointSearch::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
void JumpPointSearch::OptimizeStraightPath(std::list<Point> & route)
{
	int index = 0;
	auto iter = route.begin();
	while (index < route.size() - 2)
	{
		auto source = iter;
		auto way = ++iter;
		auto dest = ++iter;

		bool straightPath = true;
		StraightLine::Point point;
		_bresenham.Line(source->xPos, source->yPos, dest->xPos, dest->yPos);
		while (_bresenham.GetPoint(&point))
		{
			if (!_IsMovableCB(point.xPos, point.yPos))
			{
				straightPath = false;
				break;
			}
			_bresenham.NextPoint();
		}

		if (straightPath)
		{
			route.erase(way);
			iter = source;
		}
		else
		{
			iter = way;
			index++;
		}
	}
}
