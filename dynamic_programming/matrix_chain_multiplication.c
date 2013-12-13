#include"glob.h"
//#define DBGMSG
#define ZMAX_INT	0x7fffffff
#define MATRIX_N	6
#define MS_MATRIX_L	(MATRIX_N*MATRIX_N)
static int ms_matrix[MS_MATRIX_L];
static int p[MATRIX_N+1] = {30,35,15,5,10,20,25};
#define INDEX_OF(i,j)	(((i)-1)*MATRIX_N+(j)-1)
#define ZSET(i,j,v)		ms_matrix[INDEX_OF(i,j)] = v
/*
#define MSET(i,j,v)		ms_matrix[INDEX_OF(i,j)] = v
#define SSET(i,j,v)		ms_matrix[INDEX_OF(j,i)] = v
*/
#define M(i,j)			ms_matrix[INDEX_OF(i,j)]
#define S(i,j)			ms_matrix[INDEX_OF(j,i)]
static void init_ms_matrix(void)
{
	int i,j;
	bzero(ms_matrix,sizeof(int)*MS_MATRIX_L);
	for(i=1;i<=MATRIX_N;i++){
		for(j=1;j<=MATRIX_N;j++){
			if(i<j){
				ZSET(i,j,ZMAX_INT);
			}
		}
	}
	return;
}
static void prt_ms_matrix(void)
{
	int i,j;
	for(i=1;i<=MATRIX_N;i++){
		for(j=1;j<=MATRIX_N;j++){
			printf("%3d ",ms_matrix[INDEX_OF(i,j)]);
		}
		printf("\n");
	}
	printf("--------------------\n");
	return;
}
static void prt_optimal_solution(int i,int j)
{
	int k;
	if(i == j){
		printf("a%d",i);
		return;
	}
	k = S(i,j);
	printf("(");
	prt_optimal_solution(i,k);
	prt_optimal_solution(k+1,j);
	printf(")");
	return;
}
static int recur_depth = 0;
static int multi_chain_matrix_recursive_style(int i,int j)
{
	int k,v;
	recur_depth++;
	if(i==j){
		M(i,j) = 0;
		return 0;
	}else if((i+1) == j){
		M(i,j) = p[i-1]*p[i]*p[j];
		S(i,j) = i;
		return M(i,j);
	}
	for(k=i;k<j;k++){
		if(M(i,k) == ZMAX_INT){
			multi_chain_matrix_recursive_style(i,k);
		}
		if(M(k+1,j) == ZMAX_INT){
			multi_chain_matrix_recursive_style(k+1,j);
		}
		v = M(i,k) + M(k+1,j) + p[i-1]*p[k]*p[j];
		if(v < M(i,j)){
			M(i,j) = v;
			S(i,j) = k;
		}
	}
	return M(i,j);
}
static void multi_chain_matrix(void)
{
	int i,j,k,v,l;
	for(l=2;l<=MATRIX_N;l++){
#ifdef DBGMSG
		printf("--------- length #%d ----------\n",l);
#endif
		for(i=1;i<=MATRIX_N-l+1;i++){
			j = i+l-1;
#ifdef DBGMSG
			printf("---- i#%d\tj#%d ----\n",i,j);
#endif
			for(k=i;k<j;k++){
#ifdef DBGMSG
				printf("k#%d\n",k);
#endif
				v = M(i,k)+M(k+1,j)+p[i-1]*p[k]*p[j];
#ifdef DBGMSG
				printf("M(%d,%d) #%d\n",i,j,M(i,j));
				printf("S(%d,%d) #%d\n",i,j,S(i,j));
				printf("v = M(i,k)+M(k+1,j)+p[i-1]*p[k]*p[j] == #%d\n",v);
#endif
				if(v < M(i,j)){
					M(i,j) = v;
					S(i,j) = k;
				}
#ifdef DBGMSG
				printf("new M(%d,%d) #%d\n",i,j,M(i,j));
				printf("new S(%d,%d) #%d\n",i,j,S(i,j));
#endif
			}
		}
	}
	return;
}
int main()
{
	init_ms_matrix();
	prt_ms_matrix();
	multi_chain_matrix();
	prt_ms_matrix();
	prt_optimal_solution(1,MATRIX_N);
	printf("\n");
	printf("------------------recursive style---------------\n");
	init_ms_matrix();
	prt_ms_matrix();
	multi_chain_matrix_recursive_style(1,MATRIX_N);
	printf("recur_depth #%d\n",recur_depth);
	prt_ms_matrix();
	prt_optimal_solution(1,MATRIX_N);
	printf("\n");
	return;
}
