#include"glob.h"
#define ELEM_N	128
#define K	30
#define DBGMSG
static int a[ELEM_N];
static void rand_generator(int a[],int len)
{
	int i;
	for(i = 0;i < len;i++){
		srand(time(NULL) + i);
		a[i] = rand()%1000;
	}
	return;
}
static void swap(int a[],int i,int j)
{
	int tmp;
	if(i == j)
		return;
	tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
	return;
}
static int kpartition(int a[],int len)
{
	int p = a[len-1];
	int index_of_last_num_lt_pivot = -1;
	int i;
	for(i = 0;i<len;i++){
		if(a[i] < p){
			swap(a,++index_of_last_num_lt_pivot,i);
		}
	}
	swap(a,++index_of_last_num_lt_pivot,len-1);
	return index_of_last_num_lt_pivot;
}
static void qqsort(int a[],int len)
{
	int kp;
	if(len <= 0){
		return;
	}
	kp = kpartition(a,len);
	qqsort(a,kp);
	qqsort(a+kp+1,len-kp-1);
	return;

}
static void find_k_big(int a[],int len,int k,int dst[],int * index)
{
	int kp,llen,rlen,i;
	if(k <= 0){
		return;
	}
	if(k>=len){
		for(i = 0;i<len;i++){
			dst[(*index)++] = a[i];
		}
		return;
	}
	kp = kpartition(a,len);
	llen = kp;
	rlen = len - kp - 1;
#ifdef DBGMSG
	printf("-----------start----------\n");
	printf("partition\t#%d\n",kp);
	printf("llen\t#%d\n",llen);
	printf("rlen\t#%d\n",rlen);
	printf("k\t#%d\n",k);
	printf("dst_indx\t%d\n",*index);
	printf("-----------end----------\n");
#endif
	if(rlen == k){
#ifdef DBGMSG
		printf("k == rlen\n");
#endif
		for(i = 0;i < rlen;i++){
			dst[(*index)++] = a[kp + 1 + i];
		}
		return;
	}
	if(rlen < k){
#ifdef DBGMSG
		printf("k > rlen\n");
#endif
		for(i = 0;i <= rlen;i++){
			dst[(*index)++] = a[kp + i];
		}
		find_k_big(a,llen,k-rlen-1,dst,index);
	}else if(rlen > k){
#ifdef DBGMSG
		printf("k < rlen\n");
#endif
		find_k_big(a+kp+1,rlen,k,dst,index);
	}
}
int main()
{
	int index = 0,kbig[K],i;
	rand_generator(a,ELEM_N);
	for(i = 0;i<ELEM_N;i++){
		printf("%d ",a[i]);
	}
	printf("\n----------------------------------------------------------------------\n");
	find_k_big(a,ELEM_N,K,kbig,&index);
	for(i = 0;i < index;i++){
		printf("%d ",kbig[i]);
	}
	printf("\n");
	printf("\n----------------------------------------------------------------------\n");
	qqsort(a,ELEM_N);
	for(i = 0;i<ELEM_N;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
