#include"glob.h"
#define ELEM_N	256
#define HASH_N	10
static int rank[ELEM_N];;
static int father[ELEM_N];
static int a[ELEM_N];
static int count[ELEM_N] = {0};
static int index_of_longest;
typedef struct key_index{
	int key;
	int index;
	struct key_index * hash_p;
	struct key_index * hash_n;
}ki;
static ki kh[ELEM_N];
static ki * hash_table[HASH_N] = {NULL};
#define HASH(key)	((key)%HASH_N)
static void init_hash_table(int a[],ki kh[])
{
	int i,hash_v;
	ki * k,*p,*n;
	for(i=0;i<HASH_N;i++){
		hash_table[i] = NULL;
	}
	for(i=0;i<ELEM_N;i++){
		kh[i].key = a[i];
		kh[i].index = i;
		kh[i].hash_p = NULL;
		kh[i].hash_n = NULL;
		hash_v = HASH(kh[i].key);
		if(hash_table[hash_v] == NULL){
			hash_table[hash_v] = &kh[i];
			continue;
		}
		/*find the place to insert*/
		k = hash_table[hash_v];
		while(k!=NULL){
			p = k;
			n = k->hash_n;
			if(k->key<kh[i].key){
				k=n;
				continue;
			}
			/*insert to the place before k*/
			break;
		}
		if(k == NULL){
			/*put to tail*/
			p->hash_n = &kh[i];
			kh[i].hash_p = p;
			continue;
		}
		p = k->hash_p;
		if(p == NULL){
			/*put to head*/
			hash_table[hash_v] = &kh[i];
			kh[i].hash_n = k;
			k->hash_p = &kh[i];
			continue;
		}
		p->hash_n = &kh[i];
		kh[i].hash_p = p;
		kh[i].hash_n = k;
		k->hash_p = &kh[i];
	}
	/*print hash table*/
	for(i=0;i<HASH_N;i++){
		for(k=hash_table[i];k!=NULL;k=k->hash_n){
			printf("index #%3d key #%3d\n",k->index,k->key);
		}
		printf("------------------- hash_list %d over -----------------\n",i);
	}
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
static ki * find_in_hash_table(int key)
{
	int hash_v = HASH(key);
	ki * k;
	for(k = hash_table[hash_v];k!=NULL;k=k->hash_n){
		if(k->key == key){
			break;
		}
	}
	return k;
}
static void rand_generator(int a[],int len)
{
	int i;
	for(i = 0;i < len;i++){
		srand(time(NULL) + i);
		a[i] = rand()%1000;
	}
	return;
}
static void init_find_set(void)
{
	int i;
	for(i=0;i<ELEM_N;i++){
		make_set(i);
	}
	return;
}
static void exchange(int a[],int i,int j)
{
	if(i == j){
		return;
	}
	int tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
	return;
}
static int partionqs(int a[],int b,int e)
{
	int i;
	int r = a[e];
	int p = b - 1;
	for(i = b;i < e;i++){
		if(a[i] <= r){
			p++;
			exchange(a,p,i);
		}
	}
	p++;
	exchange(a,p,e);
	return p;
}
static void quicksortz(int a[],int b,int e)
{
	int i;
	if(b < e){
		i = partionqs(a,b,e);
		quicksortz(a,b,i-1);
		quicksortz(a,i+1,e);
	}
	return;
}
int main()
{
	int ip1,im1,i,j;
	ki * k;
	rand_generator(a,ELEM_N);
	init_find_set();
	init_hash_table(a,kh);
	for(i=0;i<ELEM_N;i++){
		if((k = find_in_hash_table(a[i] - 1)) != NULL){
			ip1 = k->index;
			Union(i,ip1);
		}
		if((k = find_in_hash_table(a[i] + 1)) != NULL){
			im1 = k->index;
			Union(i,im1);
		}
	}
	j = 0;
	for(i=0;i<ELEM_N;i++){
		count[father[i]]++;
		if(j<count[father[i]]){
			j = count[father[i]];
			index_of_longest = father[i];
		}
	}
	printf("length of the longest serial is %d\n",j);
	printf("index of father is a[%d] -- %d\n",index_of_longest,a[index_of_longest]);
	for(i=0;i<ELEM_N;i++){
		if(father[i] == index_of_longest){
			printf("%d ",a[i]);
		}
	}
	printf("\n");
	quicksortz(a,0,ELEM_N-1);
	printf("after sort-------------------------------------\n");
	for(i=0;i<ELEM_N;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
