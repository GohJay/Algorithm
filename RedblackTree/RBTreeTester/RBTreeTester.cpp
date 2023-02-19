#include "RedblackTree_Test.h"
#include <stdio.h>

#define COUNT 1000000

int main()
{
	RedblackTree_Test RBTreeTester;
	for (int i = 0; i < COUNT; i++)
		RBTreeTester.Insert(i);

	for (int i = 0; i < COUNT / 2; i++)
		RBTreeTester.Remove(i);

	//RBTreeTester.Print();
	RBTreeTester.Verify();
	printf_s("count: %d\n", RBTreeTester.size());
	return 0;
}
