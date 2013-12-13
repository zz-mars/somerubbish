#include"glob.h"
#define ELEM_N	10
static int recurse_depth = 0;
static int p[ELEM_N] = {1,5,8,9,10,17,17,20,24,30};
static int r[ELEM_N];
/* record the first piece to cut */
static int s[ELEM_N];
static void initr(void)
{
	int i;
	for(i=0;i<ELEM_N;i++){
		r[i] = -1;
	}
	return;
}
static inline int zmax(int a,int b)
{
	return (a>=b?a:b);
}
static void cut_rod(int p[],int r[],int s[],int len)
{
	int i,j;
	recurse_depth++;
	for(j=1;j<=len;j++){
		/* for i == 0 */
//		r[j-1] = zmax(r[j-1],p[j-1]);
		if(r[j-1] < p[j-1]){
			r[j-1] = p[j-1];
			s[j-1] = j;
		}
		for(i=1;i<j;i++){
//			r[j-1] = zmax(r[j-1],p[j-i-1]+r[i-1]);
			if(r[j-1] < (p[i-1] + r[j-i-1])){
				r[j-1] = (p[i-1] + r[j-i-1]);
				s[j-1] = i;
			}
		}
	}
	return;
}
int main()
{
	int i,pi;
	initr();
	cut_rod(p,r,s,ELEM_N);
	printf("cut_rod is called #%d times!\n",recurse_depth);
	for(i=0;i<ELEM_N;i++){
		printf("%2d ",r[i]);
	}
	printf("\n");
	for(i=0;i<ELEM_N;i++){
		printf("%2d ",s[i]);
	}
	printf("\n");
	return 0;
}
