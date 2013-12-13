#include<iostream>
using namespace std;
struct NODE{
	NODE * pLeft;
	NODE * pRight;
};
struct RESULT{
	int nMaxDistance;
	int nMaxDepth;
};
int zmax(int i,int j)
{
	return (i>=j?i:j);
}
RESULT GetMaxDistance(NODE * root)
{
	if(!root){
		RESULT empty = {0,-1};
		return empty;
	}
	RESULT lrst = GetMaxDistance(root->pLeft);
	RESULT rrst = GetMaxDistance(root->pRight);
	RESULT r;
	r.nMaxDepth = zmax(lrst.nMaxDepth+1,rrst.nMaxDepth+1);
	r.nMaxDistance = zmax(zmax(lrst.nMaxDistance,rrst.nMaxDistance),lrst.nMaxDepth+rrst.nMaxDepth+2);
	return r;
}
void Link(NODE * nodes,int p,int l,int r)
{
	if(l != -1){
		nodes[p].pLeft = &nodes[l];
	}
	if(r != -1){
		nodes[p].pRight = &nodes[r];
	}
	return;
}
int main()
{
	NODE t[9] = {0};
	Link(t,0,1,2);
	Link(t,1,3,4);
	Link(t,2,5,6);
	Link(t,3,7,-1);
	Link(t,5,-1,8);
	cout<<GetMaxDistance(&t[0]).nMaxDistance<<endl;

	NODE test2[4] = { 0 };
	Link(test2, 0, 1, 2);
	Link(test2, 1, 3, -1);
	cout << "test2: " << GetMaxDistance(&test2[0]).nMaxDistance << endl;

	NODE test3[9] = { 0 };
	Link(test3, 0, -1, 1);
	Link(test3, 1, 2, 3);
	Link(test3, 2, 4, -1);
	Link(test3, 3, 5, 6);
	Link(test3, 4, 7, -1);
	Link(test3, 5, -1, 8);
	cout << "test3: " << GetMaxDistance(&test3[0]).nMaxDistance << endl;

	NODE test4[9] = { 0 };
	Link(test4, 0, 1, 2);
	Link(test4, 1, 3, 4);
	Link(test4, 3, 5, 6);
	Link(test4, 5, 7, -1);
	Link(test4, 6, -1, 8);
	cout << "test4: " << GetMaxDistance(&test4[0]).nMaxDistance << endl;
	return 0;
}
