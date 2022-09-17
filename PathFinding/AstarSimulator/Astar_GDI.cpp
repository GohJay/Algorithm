#include "stdafx.h"
#include "Astar_GDI.h"

Astar_GDI::Astar_GDI(char map[][MAX_WIDTH], int width, int height)
	: _map(map), _width(width), _height(height), _state(Astar_GDI::DEPARTURE)
{
	_Astar = new Astar(&Astar_GDI::IsMovable, this);
	_hPen[GRID] = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	_hPen[TRAVLING] = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	_hBrush[OBSTACLE] = CreateSolidBrush(RGB(100, 100, 100));
	_hBrush[SOURCE] = CreateSolidBrush(RGB(0, 0, 255));
	_hBrush[DEST] = CreateSolidBrush(RGB(255, 0, 0));
	_hBrush[OPEN] = CreateSolidBrush(RGB(75, 137, 220));
	_hBrush[CLOSE] = CreateSolidBrush(RGB(219, 68, 85));
	_hBrush[TRAVEL] = CreateSolidBrush(RGB(255, 255, 0));
}
Astar_GDI::~Astar_GDI()
{
	DeleteObject(_hPen[GRID]);
	DeleteObject(_hPen[TRAVLING]);
	DeleteObject(_hBrush[OBSTACLE]);
	DeleteObject(_hBrush[SOURCE]);
	DeleteObject(_hBrush[DEST]);
	DeleteObject(_hBrush[OPEN]);
	DeleteObject(_hBrush[CLOSE]);
	DestroyList();
	delete _Astar;
}
void Astar_GDI::SetSource(int srcX, int srcY)
{
	_source.xPos = srcX;
	_source.yPos = srcY;
	_state = Astar_GDI::DEPARTURE;
}
void Astar_GDI::SetDestination(int dstX, int dstY)
{
	_destination.xPos = dstX;
	_destination.yPos = dstY;
	_state = Astar_GDI::DEPARTURE;
}
bool Astar_GDI::FindPathOnce()
{
	switch (_state)
	{
	case Astar_GDI::DEPARTURE:
		// 길찾기를 위한 초기화 작업 수행
		DestroyList();
		{
			// 시작 노드 생성
			Node* node = new Node;
			node->G = 0.0f;
			node->H = abs(_destination.xPos - _source.xPos) + abs(_destination.yPos - _source.yPos);
			node->F = node->G + node->H;
			node->xPos = _source.xPos;
			node->yPos = _source.yPos;
			node->parent = nullptr;
			_openList.insert(node);
		}
		_state = Astar_GDI::TRAVLING;
	case Astar_GDI::TRAVLING:
		{
			// 오픈리스트에서 노드 꺼내기
			auto iter = _openList.begin();
			if (iter == _openList.end())
			{
				_state = Astar_GDI::NOWAY;
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
				std::list<Astar::Point> answer;
				if (!_Astar->FindPath(_source.xPos, _source.yPos, _destination.xPos, _destination.yPos, answer))
					throw;

				_tracker = node;
				while (node)
				{
					Astar::Point point = answer.back();
					if (node->xPos != point.xPos || node->yPos != point.yPos)
						throw;

					answer.pop_back();
					node = node->parent;
				}
				_state = Astar_GDI::ARRIVAL;
				return true;
			}

			// 노드 좌표가 도착지가 아닐 경우 8방향서치 수행
			MakeEightDirectionNode(node);
			_traveler = node;
		}
		break;
	case Astar_GDI::ARRIVAL:
	case Astar_GDI::NOWAY:
		// 해당 함수가 길찾기 완료 이후 호출 되었으므로 길찾기를 다시 수행한다.
		_state = Astar_GDI::DEPARTURE;
		return FindPathOnce();
	default:
		break;
	}
	return false;
}
void Astar_GDI::Cleanup()
{
	DestroyList();
	_state = Astar_GDI::DEPARTURE;
}
void Astar_GDI::Render(HDC hdc, INT screanX, INT screanY, INT scale)
{
	RenderObstacle(hdc, screanX, screanY, scale);
	RenderPathfinding(hdc, screanX, screanY, scale);
	RenderGrid(hdc, screanX, screanY, scale);
}
void Astar_GDI::RenderGrid(HDC hdc, INT screanX, INT screanY, INT scale)
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
void Astar_GDI::RenderObstacle(HDC hdc, INT screanX, INT screanY, INT scale)
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
void Astar_GDI::RenderPathfinding(HDC hdc, INT screanX, INT screanY, INT scale)
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
	case Astar_GDI::DEPARTURE:
		break;
	case Astar_GDI::TRAVLING:
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
	case Astar_GDI::ARRIVAL:
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
	case Astar_GDI::NOWAY:
		break;
	default:
		break;
	}
}
void Astar_GDI::DestroyList()
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
void Astar_GDI::MakeNode(Node* parent, int x, int y, bool diagonal)
{
	float g = parent->G;
	if (IsDiagonal(parent->xPos, parent->yPos, x, y))
		g += 1.5f;
	else
		g += 1.0f;

	// 생성하려는 노드가 클로즈리스트에 이미 존재할 경우 방문했던 타일이므로 생성하지 않고 나간다.
	for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		Node* node = *iter;
		if (node->xPos == x && node->yPos == y)
			return;
	}

	// 생성하려는 노드가 오픈리스트에 이미 존재할 경우 G 값을 비교하여 최적값으로 갱신한다.
	for (auto iter = _openList.begin(); iter != _openList.end(); ++iter)
	{
		Node* node = *iter;
		if (node->xPos == x && node->yPos == y)
		{
			if (node->G > g)
			{
				_openList.erase(iter);
				node->G = g;
				node->F = node->G + node->H;
				node->parent = parent;
				_openList.insert(node);
			}
			return;
		}
	}
	
	Node* node = new Node;
	node->G = g;
	node->H = abs(_destination.xPos - x) + abs(_destination.yPos - y);
	node->F = node->G + node->H;
	node->xPos = x;
	node->yPos = y;
	node->parent = parent;
	_openList.insert(node);
}
void Astar_GDI::MakeEightDirectionNode(Node * parent)
{
	// Direction Left
	if (IsMovable(parent->xPos - 1, parent->yPos))
		MakeNode(parent, parent->xPos - 1, parent->yPos, false);

	// Direction Left UP (대각선)
	if (IsMovable(parent->xPos - 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos - 1, true);

	// Direction UP
	if (IsMovable(parent->xPos, parent->yPos - 1))
		MakeNode(parent, parent->xPos, parent->yPos - 1, false);

	// Direction Right UP (대각선)
	if (IsMovable(parent->xPos + 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos - 1, true);

	// Direction Right
	if (IsMovable(parent->xPos + 1, parent->yPos))
		MakeNode(parent, parent->xPos + 1, parent->yPos, false);

	// Direction Right Down (대각선)
	if (IsMovable(parent->xPos + 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos + 1, true);

	// Direction Down
	if (IsMovable(parent->xPos, parent->yPos + 1))
		MakeNode(parent, parent->xPos, parent->yPos + 1, false);

	// Direction Left Down (대각선)
	if (IsMovable(parent->xPos - 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos + 1, true);
}
bool Astar_GDI::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
