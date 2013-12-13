#include"glob.h"
#define INPUT_FILE	"input.txt"
#define COLOR_WHITE	'W'
#define COLOR_GREY	'G'
#define COLOR_BLACK	'B'
#define V_NODE_N	20
#define IS_BETWEEN(i,x,y)	((i) >= (x) && (i) <= (y))
typedef struct _time_stamp{
	int d;
	int f;
}time_stamp;
typedef struct __vnode{
	int n;				/*node number*/
	char c;				/*color*/
	int d;				/*distance to root*/
	struct __vnode * p;	/*parent,used when create a breadth_first_tree*/
	struct __vnode * first_child;
	struct __vnode * next_sibling;
	time_stamp tstp;	/* for depth_first search */
}vnode;
typedef struct _adjac_node{
	vnode * v;
	struct _adjac_node * n;
}adjnode;
#define VNODE_SZ	sizeof(vnode)
#define ADJNODE_SZ	sizeof(adjnode)
static vnode * v;
static adjnode * adj;
static int v_node_n;
static void init_graph(void)
{
	int i;
	/* initialize the vnode */
	for(i=0;i<v_node_n;i++){
		v[i].n = i+1;
		v[i].c = COLOR_WHITE;
		v[i].d = 0;
		v[i].p = NULL;
		v[i].first_child = NULL;
		v[i].next_sibling = NULL;
	}
	/* initialize the adjnode */
	for(i=0;i<v_node_n;i++){
		adj[i].v = &v[i];
		adj[i].n = NULL;
	}
	return;
}
static adjnode * new_adjnode(vnode * p,adjnode * n)
{
	adjnode * adjn = (adjnode*)malloc(ADJNODE_SZ);
	if(adjn == NULL){
		return NULL;
	}
	adjn->v = p;
	adjn->n = n;
	return adjn;
}
static void creat_graph(void)
{
	int v1,v2;
	vnode * dst_v;
	adjnode * h,*n;
	printf("input number of v_node : ");
	scanf("%d",&v_node_n);
	if(v_node_n < 0){
		fprintf(stderr,"invalid number of node!\n");
		return;
	}
	v = (vnode*)malloc(v_node_n*VNODE_SZ);
	adj = (adjnode*)malloc(v_node_n*ADJNODE_SZ);
	init_graph();
	printf("input Es with two endians:\n");
	scanf("%d%d",&v1,&v2);
	while(!(v1 == 0 && v2 == 0)){
		if(!IS_BETWEEN(v1,1,v_node_n) || !IS_BETWEEN(v2,1,v_node_n)){
			fprintf(stderr,"node number out of range!\n");
			continue;
		}
		h = &adj[v1-1];
		dst_v = &v[v2-1];
		n = new_adjnode(dst_v,h->n);
		h->n = n;
		scanf("%d%d",&v1,&v2);
	}
	return;
}
/*
static void destroy_adjlist(void)
{
}
static void transposition_graph(void)
{
	adjnode * new_adj = (adjnode*)malloc(v_node_n*ADJNODE_SZ);
	int i;
	for(i=0;i<v_node_n;i++){
		new_adj[i].v = &v[i];
		new_adj[i].n = NULL;
	}
}
*/
static void print_adjlist(void)
{
	adjnode * adjn;
	int i;
	for(i=0;i<v_node_n;i++){
		adjn = &adj[i];
		printf("%d : ",adjn->v->n);
		adjn = adjn->n;
		while(adjn){
			printf("-- %d ",adjn->v->n);
			adjn = adjn->n;
		}
		printf("\n");
	}
	return;
}
static void destory_graph(void)
{
	adjnode * adjn,*n;
	int i;
	for(i=0;i<v_node_n;i++){
		adjn = adj[i].n;
		adj[i].n = NULL;
		while(adjn){
			n = adjn->n;
			free(adjn);
			adjn = n;
		}
	}
	free(adj);
	free(v);
	v_node_n = 0;
	return;
}
/********************* breadth first search **********************/
static vnode * root;
#define Q_LEN	1024
static void * bfs_q[Q_LEN] = {NULL};
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
	bfs_q[q_t] = ptr;
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
	n = bfs_q[q_h];
	q_h = CIRCULAR_NEXT(q_h);
	return n;
}
static void breadth_first_search(void)
{
	int iu,iv,s;
	adjnode * adjn;
	void * vadjn;
	printf("input start node number (1~%d) : ",v_node_n);
	scanf("%d",&s);
	printf("\n");
	iv = s;
	adjn = &adj[iv-1];
	v[iv-1].c = COLOR_GREY;
	v[iv-1].d = 0;
	v[iv-1].p = NULL;
	root = &v[iv-1];
	init_q();
	en_q((void*)adjn);
	while(!Q_EMPTY){
		vadjn = de_q();
		adjn = (adjnode*)vadjn;
		iu = adjn->v->n;
		for(adjn = adj[iu-1].n;adjn;adjn = adjn->n){
			if(adjn->v->c == COLOR_WHITE){
				iv = adjn->v->n;
				if(v[iu-1].first_child != NULL){
					v[iv-1].next_sibling = v[iu-1].first_child;
				}
				v[iu-1].first_child = &v[iv-1];
				v[iv-1].c = COLOR_GREY;
				v[iv-1].p = &v[iu-1];
				v[iv-1].d = v[iu-1].d + 1;
				en_q((void*)adjn);
			}
		}
		v[iu-1].c = COLOR_BLACK;
	}
	return;
}
static void print_bfst(void)
{
	void * t;
	vnode * vn = root;
	init_q();
	en_q((void*)vn);
	while(!Q_EMPTY){
		t = de_q();
		vn = (vnode*)t;
		printf("%d%c%d\n",vn->n,vn->c,vn->d);
		vn = vn->first_child;
		if(vn == NULL){
			continue;
		}
		while(vn){
			en_q((void*)vn);
			printf("%d%c%d - ",vn->n,vn->c,vn->d);
			vn = vn->next_sibling;
		}
		printf("\n");
	}
	return;
}
/********************* depth first search ***********************/
/* useful fields in vnode for depth_first search:
 * 1) timestamp
 * 2) parent
 * 3) color */
