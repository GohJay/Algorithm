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

JumpPointSearch::JumpPointSearch(bool(*func)(int, int)) : _IsMovableCB(func)
{
}
JumpPointSearch::~JumpPointSearch()
{
	DestroyList();
}
bool JumpPointSearch::FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& route)
{
	// ��ã�⸦ ���� �ʱ�ȭ �۾� ����
	_destination.xPos = dstX;
	_destination.yPos = dstY;

	// ���� ��� ����
	Node* node = new Node;
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
		// ���¸���Ʈ���� ��� ������
		auto iter = _openList.begin();
		Node* node = *iter;

		// ���¸���Ʈ���� ���� ��带 Ŭ�����Ʈ�� �̵�
		_openList.erase(iter);
		_closeList.insert(node);

		// ��� ��ǥ�� ���������� Ȯ��
		if (node->xPos == dstX && node->yPos == dstY)
		{
			// ã�� �Ϸ��� �뼱�� ����Ʈ�� ����
			while (node)
			{
				route.emplace_front(node->xPos, node->yPos);
				node = node->parent;
			}
			
			// ���� ��ο� ���� ����ȭ ���� ���� (�극���� �˰��� ���)
			OptimizeStraightPath(route);

			DestroyList();
			return true;
		}

		// ��� ��ǥ�� �������� �ƴ� ��� ��������Ʈ��ġ ����
		JumpProc(node);
	}
	DestroyList();
	return false;
}
void JumpPointSearch::DestroyList()
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
void JumpPointSearch::JumpProc(Node * node)
{
	// ��������Ʈ��ġ�� �����Ѵ�.
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

	// �����Ϸ��� ��尡 Ŭ�����Ʈ�� �̹� ������ ��� �湮�ߴ� Ÿ���̹Ƿ� �������� �ʰ� ������.
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
			// �����Ϸ��� ��尡 ���¸���Ʈ�� �̹� ������ ��� G ���� ���Ͽ� ���������� �����Ѵ�.
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
bool JumpPointSearch::SearchLLCorner(int x, int y, JumpPoint* point)
{
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LL;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}
		
		// LL ������ �»��� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LL ������ ������ �ڳʸ� Ȯ���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LU;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos	= y;
			return true;
		}
		
		// LU ������ ����� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// LU ������ ������ �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// LU ������ ���� �ڳʸ� Ž���Ѵ�.
		if (SearchUUCorner(x, y - 1, point))
			direction |= NODE_DIRECTION_UU;

		// LU ������ ���� �ڳʸ� Ž���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_UU;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}
		
		// UU ������ �»��� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// UU ������ ����� �ڳʸ� Ȯ���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RU;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RU ������ �»��� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// RU ������ ������ �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x, y + 1) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		// RU ������ ���� �ڳʸ� Ž���Ѵ�.
		if (SearchUUCorner(x, y - 1, point))
			direction |= NODE_DIRECTION_UU;

		// RU ������ ���� �ڳʸ� Ž���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RR;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RR ������ ����� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RR ������ ������ �ڳʸ� Ȯ���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_RD;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// RD ������ ����� �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x + 1, y - 1))
			direction |= NODE_DIRECTION_RU;

		// RD ������ ������ �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// RD ������ ���� �ڳʸ� Ž���Ѵ�.
		if (SearchRRCorner(x + 1, y, point))
			direction |= NODE_DIRECTION_RR;

		// RD ������ ���� �ڳʸ� Ž���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_DD;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		}

		// DD ������ ������ �ڳʸ� Ȯ���Ѵ�.
		if (!_IsMovableCB(x - 1, y) && _IsMovableCB(x - 1, y + 1))
			direction |= NODE_DIRECTION_LD;

		// DD ������ ����� �ڳʸ� Ȯ���Ѵ�.
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
	// �湮�� Ÿ���� �̵� ������ ��ǥ���� Ȯ���Ѵ�.
	while (_IsMovableCB(x, y))
	{
		unsigned char direction = NODE_DIRECTION_LD;

		// �湮�� Ÿ���� ���������� Ȯ���Ѵ�.
		if (x == _destination.xPos && y == _destination.yPos)
		{
			point->direction = direction;
			point->xPos = x;
			point->yPos = y;
			return true;
		};
		
		// LD ������ �»��� �ڳʸ� Ž���Ѵ�.
		if (!_IsMovableCB(x, y - 1) && _IsMovableCB(x - 1, y - 1))
			direction |= NODE_DIRECTION_LU;

		// LD ������ ������ �ڳʸ� Ž���Ѵ�.
		if (!_IsMovableCB(x + 1, y) && _IsMovableCB(x + 1, y + 1))
			direction |= NODE_DIRECTION_RD;

		// LD ������ ���� �ڳʸ� Ž���Ѵ�.
		if (SearchLLCorner(x - 1, y, point))
			direction |= NODE_DIRECTION_LL;

		// LD ������ ���� �ڳʸ� Ž���Ѵ�.
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
