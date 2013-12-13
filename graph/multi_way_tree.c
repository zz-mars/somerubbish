#include"glob.h"
typedef struct _node{
	int id;				/* node id */
	int noc;			/* number of children */
	struct _node * fc;	/* first child */
	struct _node * lc;	/* last child */
	struct _node * ns;	/* next sibling */
	struct _node * p;	/* parent */
}node;
#define Q_LEN	1024
static void * qArray[Q_LEN] = {NULL};
static int q_h = 0;
static int q_t = 0;
/* queue is EMPTY when q_h == q_t 
 * queue is FULL when q_h == (q_t + 1)%Q_LEN */
#define Q_EMPTY	(q_h == q_t)
#define Q_FULL	(q_h == (q_t + 1)%Q_LEN)
#define CIRCULAR_NEXT(i)	(((i) + 1)%Q_LEN)
static inline void init_q(void)
{
	q_h = 0;
	q_t = 0;
	return;
}
static int en_q(void  * ptr)
{
	if(Q_FULL){
		fprintf(stderr,"Q is FULL\n");
		return 1;
	}
	qArray[q_t] = ptr;
	q_t = CIRCULAR_NEXT(q_t);
	return 0;
}
static void * de_q(void)
{
	void * n;
	if(Q_EMPTY){
		fprintf(stderr,"Q is EMPTY\n");
		return NULL;
	}
	n = qArray[q_h];
	q_h = CIRCULAR_NEXT(q_h);
	return n;
}
static void printMultiwayTree(node * root)
{
	void * t;
	node * vn = root;
	init_q();
	en_q((void*)vn);
	while(!Q_EMPTY){
		t = de_q();
		vn = (node*)t;
		printf("%d\n",vn->id);
		vn = vn->fc;
		if(vn == NULL){
			continue;
		}
		while(vn){
			en_q((void*)vn);
			printf("%d - ",vn->id);
			vn = vn->ns;
		}
		printf("\n------------------\n");
	}
	return;
}
#define NODE_SZ	sizeof(node)
#define LINKED(n)	((n)->noc != 0 || (n)->p != NULL)
#define NO_CHILD(n)	((n)->noc == 0 && (n)->fc == NULL && (n)->lc == NULL)
static int zmax(int i,int j)
{
	return (i>=j?i:j);
}
static void init_node(node * n,int len)
{
	int i;
	for(i=0;i<len;i++){
		n[i].id = i;
		n[i].noc = 0;
		n[i].fc = NULL;
		n[i].lc = NULL;
		n[i].ns = NULL;
		n[i].p = NULL;
	}
	return;
}
static void zlink(node * node_array,int parent,int child)
{
	node * p = &node_array[parent];
	node * c = &node_array[child];
	if(LINKED(c)){
		fprintf(stderr,"child node already linked!\n");
		return;
	}
	if(NO_CHILD(p)){
		p->fc = c;
	}else{
		p->lc->ns = c;
	}
	p->lc = c;
	p->noc++;
	return;
}
static void find_big2(int a[],int len,int * biggest,int * second_biggest)
{
	int i,b1 = 0,b2 = 0;
	for(i=0;i<len;i++){
		if(a[i]>b1){
			b2=b1;
			b1=a[i];
		}else if(a[i]>b2){
			b2 = a[i];
		}
	}
	*biggest = b1;
	if(second_biggest){
		*second_biggest = b2;
	}
	return;
}
static void get_max_distance(node * root,int * max_dis,int * max_dep)
{
	int i,max_child_dep,sec_max_child_dep,max_child_dis,noc = root->noc;
	int * child_dep,*child_dis;
	node * child;
	if(noc == 0){
		*max_dep = 0;
		*max_dis = 0;
		return;
	}
	child_dep = (int*)malloc(noc*sizeof(int));
	child_dis = (int*)malloc(noc*sizeof(int));
	for(i=0,child=root->fc;i<noc&&child!=NULL;i++,child=child->ns){
		get_max_distance(child,&child_dis[i],&child_dep[i]);
	}
	if(noc == 1){
		max_child_dep = child_dep[0];
		sec_max_child_dep = -1;
		max_child_dis = child_dis[0];
	}else{
		find_big2(child_dis,noc,&max_child_dis,NULL);
		find_big2(child_dep,noc,&max_child_dep,&sec_max_child_dep);
	}
	*max_dep = max_child_dep + 1;
	*max_dis = zmax(max_child_dep+sec_max_child_dep+2,max_child_dis);
	free(child_dep);
	free(child_dis);
	return;
}
int main(int argc,char *argv[])
{
	int max_distance,max_depth;
	int node_n,p,c;
	node * t1;
//	node t[9];
//	init_node(t,9);
//	zlink(t,0,1);
//	zlink(t,0,2);
//	zlink(t,1,3);
//	zlink(t,1,4);
//	zlink(t,2,5);
//	zlink(t,2,6);
//	zlink(t,3,7);
//	zlink(t,5,8);
////	printMultiwayTree(t);
//	get_max_distance(&t[0],&max_distance,&max_depth);
//	printf("test case 1 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
//
//	init_node(t,9);
//	zlink(t, 0, 1);
//	zlink(t, 0, 2);
//	zlink(t, 1, 3);
////	printMultiwayTree(t);
//	get_max_distance(&t[0],&max_distance,&max_depth);
//	printf("test case 2 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
//
//	init_node(t,9);
//	zlink(t, 0, 1);
//	zlink(t, 1, 2);
//	zlink(t, 1, 3);
//	zlink(t, 2, 4);
//	zlink(t, 3, 5);
//	zlink(t, 3, 6);
//	zlink(t, 4, 7);
//	zlink(t, 5, 8);
////	printMultiwayTree(t);
//	get_max_distance(&t[0],&max_distance,&max_depth);
//	printf("test case 3 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
//
//	init_node(t,9);
//	zlink(t, 0, 1);
//	zlink(t, 0, 2);
//	zlink(t, 1, 3);
//	zlink(t, 1, 4);
//	zlink(t, 3, 5);
//	zlink(t, 3, 6);
//	zlink(t, 5, 7);
//	zlink(t, 6, 8);
////	printMultiwayTree(t);
//	get_max_distance(&t[0],&max_distance,&max_depth);
//	printf("test case 4 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
	if(argc != 2){
		fprintf(stderr,"E_ARG\n");
		exit(1);
	}
	int fd = open(argv[1],O_RDONLY);
	int stdin_dup = dup(STDIN_FILENO);
	dup2(fd,STDIN_FILENO);
	printf("input node_n \n");
	scanf("%d",&node_n);
	t1 = (node*)malloc(node_n*NODE_SZ);
	init_node(t1,node_n);
	scanf("%d%d",&p,&c);
#define IS_BETWEEN(i,x,y)	((i) >= (x) && (i) <= (y))
	while(!(p == 0 && c == 0)){
		if(!IS_BETWEEN(p,0,node_n-1) || !IS_BETWEEN(c,0,node_n-1)){
			continue;
		}
		zlink(t1,p,c);
		scanf("%d%d",&p,&c);
	}
	printMultiwayTree(t1);
	get_max_distance(&t1[0],&max_distance,&max_depth);
	printf("test case 5 : max_distance #%d max_depth #%d\n",max_distance,max_depth);
	dup2(stdin_dup,STDIN_FILENO);
	close(fd);
	return 0;
}
