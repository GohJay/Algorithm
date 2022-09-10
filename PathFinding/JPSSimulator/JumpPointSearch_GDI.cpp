#include "stdafx.h"
#include "JumpPointSearch_GDI.h"

#define NODE_DIRECTION_LL		1
#define NODE_DIRECTION_LU		NODE_DIRECTION_LL << 1
#define NODE_DIRECTION_UU		NODE_DIRECTION_LL << 2
#define NODE_DIRECTION_RU		NODE_DIRECTION_LL << 3
#define NODE_DIRECTION_RR		NODE_DIRECTION_LL << 4
#define NODE_DIRECTION_RD		NODE_DIRECTION_LL << 5
#define NODE_DIRECTION_DD		NODE_DIRECTION_LL << 6
#define NODE_DIRECTION_LD		NODE_DIRECTION_LL << 7
#define NODE_DIRECTION_ALL		0xff

JumpPointSearch_GDI::JumpPointSearch_GDI(char map[][MAX_WIDTH], int width, int height)
	: _map(map), _width(width), _height(height), _state(JPS_GDI::DEPARTURE)
{
	_JPS = new JPS(&JPS_GDI::IsMovable, this);
	InitColorTable();
	InitColorMap();
	_hPen[GRID] = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	_hPen[TRAVLING] = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	_hBrush[OBSTACLE] = CreateSolidBrush(RGB(100, 100, 100));
	_hBrush[SOURCE] = CreateSolidBrush(RGB(0, 0, 255));
	_hBrush[DEST] = CreateSolidBrush(RGB(255, 0, 0));
	_hBrush[OPEN] = CreateSolidBrush(RGB(75, 137, 220));
	_hBrush[CLOSE] = CreateSolidBrush(RGB(219, 68, 85));
	_hBrush[TRAVEL] = CreateSolidBrush(RGB(255, 255, 0));
}
JumpPointSearch_GDI::~JumpPointSearch_GDI()
{
	DeleteObject(_hPen[GRID]);
	DeleteObject(_hPen[TRAVLING]);
	DeleteObject(_hBrush[OBSTACLE]);
	DeleteObject(_hBrush[SOURCE]);
	DeleteObject(_hBrush[DEST]);
	DeleteObject(_hBrush[OPEN]);
	DeleteObject(_hBrush[CLOSE]);
	DestroyList();
	delete[] _colorMap;
	delete _colorTable;
	delete _JPS;
}
void JumpPointSearch_GDI::SetSource(int srcX, int srcY)
{
	_source.xPos = srcX;
	_source.yPos = srcY;
	_state = JPS_GDI::DEPARTURE;
}
void JumpPointSearch_GDI::SetDestination(int dstX, int dstY)
{
	_destination.xPos = dstX;
	_destination.yPos = dstY;
	_state = JPS_GDI::DEPARTURE;
}
bool JumpPointSearch_GDI::FindPathOnce()
{
_ENTRY_POINT:
	switch (_state)
	{
	case JPS_GDI::DEPARTURE:
		// 길찾기를 위한 초기화 작업 수행
		ResetColorMap();
		DestroyList();
		{
			// 시작 노드 생성
			Node* node = new Node;
			node->G = 0.0f;
			node->H = abs(_destination.xPos - _source.xPos) + abs(_destination.yPos - _source.yPos);
			node->F = node->G + node->H;
			node->xPos = _source.xPos;
			node->yPos = _source.yPos;
			node->direction = NODE_DIRECTION_ALL;
			node->parent = nullptr;
			_openList.insert(node);
		}
		_state = JPS_GDI::TRAVLING;
	case JPS_GDI::TRAVLING:
		{
			// 오픈리스트에서 노드 꺼내기
			auto iter = _openList.begin();
			if (iter == _openList.end())
			{
				_state = JPS_GDI::NOWAY;
				return true;
			}

			// 오픈리스트에서 꺼낸 노드를 클로즈리스트로 이동
			Node* node = *iter;
			_openList.erase(iter);
			_closeList.insert(node);

			// 노드 좌표가 도착지인지 확인
			if (node->xPos == _destination.xPos && node->yPos == _destination.yPos)
			{
				// 노드 좌표가 도착지일 경우 한번 더 검증한다. (GDI 기능을 제거한 JPS 알고리즘 함수를 수행하여 검증)
				std::list<JPS::Point> answer;
				if (!_JPS->FindPath(_source.xPos, _source.yPos, _destination.xPos, _destination.yPos, answer))
					throw;

				_tracker = node;
				while (node)
				{
					JPS::Point point = answer.back();
					if (node->xPos != point.xPos || node->yPos != point.yPos)
						throw;

					answer.pop_back();
					node = node->parent;
				}
				_state = JPS_GDI::ARRIVAL;
				return true;
			}

			// 노드 좌표가 도착지가 아닐 경우 점프포인트서치 수행
			JumpProc(node);
			_traveler = node;
		}
		break;
	case JPS_GDI::ARRIVAL:
	case JPS_GDI::NOWAY:
		// 해당 함수가 길찾기 완료 이후 호출 되었으므로 길찾기를 다시 수행한다.
		_state = JPS_GDI::DEPARTURE;
		goto _ENTRY_POINT;
	default:
		break;
	}
	return false;
}
void JumpPointSearch_GDI::Cleanup()
{
	ResetColorMap();
	DestroyList();
	_state = JPS_GDI::DEPARTURE;
}
void JumpPointSearch_GDI::Render(HDC hdc, INT screanX, INT screanY, INT scale)
{
	RenderTileColor(hdc, screanX, screanY, scale);
	RenderObstacle(hdc, screanX, screanY, scale);
	RenderPathfinding(hdc, screanX, screanY, scale);
	RenderGrid(hdc, screanX, screanY, scale);
}
void JumpPointSearch_GDI::RenderGrid(HDC hdc, INT screanX, INT screanY, INT scale)
{
	// 타일을 그린다.
	HPEN hOldPen = (HPEN)SelectObject(hdc, _hPen[GRID]);
	int iX = 0;
	int iY = 0;
	for (int w = screanX; w <= _width; w++)
	{
		MoveToEx(hdc, iX, 0, NULL);
		LineTo(hdc, iX, _height * scale);
		iX += scale;
	}
	for (int h = screanY; h <= _height; h++)
	{
		MoveToEx(hdc, 0, iY, NULL);
		LineTo(hdc, _width * scale, iY);
		iY += scale;
	}
	SelectObject(hdc, hOldPen);
}
void JumpPointSearch_GDI::RenderObstacle(HDC hdc, INT screanX, INT screanY, INT scale)
{
	// 장애물을 그린다.
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, _hBrush[OBSTACLE]);
	SelectObject(hdc, GetStockObject(NULL_PEN));
	int iX = 0;
	int iY = 0;
	for (int w = screanX; w < _width; w++)
	{
		for (int h = screanY; h < _height; h++)
		{
			if (!_map[h][w])
				continue;

			iX = (w - screanX) * scale;
			iY = (h - screanY) * scale;
			// 테두리 크기가 있으므로 +2를 해준다.
			Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
		}
	}
	SelectObject(hdc, hOldBrush);
}
void JumpPointSearch_GDI::RenderPathfinding(HDC hdc, INT screanX, INT screanY, INT scale)
{
	int iX;
	int iY;

	// 오픈리스트를 그린다.
	if (!_openList.empty())
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, _hBrush[OPEN]);
		for (auto iter = _openList.begin(); iter != _openList.end(); ++iter)
		{
			Node* node = *iter;
			if (node->xPos < screanX || node->yPos < screanY)
				continue;

			iX = (node->xPos - screanX) * scale;
			iY = (node->yPos - screanY) * scale;
			Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);

			if (scale >= 80)
			{
				char data[32];
				sprintf_s(data, "G: %.1f", node->G);
				TextOutA(hdc, iX + 5, iY + 5, data, strlen(data));
				sprintf_s(data, "H: %.1f", node->H);
				TextOutA(hdc, iX + 5, iY + 25, data, strlen(data));
				sprintf_s(data, "F: %.1f", node->F);
				TextOutA(hdc, iX + 5, iY + 45, data, strlen(data));
				sprintf_s(data, "X: %d, Y: %d", node->xPos, node->yPos);
				TextOutA(hdc, iX + 5, iY + 65, data, strlen(data));
			}
		}
		SelectObject(hdc, hOldBrush);
	}

	// 클로즈리스트를 그린다.
	if (!_closeList.empty())
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, _hBrush[CLOSE]);
		for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
		{
			Node* node = *iter;
			if (node->xPos < screanX || node->yPos < screanY)
				continue;

			iX = (node->xPos - screanX) * scale;
			iY = (node->yPos - screanY) * scale;
			Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);

			if (scale >= 80)
			{
				char data[32];
				sprintf_s(data, "G: %.1f", node->G);
				TextOutA(hdc, iX + 5, iY + 5, data, strlen(data));
				sprintf_s(data, "H: %.1f", node->H);
				TextOutA(hdc, iX + 5, iY + 25, data, strlen(data));
				sprintf_s(data, "F: %.1f", node->F);
				TextOutA(hdc, iX + 5, iY + 45, data, strlen(data));
				sprintf_s(data, "X: %d, Y: %d", node->xPos, node->yPos);
				TextOutA(hdc, iX + 5, iY + 65, data, strlen(data));
			}
		}
		SelectObject(hdc, hOldBrush);
	}

	// 출발점과 도착점을 그린다.
	if (_source.xPos >= screanX && _source.yPos >= screanY)
	{
		HBRUSH hSrcBrush = (HBRUSH)SelectObject(hdc, _hBrush[SOURCE]);
		iX = (_source.xPos - screanX) * scale;
		iY = (_source.yPos - screanY) * scale;
		Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
		SelectObject(hdc, hSrcBrush);
	}
	if (_destination.xPos >= screanX && _destination.yPos >= screanY)
	{
		HBRUSH hDstBrush = (HBRUSH)SelectObject(hdc, _hBrush[DEST]);
		iX = (_destination.xPos - screanX) * scale;
		iY = (_destination.yPos - screanY) * scale;
		Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
		SelectObject(hdc, hDstBrush);
	}

	// 길찾기 관련 오브젝트를 그린다.
	switch (_state)
	{
	case JPS_GDI::DEPARTURE:
		break;
	case JPS_GDI::TRAVLING:
		{
			// 현재 찾기 진행중인 노드를 색칠한다.
			if (_traveler->xPos >= screanX && _traveler->yPos >= screanY)
			{
				HBRUSH hDstBrush = (HBRUSH)SelectObject(hdc, _hBrush[TRAVEL]);
				iX = (_traveler->xPos - screanX) * scale;
				iY = (_traveler->yPos - screanY) * scale;
				Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
				SelectObject(hdc, hDstBrush);
			}
		}
		break;
	case JPS_GDI::ARRIVAL:
		{
			// 찾기 완료한 노드와 해당 노드의 부모를 선으로 이어준다.
			HPEN hOldPen = (HPEN)SelectObject(hdc, _hPen[TRACKING]);
			Node* node = _tracker;
			for (;;)
			{
				if (node == nullptr)
					break;

				if (node->xPos == _source.xPos && node->yPos == _source.yPos)
					break;

				MoveToEx(hdc, ((node->xPos - screanX) * scale) + (scale / 2), ((node->yPos - screanY) * scale) + (scale / 2), NULL);
				LineTo(hdc, ((node->parent->xPos - screanX) * scale) + (scale / 2), ((node->parent->yPos - screanY) * scale) + (scale / 2));
				node = node->parent;
			}
			SelectObject(hdc, hOldPen);
		}
		break;
	case JPS_GDI::NOWAY:
		break;
	default:
		break;
	}
}
void JumpPointSearch_GDI::RenderTileColor(HDC hdc, INT screanX, INT screanY, INT scale)
{
	// 타일을 색칠한다.
	SelectObject(hdc, GetStockObject(NULL_PEN));
	int iX = 0;
	int iY = 0;
	for (int w = screanX; w < _width; w++)
	{
		for (int h = screanY; h < _height; h++)
		{
			if (_map[h][w])
				continue;

			COLORREF color = _colorTable[_colorMap[h][w]];
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(color));
			iX = (w - screanX) * scale;
			iY = (h - screanY) * scale;
			Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
			DeleteObject(SelectObject(hdc, (HBRUSH)hOldBrush));
		}
	}
}
void JumpPointSearch_GDI::InitColorTable()
{
	_colorTable = new COLORREF[_width * _height];
	_colorTable[0] = RGB(255, 255, 255);
	for (int i = 1; i < _width * _height; i++)
	{
		_colorTable[i] = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
}
void JumpPointSearch_GDI::InitColorMap()
{
	_colorMap = new int*[_height];
	for (int i = 0; i < _height; i++)
	{
		_colorMap[i] = new int[_width];
		memset(_colorMap[i], 0, sizeof(int) * _width);
	}
}
void JumpPointSearch_GDI::ResetColorMap()
{
	for (int i = 0; i < _height; i++)
		memset(_colorMap[i], 0, sizeof(int) * _width);
	_selColor = 1;
}
void JumpPointSearch_GDI::DestroyList()
{
	// 오픈리스트와 클로즈리스트를 정리한다.
	for (auto iter = _openList.begin(); iter != _openList.end();)
	{
		Node* node = *iter;
		delete node;
		iter = _openList.erase(iter);
	}
	for (auto iter = _closeList.begin(); iter != _closeList.end();)
	{
		Node* node = *iter;
		delete node;
		iter = _closeList.erase(iter);
	}
}
void JumpPointSearch_GDI::JumpProc(Node * node)
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
	_selColor++;
}
void JumpPointSearch_GDI::MakeNode(Node * parent, const JumpPoint& point)
{
	float g = parent->G;
	if (IsDiagonal(parent->xPos, parent->yPos, point.xPos, point.yPos))
		g += 1.5f;
	else
		g += 1.0f;
	
	for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		// 생성하려는 노드가 클로즈리스트에 이미 존재할 경우 방문했던 타일이므로 생성하지 않고 나간다.
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

	Node* node = new Node;
	node->G = g;
	node->H = abs(_destination.xPos - point.xPos) + abs(_destination.yPos - point.yPos);
	node->F = node->G + node->H;
	node->xPos = point.xPos;
	node->yPos = point.yPos;
	node->direction = point.direction;
	node->parent = parent;
	_openList.insert(node);
}
bool JumpPointSearch_GDI::SearchLLCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x, y - 1) && IsMovable(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LL 방향의 좌하측 코너를 확인한다.
		if (!IsMovable(x, y + 1) && IsMovable(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		if (direction > NODE_DIRECTION_LL)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x--;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchLUCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LU;

		// 방문한 타일이 도착지인지 확인한다.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// LU 방향의 우상측 코너를 확인한다.
		if (!IsMovable(x + 1, y) && IsMovable(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// LU 방향의 좌하측 코너를 확인한다.
		if (!IsMovable(x, y + 1) && IsMovable(x - 1, y + 1))
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

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x--;
		y--;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchUUCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x - 1, y) && IsMovable(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// UU 방향의 우상측 코너를 확인한다.
		if (!IsMovable(x + 1, y) && IsMovable(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		if (direction > NODE_DIRECTION_UU)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		y--;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchRUCorner(int x, int y, JumpPoint* point)
{
	while (IsMovable(x, y))
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
		if (!IsMovable(x - 1, y) && IsMovable(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// RU 방향의 우하측 코너를 확인한다.
		if (!IsMovable(x, y + 1) && IsMovable(x + 1, y + 1))
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

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x++;
		y--;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchRRCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x, y - 1) && IsMovable(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RR 방향의 우하측 코너를 확인한다.
		if (!IsMovable(x, y + 1) && IsMovable(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		if (direction > NODE_DIRECTION_RR)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x++;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchRDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x, y - 1) && IsMovable(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RD 방향의 좌하측 코너를 확인한다.
		if (!IsMovable(x - 1, y) && IsMovable(x - 1, y + 1))
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

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x++;
		y++;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchDDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x - 1, y) && IsMovable(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// DD 방향의 우상측 코너를 확인한다.
		if (!IsMovable(x + 1, y) && IsMovable(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		if (direction > NODE_DIRECTION_DD)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		y++;
	}
	return false;
}
bool JumpPointSearch_GDI::SearchLDCorner(int x, int y, JumpPoint* point)
{
	// 방문한 타일이 이동 가능한 좌표인지 확인한다.
	while (IsMovable(x, y))
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
		if (!IsMovable(x, y - 1) && IsMovable(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LD 방향의 우하측 코너를 탐색한다.
		if (!IsMovable(x + 1, y) && IsMovable(x + 1, y + 1))
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

		// 방문한 타일의 색상을 변경한다.
		_colorMap[y][x] = _selColor;

		x--;
		y++;
	}
	return false;
}
bool JumpPointSearch_GDI::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
