#include"glob.h"
#define DBGMSG
static int * rank;
static int * father;
static int * ndis;
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
	ndis[x] = 0;
	return;
}
static int get_father(int x)
{
	int prev,ndisx = ndis[x];
#ifdef DBGMSG
	printf("current father of %d is %d\n",x+1,father[x]+1);
#endif
	if(father[x] != x){
		prev = father[x];
		father[x] = get_father(father[x]);
		ndis[x] = (ndisx + ndis[prev])%2;
#ifdef DBGMSG
		printf("new father of %d is %d\n",x+1,father[x]+1);
		printf("adjust ndis : ndis[%d] = (ndis[%d] + ndis[%d])%%2 = (%d + %d)%%2 = %d\n",x+1,x+1,prev+1,ndisx,ndis[prev],ndis[x]);
#endif
	}
	return father[x];
}
static void Union(int x,int y)
{
	int fx = get_father(x);
	int fy = get_father(y);
	int ndisx = ndis[x];
	int ndisy = ndis[y];
#ifdef DBGMSG
	printf("union x : %d y : %d fx : %d fy : %d\n",x+1,y+1,fx+1,fy+1);
#endif
	if(fx == fy){
#ifdef DBGMSG
		printf("fx == fy!return\n");
#endif
		return;
	}
	if(rank[fx] > rank[fy]){
		father[fy] = fx;
		ndis[fy] = (ndisx + ndisy + 1)%2;
#ifdef DBGMSG
		printf("father[%d] == %d\n",fy+1,fx+1);
		printf("ndis[%d] = (ndis[%d] + ndis[%d] + 1)%2 = (%d + %d +1)%%2 = %d\n",fy+1,x+1,y+1,ndisx,ndisy,ndis[fy]);
#endif
	}else{
		father[fx] = fy;
		if(rank[fx] == rank[fy]){
			rank[fy]++;
		}
		ndis[fx] = (ndisx + ndisy + 1)%2;
#ifdef DBGMSG
		printf("father[%d] == %d\n",fx+1,fy+1);
		printf("ndis[%d] = (ndis[%d] + ndis[%d] + 1)%2 = (%d + %d +1)%%2 = %d\n",fx+1,x+1,y+1,ndisx,ndisy,ndis[fx]);
#endif
	}
	return;
}
int main()
{
	int t,n,m,c=0,i,j,k,fj,fk;
	char cc;
	scanf("%d",&t);
	scanf("%d%d",&n,&m);
	while(c<t){
#ifdef DBGMSG
		printf("n -- %d\nm -- %d\n",n,m);
#endif
		rank = (int*)malloc(n*sizeof(int));
		father = (int*)malloc(n*sizeof(int));
		ndis = (int*)malloc(n*sizeof(int));
		for(i=0;i<n;i++){
			make_set(i);
		}
		for(i=0;i<m;i++){
			getchar();
			scanf("%c%d%d",&cc,&j,&k);
#ifdef DBGMSG
			printf("#%d : cc -- %c %d\nj -- %d\nk -- %d\n",i,cc,cc,j,k);
#endif
			if((cc != 'A' && cc != 'D') || j > n || k > n){
				fprintf(stderr,"invalid input\n");
				exit(1);
			}
			if(cc == 'A'){
				fj = get_father(j-1);
				fk = get_father(k-1);
				if(fj == fk){
					if(ndis[j-1] == ndis[k-1]){
						printf("In the same gang.\n");
					}else{
						printf("In different gangs.\n");
					}
				}else{
					printf("Not sure yet.\n");
				}
			}else if(cc == 'D'){
				Union(j-1,k-1);
			}
		}
		free(rank);
		free(father);
		free(ndis);
		if(c == (t-1)){
			break;
		}
		scanf("%d%d",&n,&m);
	}
	return 0;
}
