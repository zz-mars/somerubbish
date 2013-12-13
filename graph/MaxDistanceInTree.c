#include"glob.h"
typedef struct _node{
	struct _node * l;
	struct _node * r;
}node;
inline int imax(int i,int j)
{
	return (i>=j?i:j);
}
static void get_max_distance(node * root,int * max_dis,int * max_dep)
{
	node * l,*r;
	int l_max_dep,r_max_dep,l_max_dis,r_max_dis;
	if(!root){
		*max_dis = 0;
		*max_dep = -1;
		return;
	}
	l = root->l;
	r = root->r;
	get_max_distance(l,&l_max_dis,&l_max_dep);
	get_max_distance(r,&r_max_dis,&r_max_dep);
	*max_dep = imax(l_max_dep+1,r_max_dep+1);
	*max_dis = imax(imax(l_max_dis,r_max_dis),l_max_dep+r_max_dep+2);
	return;
}
void zlink(node * nodes,int p,int l,int r)
{
	if(l != -1){
		nodes[p].l = &nodes[l];
	}
	if(r != -1){
		nodes[p].r = &nodes[r];
	}
	return;
}
int main()
{
	int max_distance,max_depth;
	node t[9] = {0};
	node test2[4] = { 0 };
	node test3[9] = { 0 };
	node test4[9] = { 0 };
	zlink(t,0,1,2);
	zlink(t,1,3,4);
	zlink(t,2,5,6);
	zlink(t,3,7,-1);
	zlink(t,5,-1,8);
	get_max_distance(&t[0],&max_distance,&max_depth);
	printf("test case 1 : max_distance #%d max_depth #%d\n",max_distance,max_depth);

	zlink(test2, 0, 1, 2);
	zlink(test2, 1, 3, -1);
	get_max_distance(&test2[0],&max_distance,&max_depth);
	printf("test case 2 : max_distance #%d max_depth #%d\n",max_distance,max_depth);

	zlink(test3, 0, -1, 1);
	zlink(test3, 1, 2, 3);
	zlink(test3, 2, 4, -1);
	zlink(test3, 3, 5, 6);
	zlink(test3, 4, 7, -1);
	zlink(test3, 5, -1, 8);
	get_max_distance(&test3[0],&max_distance,&max_depth);
	printf("test case 3 : max_distance #%d max_depth #%d\n",max_distance,max_depth);

	zlink(test4, 0, 1, 2);
	zlink(test4, 1, 3, 4);
	zlink(test4, 3, 5, 6);
	zlink(test4, 5, 7, -1);
	zlink(test4, 6, -1, 8);
	get_max_distance(&test4[0],&max_distance,&max_depth);
	printf("test case 4 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
	return 0;
}
