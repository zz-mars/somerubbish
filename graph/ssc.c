#include"glob.h"
#define COLOR_WHITE	'W'
#define COLOR_GREY	'G'
#define COLOR_BLACK	'B'
#define V_NODE_N	20
#define IS_BETWEEN(i,x,y)	((i) >= (x) && (i) <= (y))
/**/
typedef struct _scc_node{
	struct _scc_node * fc;
	struct _scc_node * ns;
	struct _scc_node * p;
}sccn;
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
	sccn * sccp;
}vnode;
typedef struct _adjac_node{
	vnode * v;
	struct _adjac_node * n;
}adjnode;
#define VNODE_SZ	sizeof(vnode)
#define ADJNODE_SZ	sizeof(adjnode)
static void init_graph(vnode * v,adjnode * adj,int elem_n)
{
	int i;
	/* initialize the vnode */
	for(i=0;i<elem_n;i++){
		v[i].n = i+1;
		v[i].c = COLOR_WHITE;
		v[i].d = 0;
		v[i].p = NULL;
		v[i].first_child = NULL;
		v[i].next_sibling = NULL;
		v[i].sccp = NULL;
	}
	/* initialize the adjnode */
	for(i=0;i<elem_n;i++){
		adj[i].v = &v[i];
		adj[i].n = NULL;
	}
	return;
}
static void add_to_adj_list(adjnode * adj,vnode * v,int i,int j)
{
	/* add new edge ' i --> j ' */
	adjnode * n = (adjnode*)malloc(ADJNODE_SZ);
	n->v = &v[j-1];
	n->n = adj[i-1].n;
	adj[i-1].n = n;
	return;
}
static char trans_buf[BUFSIZ];
static int scanf_flag = 0;
static adjnode * creat_graph(vnode * v,int v_node_n)
{
	int v1,v2;
	char * p = trans_buf;
	adjnode * adj = (adjnode*)malloc(ADJNODE_SZ*v_node_n);
	init_graph(v,adj,v_node_n);
	if(scanf_flag == 0){
		scanf("%d%d",&v1,&v2);
	}else if(scanf_flag == 1){
		sscanf(p,"%d%d",&v1,&v2);
	}
	while(!(v1 == 0 && v2 == 0)){
		if(!IS_BETWEEN(v1,1,v_node_n) || !IS_BETWEEN(v2,1,v_node_n)){
			fprintf(stderr,"node number out of range!\n");
			continue;
		}
		add_to_adj_list(adj,v,v1,v2);
		if(scanf_flag == 0){
			scanf("%d%d",&v1,&v2);
		}else if(scanf_flag == 1){
			while(*p != '\n'){p++;}
			p++;
			sscanf(p,"%d%d",&v1,&v2);
		}
	}
	return adj;
}
static adjnode * transposition_graph(vnode * v,adjnode * adj,int v_node_n)
{
	adjnode * tadj;
	int i,j,off = 0;
	scanf_flag = 1;
	bzero(trans_buf,BUFSIZ);
	for(i=1;i<=v_node_n;i++){
		tadj = adj[i-1].n;
		while(tadj){
			j = tadj->v->n;
			off = strlen(trans_buf);
			snprintf(trans_buf+off,BUFSIZ-off,"%d %d\n",j,i);
			tadj = tadj->n;
		}
	}
	off = strlen(trans_buf);
	snprintf(trans_buf+off,BUFSIZ-off,"%d %d\n",0,0);
	tadj = creat_graph(v,v_node_n);
	scanf_flag = 0;
	return tadj;
}
static void print_adjlist(adjnode * adj,int v_node_n)
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
static void destory_adjlist(adjnode * adj,int v_node_n)
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
	return;
}
static void destory_vnode(vnode * v)
{
	return free(v);
}
/********************* depth first search ***********************/
/* useful fields in vnode for depth_first search:
 * 1) timestamp
 * 2) parent
 * 3) color */
