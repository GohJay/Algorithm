#pragma once
#include "../Common/IPathFinding.h"
#include <functional>
#include <set>

class Astar : public IPathFinding
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
		unsigned char direction;
		Node* parent;
	};
public:
	template<typename T>
	Astar(bool(T::* mem_func)(int, int), T* obj);
	Astar(bool(*func)(int, int));
	~Astar();
public:
	bool FindPath(int srcX, int srcY, int dstX, int dstY, std::list<Point>& answer);
private:
	void DestroyList();
	void MakeNode(Node* parent, int x, int y, bool diagonal);
	void MakeEightDirectionNode(Node* parent);
	bool IsDiagonal(int srcX, int srcY, int dstX, int dstY);
private:
	std::function<bool(int, int)> _IsMovableCB;
	std::multiset<Node*, Node> _openList;
	std::multiset<Node*, Node> _closeList;
	Point _destination;
};
template<typename T>
Astar::Astar(bool(T::* mem_func)(int, int), T * obj)
{
	_IsMovableCB = std::bind(mem_func, obj, std::placeholders::_1, std::placeholders::_2);
}
