#include "Astar.h"

Astar::Astar(std::function<bool(int, int)> callback) : _IsMovableCB(callback), _objectPool(0, false)
{
}
Astar::~Astar()
{
	DestroyList();
}
bool Astar::FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& answer)
{
	// ��ã�⸦ ���� �ʱ�ȭ �۾� ����
	_destination.xPos = dstX;
	_destination.yPos = dstY;

	// ���� ��� ����
	NODE* node = _objectPool.Alloc();
	node->G = 0.0f;
	node->H = abs(dstX - srcX) + abs(dstY - srcY);
	node->F = node->G + node->H;
	node->xPos = srcX;
	node->yPos = srcY;
	node->parent = nullptr;
	_openList.insert(node);

	while (!_openList.empty())
	{
		// ���¸���Ʈ���� ��� ������
		auto iter = _openList.begin();
		NODE* node = *iter;

		// ���¸���Ʈ���� ���� ��带 Ŭ�����Ʈ�� �̵�
		_openList.erase(iter);
		_closeList.push_back(node);

		// ��� ��ǥ�� ���������� Ȯ��
		if (node->xPos == dstX && node->yPos == dstY)
		{
			while (node)
			{
				answer.emplace_front(node->xPos, node->yPos);
				node = node->parent;
			}
			DestroyList();
			return true;
		}

		// ��� ��ǥ�� �������� �ƴ� ��� 8���⼭ġ ����
		MakeEightDirectionNode(node);
	}
	DestroyList();
	return false;
}
void Astar::DestroyList()
{
	// ���¸���Ʈ�� Ŭ�����Ʈ�� �����Ѵ�.
	for (auto iter = _openList.begin(); iter != _openList.end();)
	{
		NODE* node = *iter;
		_objectPool.Free(node);
		iter = _openList.erase(iter);
	}
	for (auto iter = _closeList.begin(); iter != _closeList.end();)
	{
		NODE* node = *iter;
		_objectPool.Free(node);
		iter = _closeList.erase(iter);
	}
}
void Astar::MakeNode(NODE * parent, int x, int y, bool diagonal)
{
	float g = parent->G;
	if (IsDiagonal(parent->xPos, parent->yPos, x, y))
		g += 1.5f;
	else
		g += 1.0f;

	// �����Ϸ��� ��尡 Ŭ�����Ʈ�� �̹� ������ ��� �湮�ߴ� Ÿ���̹Ƿ� �������� �ʰ� ������.
	for (auto iter = _closeList.begin(); iter != _closeList.end(); ++iter)
	{
		NODE* node = *iter;
		if (node->xPos == x && node->yPos == y)
			return;
	}

	// �����Ϸ��� ��尡 ���¸���Ʈ�� �̹� ������ ��� G ���� ���Ͽ� ���������� �����Ѵ�.
	for (auto iter = _openList.begin(); iter != _openList.end(); ++iter)
	{
		NODE* node = *iter;
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

	NODE* node = _objectPool.Alloc();
	node->G = g;
	node->H = abs(_destination.xPos - x) + abs(_destination.yPos - y);
	node->F = node->G + node->H;
	node->xPos = x;
	node->yPos = y;
	node->parent = parent;
	_openList.insert(node);
}
void Astar::MakeEightDirectionNode(NODE * parent)
{
	// Direction Left
	if (_IsMovableCB(parent->xPos - 1, parent->yPos))
		MakeNode(parent, parent->xPos - 1, parent->yPos, false);

	// Direction Left UP (�밢��)
	if (_IsMovableCB(parent->xPos - 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos - 1, true);

	// Direction UP
	if (_IsMovableCB(parent->xPos, parent->yPos - 1))
		MakeNode(parent, parent->xPos, parent->yPos - 1, false);

	// Direction Right UP (�밢��)
	if (_IsMovableCB(parent->xPos + 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos - 1, true);

	// Direction Right
	if (_IsMovableCB(parent->xPos + 1, parent->yPos))
		MakeNode(parent, parent->xPos + 1, parent->yPos, false);

	// Direction Right Down (�밢��)
	if (_IsMovableCB(parent->xPos + 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos + 1, true);

	// Direction Down
	if (_IsMovableCB(parent->xPos, parent->yPos + 1))
		MakeNode(parent, parent->xPos, parent->yPos + 1, false);

	// Direction Left Down (�밢��)
	if (_IsMovableCB(parent->xPos - 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos + 1, true);
}
bool Astar::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