static int timestamp;
static void dfs_visit(vnode * u,adjnode * adj)
{
	adjnode * adjn;
	u->tstp.d = (timestamp += 1);
	u->c = COLOR_GREY;
	for(adjn=adj[u->n-1].n;adjn!=NULL;adjn=adjn->n){
		if(adjn->v->c == COLOR_WHITE){
			adjn->v->p = u;
			dfs_visit(adjn->v,adj);
		}
	}
	u->c = COLOR_BLACK;
	u->tstp.f = (++timestamp);
	return;
}
static void depth_first_search(vnode * v,adjnode * adj,int v_node_n,int dfs_order[],int start)
{
	int i,j;
	for(i=0;i<v_node_n;i++){
		v[i].c = COLOR_WHITE;
		v[i].p = NULL;
	}
	timestamp = 0;
	for(i=0;i<v_node_n;i++){
		j = dfs_order[(i+start)%v_node_n];
		if(v[j].c == COLOR_WHITE){
			dfs_visit(&v[j],adj);
		}
	}
	return;
}
static void print_v(vnode * v,int v_node_n)
{
	int i;
	for(i=0;i<v_node_n;i++){
		printf("node #%d : c #%c tstp.d/f #%2d:%2d ",v[i].n,v[i].c,v[i].tstp.d,v[i].tstp.f);
		if(v[i].p){
			printf("p #%d",v[i].p->n);
		}
		printf("\n");
	}
	return;
}
static void zswap(int a[],int i,int j)
{
	int tm;
	if(i==j){
		return;
	}
	tm = a[i];
	a[i] = a[j];
	a[j] = tm;
	return;
}
int main(int argc,char *argv[])
{
	char * input_file;
	int v_node_n,fd,stdin_dup,start;
	adjnode * adj,* tadj;
	vnode * v;
	int *dfs_order,i = 0,j;
	if(argc != 2){
		fprintf(stderr,"E_ARG\n");
		exit(1);
	}
	input_file = argv[1];
	fd = open(input_file,O_RDONLY);
	stdin_dup = dup(STDIN_FILENO);
	dup2(fd,STDIN_FILENO);
	printf("input number of v_node : ");
	scanf("%d",&v_node_n);
	if(v_node_n < 0){
		fprintf(stderr,"invalid number of node!\n");
		return;
	}
	printf("%d\n",v_node_n);
	dfs_order = (int*)malloc(v_node_n*sizeof(int));
	while(i<v_node_n){dfs_order[i++] = i - 1;}
	v = (vnode*)malloc(v_node_n*VNODE_SZ);
	adj = creat_graph(v,v_node_n);
	dup2(stdin_dup,STDIN_FILENO);
	close(fd);
	print_adjlist(adj,v_node_n);
	printf("dfs_order : \n");
	start = 3;
	for(i=0;i<v_node_n;i++){
		printf("%d ",dfs_order[(i+start)%v_node_n]+1);
	}
	printf("\n");
	depth_first_search(v,adj,v_node_n,dfs_order,start);
	print_v(v,v_node_n);
	/* reset dfs_order */
	for(i=0;i<v_node_n;i++){
		for(j=v_node_n-1;j>i;j--){
			if(v[j].tstp.f > v[j-1].tstp.f){
				zswap(dfs_order,j,j-1);
			}
		}
	}
	tadj = transposition_graph(v,adj,v_node_n);
	print_adjlist(tadj,v_node_n);
	printf("new dfs_order : \n");
	start = 0;
	for(i=0;i<v_node_n;i++){
		printf("%d ",dfs_order[(i+start)%v_node_n]+1);
	}
	printf("\n");
	depth_first_search(v,tadj,v_node_n,dfs_order,start);
	print_v(v,v_node_n);
	destory_adjlist(adj,v_node_n);
	destory_adjlist(tadj,v_node_n);
	destory_vnode(v);
	return;
}
