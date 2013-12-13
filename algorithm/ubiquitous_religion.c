#include"glob.h"
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
	if(father[x] != x){
		return get_father(father[x]);
	}
	return x;
}
static void Union(int x,int y)
{
	int fx = get_father(x);
	int fy = get_father(y);
	if(fx == fy)
		return;
	if(rank[fx] > rank[fy]){
		father[fy] = fx;
	}else{
		father[fx] = fy;
		if(rank[fx] == rank[fy]){
			rank[fy]++;
		}
	}
	return;
}
int main()
{
	int n,m,c=0,i,j,k;
	scanf("%d%d",&n,&m);
	while(!(n == 0 && m == 0)){
		rank = (int*)malloc(n*sizeof(int));
		father = (int*)malloc(n*sizeof(int));
		for(i=0;i<n;i++){
			make_set(i);
		}
		for(i=0;i<m;i++){
			scanf("%d%d",&j,&k);
			if(j > n || k > n){
				fprintf(stderr,"invalid\n");
				exit(1);
			}
			Union(j-1,k-1);
		}
		j=0;
		for(i=0;i<n;i++){
			if(father[i] == i){
				j++;
			}
		}
		printf("case%d: %d\n",c++,j);
		free(rank);
		free(father);
		scanf("%d%d",&n,&m);
	}
	return 0;
}
