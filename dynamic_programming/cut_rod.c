#include"glob.h"
#define ELEM_N	10
static int recurse_depth = 0;
static int p[ELEM_N] = {1,5,8,9,10,17,17,20,24,30};
static inline int zmax(int a,int b)
{
	return (a>=b?a:b);
}
static int cut_rod(int p[],int len)
{
	int i,pi = -1;
	recurse_depth++;
	if(len == 0){
		return 0;
	}
	for(i=1;i<=len;i++){
		pi = zmax(pi,p[i-1]+cut_rod(p,len-i));
	}
	return pi;
}
int main()
{
	int i,pi;
	cut_rod(p,10);
	//for(i=1;i<=10;i++){
	//	pi = cut_rod(p,i);
	//	printf("cut_rod[%d] = %d\n",i,pi);
	//}
	printf("cut_rod is called #%d times!\n",recurse_depth);
	return 0;
}
