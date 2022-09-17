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
		// ��ã�⸦ ���� �ʱ�ȭ �۾� ����
		DestroyList();
		{
			// ���� ��� ����
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
			// ���¸���Ʈ���� ��� ������
			auto iter = _openList.begin();
			if (iter == _openList.end())
			{
				_state = Astar_GDI::NOWAY;
				return true;
			}

			// ���¸���Ʈ���� ���� ��带 Ŭ�����Ʈ�� �̵�
			Node* node = *iter;
			_openList.erase(iter);
			_closeList.insert(node);

			// ��� ��ǥ�� ���������� Ȯ��
			if (node->xPos == _destination.xPos && node->yPos == _destination.yPos)
			{
				// ��� ��ǥ�� �������� ��� �ѹ� �� �����Ѵ�. (GDI ����� ������ JPS �˰��� �Լ��� �����Ͽ� ����)
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

			// ��� ��ǥ�� �������� �ƴ� ��� 8���⼭ġ ����
			MakeEightDirectionNode(node);
			_traveler = node;
		}
		break;
	case Astar_GDI::ARRIVAL:
	case Astar_GDI::NOWAY:
		// �ش� �Լ��� ��ã�� �Ϸ� ���� ȣ�� �Ǿ����Ƿ� ��ã�⸦ �ٽ� �����Ѵ�.
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
	// Ÿ���� �׸���.
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
	// ��ֹ��� �׸���.
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
			// �׵θ� ũ�Ⱑ �����Ƿ� +2�� ���ش�.
			Rectangle(hdc, iX, iY, iX + scale + 2, iY + scale + 2);
		}
	}
	SelectObject(hdc, hOldBrush);
}
void Astar_GDI::RenderPathfinding(HDC hdc, INT screanX, INT screanY, INT scale)
{
	int iX;
	int iY;

	// ���¸���Ʈ�� �׸���.
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

	// Ŭ�����Ʈ�� �׸���.
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

	// ������� �������� �׸���.
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

	// ��ã�� ���� ������Ʈ�� �׸���.
	switch (_state)
	{
	case Astar_GDI::DEPARTURE:
		break;
	case Astar_GDI::TRAVLING:
		{
			// ���� ã�� �������� ��带 ��ĥ�Ѵ�.
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
			// ã�� �Ϸ��� ���� �ش� ����� �θ� ������ �̾��ش�.
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
	// ���¸���Ʈ�� Ŭ�����Ʈ�� �����Ѵ�.
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

	// �����Ϸ��� ��尡 Ŭ�����Ʈ�� �̹� ������ ��� �湮�ߴ� Ÿ���̹Ƿ� �������� �ʰ� ������.
	for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		Node* node = *iter;
		if (node->xPos == x && node->yPos == y)
			return;
	}

	// �����Ϸ��� ��尡 ���¸���Ʈ�� �̹� ������ ��� G ���� ���Ͽ� ���������� �����Ѵ�.
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

	// Direction Left UP (�밢��)
	if (IsMovable(parent->xPos - 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos - 1, true);

	// Direction UP
	if (IsMovable(parent->xPos, parent->yPos - 1))
		MakeNode(parent, parent->xPos, parent->yPos - 1, false);

	// Direction Right UP (�밢��)
	if (IsMovable(parent->xPos + 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos - 1, true);

	// Direction Right
	if (IsMovable(parent->xPos + 1, parent->yPos))
		MakeNode(parent, parent->xPos + 1, parent->yPos, false);

	// Direction Right Down (�밢��)
	if (IsMovable(parent->xPos + 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos + 1, true);

	// Direction Down
	if (IsMovable(parent->xPos, parent->yPos + 1))
		MakeNode(parent, parent->xPos, parent->yPos + 1, false);

	// Direction Left Down (�밢��)
	if (IsMovable(parent->xPos - 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos + 1, true);
}
bool Astar_GDI::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
