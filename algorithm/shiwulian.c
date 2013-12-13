#include"glob.h"
#define DBGMSG
static int * rank;
static int * father;
static void print_inta(int a[],int len)
{
	int i;
	for(i=0;i<len;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
	return;
}
static void make_set(int x)
{
	rank[x] = 0;
	father[x] = x;
	return;
}
static int get_father(int x)
{
	int prev,prank = rank[x];
#ifdef DBGMSG
	printf("current father of %d is %d\n",x+1,father[x]+1);
#endif
	if(father[x] != x){
		prev = father[x];
		father[x] = get_father(father[x]);
		rank[x] = (prank + rank[prev])%3;
#ifdef DBGMSG
		printf("new father of %d is %d\n",x+1,father[x]+1);
		printf("adjust rank : rank[%d] = (rank[%d] + rank[%d])%%3 = (%d + %d)%%3 = %d\n",x+1,x+1,prev+1,prank,rank[prev],rank[x]);
#endif
	}
	return father[x];
}
static void Union(int len,int x,int y)
{
	int fx = get_father(x);
	int fy = get_father(y);
	int rankx = rank[x];
	int ranky = rank[y];
#ifdef DBGMSG
	printf("union x : %d y : %d fx : %d fy : %d\n",x+1,y+1,fx+1,fy+1);
#endif
	if(fx == fy){
#ifdef DBGMSG
		printf("fx == fy!return\n");
#endif
		return;
	}
	father[fx] = fy;
	rank[fx] = (ranky + 3 - rankx + len)%3;
#ifdef DBGMSG
	printf("father[%d] == %d\n",fx+1,fy+1);
	printf("rank[%d] = (rank[%d] + 3 - rank[%d] + len)%2 = (%d + 3 - %d + %d)%%3 = %d\n",fx+1,y+1,x+1,ranky,rankx,len,rank[fx]);
#endif
	return;
}
int main()
{
	int n,i,j,k,fj,fk,len,sum = 0;
	scanf("%d",&n);
	rank = (int*)malloc(n*sizeof(int));
	father = (int*)malloc(n*sizeof(int));
	for(i=0;i<n;i++){
		make_set(i);
	}
	scanf("%d%d%d",&len,&j,&k);
	while(!(len==0&&j==0&&k==0)){
		if((len != 1 && len != 0) || j > n || k > n){
#ifdef DBGMSG
			printf("invalid input!\n");
#endif
			sum++;
			goto cont;
		}
		fj = get_father(j-1);
		fk = get_father(k-1);
		if(fj == fk && (rank[j-1] - rank[k-1]) != len){
#ifdef DBGMSG
			printf("logic error!\n");
#endif
			sum++;
			goto cont;
		}
		Union(len,j-1,k-1);
cont:
		scanf("%d%d%d",&len,&j,&k);
	}
	printf("sum %d\n",sum);
	free(father);
	free(rank);
	return 0;
}