static int timestamp;
static void dfs_visit(vnode * u)
{
	adjnode * adjn;
	u->tstp.d = (timestamp += 1);
	u->c = COLOR_GREY;
	for(adjn=adj[u->n-1].n;adjn!=NULL;adjn=adjn->n){
		if(adjn->v->c == COLOR_WHITE){
			adjn->v->p = u;
			dfs_visit(adjn->v);
		}
	}
	u->c = COLOR_BLACK;
	u->tstp.f = (++timestamp);
	return;
}
static void depth_first_search(void)
{
	int i;
	for(i=0;i<v_node_n;i++){
		v[i].c = COLOR_WHITE;
		v[i].p = NULL;
	}
	timestamp = 0;
	for(i=0;i<v_node_n;i++){
		if(v[i].c == COLOR_WHITE){
			dfs_visit(&v[i]);
		}
	}
	return;
}
static void print_v(void)
{
	int i;
	for(i=0;i<v_node_n;i++){
		printf("node #%d : c #%c tstp.d/v #%2d:%2d ",v[i].n,v[i].c,v[i].tstp.d,v[i].tstp.f);
		if(v[i].p){
			printf("p #%d",v[i].p->n);
		}
		printf("\n");
	}
	return;
}
int main(int argc,char *argv[])
{
	char * input_file;
	if(argc != 2){
		fprintf(stderr,"E_ARG\n");
		exit(1);
	}
	input_file = argv[1];
	int fd = open(input_file,O_RDONLY);
	int stdin_dup = dup(STDIN_FILENO);
	dup2(fd,STDIN_FILENO);
	creat_graph();
	print_adjlist();
//	breadth_first_search();
//	print_bfst();
	depth_first_search();
	print_v();
	destory_graph();
	dup2(stdin_dup,STDIN_FILENO);
	close(fd);
	return;
}
