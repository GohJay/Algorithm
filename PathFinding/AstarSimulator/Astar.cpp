#include "Astar.h"

Astar::Astar(bool(*func)(int, int)) : _IsMovableCB(func)
{
}
Astar::~Astar()
{
	DestroyList();
}
bool Astar::FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& answer)
{
	// 길찾기를 위한 초기화 작업 수행
	_destination.xPos = dstX;
	_destination.yPos = dstY;

	// 시작 노드 생성
	Node* node = new Node;
	node->G = 0.0f;
	node->H = abs(dstX - srcX) + abs(dstY - srcY);
	node->F = node->G + node->H;
	node->xPos = srcX;
	node->yPos = srcY;
	node->parent = nullptr;
	_openList.insert(node);

	while (!_openList.empty())
	{
		// 오픈리스트에서 노드 꺼내기
		auto iter = _openList.begin();
		Node* node = *iter;

		// 오픈리스트에서 꺼낸 노드를 클로즈리스트로 이동
		_openList.erase(iter);
		_closeList.insert(node);

		// 노드 좌표가 도착지인지 확인
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

		// 노드 좌표가 도착지가 아닐 경우 8방향서치 수행
		MakeEightDirectionNode(node);
	}
	DestroyList();
	return false;
}
void Astar::DestroyList()
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
void Astar::MakeNode(Node * parent, int x, int y, bool diagonal)
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
void Astar::MakeEightDirectionNode(Node * parent)
{
	// Direction Left
	if (_IsMovableCB(parent->xPos - 1, parent->yPos))
		MakeNode(parent, parent->xPos - 1, parent->yPos, false);

	// Direction Left UP (대각선)
	if (_IsMovableCB(parent->xPos - 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos - 1, true);

	// Direction UP
	if (_IsMovableCB(parent->xPos, parent->yPos - 1))
		MakeNode(parent, parent->xPos, parent->yPos - 1, false);

	// Direction Right UP (대각선)
	if (_IsMovableCB(parent->xPos + 1, parent->yPos - 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos - 1, true);

	// Direction Right
	if (_IsMovableCB(parent->xPos + 1, parent->yPos))
		MakeNode(parent, parent->xPos + 1, parent->yPos, false);

	// Direction Right Down (대각선)
	if (_IsMovableCB(parent->xPos + 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos + 1, parent->yPos + 1, true);

	// Direction Down
	if (_IsMovableCB(parent->xPos, parent->yPos + 1))
		MakeNode(parent, parent->xPos, parent->yPos + 1, false);

	// Direction Left Down (대각선)
	if (_IsMovableCB(parent->xPos - 1, parent->yPos + 1))
		MakeNode(parent, parent->xPos - 1, parent->yPos + 1, true);
}
bool Astar::IsDiagonal(int srcX, int srcY, int dstX, int dstY)
{
	if (srcX != dstX && srcY != dstY)
		return true;

	return false;
}
