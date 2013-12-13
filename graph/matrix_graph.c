#include"glob.h"
#define COLOR_WHITE	'W'
#define COLOR_GREY	'G'
#define COLOR_BLACK	'B'
#define BITS_ONE_BYTE	8
#define IS_BETWEEN(i,x,y)	((i) >= (x) && (i) <= (y))
typedef struct __vnode{
	int n;				/*node number*/
	char c;				/*color*/
	int d;				/*distance to root*/
	struct __vnode * p;	/*parent,used when create a breadth_first_tree*/
	struct __vnode * first_child;
	struct __vnode * last_child;
	struct __vnode * next_sibling;
}vnode;
#define VNODE_SZ	sizeof(vnode)
static vnode * v = NULL;
static char * graph_matrix_bitmap = NULL;
static int bytes_in_matrix_bitmap = 0;
static int v_node_n = 0;
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
		v[i].last_child = NULL;
		v[i].next_sibling = NULL;
	}
	bzero(graph_matrix_bitmap,bytes_in_matrix_bitmap);
	return;
}
static int set_e(int i,int j)
{
	/* set in the matrix for E i--j */
	int index,byte_no,off_in_byte;
	if(!IS_BETWEEN(i,1,v_node_n) || !IS_BETWEEN(j,1,v_node_n)){
		fprintf(stderr,"invalid node number!\n");
		return 1;
	}
	index = v_node_n * (i - 1) + j - 1;
	byte_no = index/BITS_ONE_BYTE;
	off_in_byte = index%BITS_ONE_BYTE;
	*(graph_matrix_bitmap + byte_no) |= (1<<off_in_byte);
	return 0;
}
static int get_e(int i,int j)
{
	int index,byte_no,off_in_byte;
	if(!IS_BETWEEN(i,1,v_node_n) || !IS_BETWEEN(j,1,v_node_n)){
		fprintf(stderr,"invalid node number!\n");
		return 1;
	}
	index = v_node_n * (i - 1) + j - 1;
	byte_no = index/BITS_ONE_BYTE;
	off_in_byte = index%BITS_ONE_BYTE;
	return ((*(graph_matrix_bitmap + byte_no) & (1<<off_in_byte)) == 0?0:1);
}
static void creat_graph(void)
{
	int v1,v2;
	vnode * dst_v;
	printf("input number of v_node : ");
	scanf("%d",&v_node_n);
	if(v_node_n < 0){
		fprintf(stderr,"invalid number of node!\n");
		return;
	}
	v = (vnode*)malloc(v_node_n*VNODE_SZ);
	bytes_in_matrix_bitmap = (v_node_n*v_node_n)/BITS_ONE_BYTE + 1;
	graph_matrix_bitmap = (char*)malloc(bytes_in_matrix_bitmap);
	init_graph();
	printf("input Es with two endians:\n");
	scanf("%d%d",&v1,&v2);
	while(!(v1 == 0 && v2 == 0)){
		if(!IS_BETWEEN(v1,1,v_node_n) || !IS_BETWEEN(v2,1,v_node_n)){
			fprintf(stderr,"node number out of range!\n");
			continue;
		}
		set_e(v1,v2);
		scanf("%d%d",&v1,&v2);
	}
	return;
}
static void print_graph_matrix(void)
{
	int i,j,index;
	printf("     ");
	for(i=1;i<=v_node_n;i++){
		printf("%d ",i);
	}
	printf("\n-------------------------\n");
	for(i=1;i<=v_node_n;i++){
		printf("%2d | ",i);
		for(j=1;j<=v_node_n;j++){
			index = get_e(i,j);
			printf("%d ",index);
		}
		printf("\n-------------------------\n");
	}
	return;
}
static void destory_graph(void)
{
	free(graph_matrix_bitmap);
	free(v);
	v_node_n = 0;
	bytes_in_matrix_bitmap = 0;
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
	int iv;
	vnode * uvn;
	void * dqv;
	printf("input start node number (1~%d) : ",v_node_n);
	scanf("%d",&iv);
	printf("\n");
	v[iv-1].c = COLOR_GREY;
	v[iv-1].d = 0;
	v[iv-1].p = NULL;
	root = &v[iv-1];
	init_q();
	en_q((void*)&v[iv-1]);
	while(!Q_EMPTY){
		dqv = de_q();
		uvn = (vnode*)dqv;
		for(iv=1;iv<=v_node_n;iv++){
			if(get_e(uvn->n,iv) == 1 && v[iv-1].c == COLOR_WHITE){
				v[iv-1].c = COLOR_GREY;
				v[iv-1].d = uvn->d + 1;
				v[iv-1].p = uvn;
				if(uvn->first_child == NULL && uvn->last_child == NULL){
					uvn->first_child = &v[iv-1];
				}else{
					uvn->last_child->next_sibling = &v[iv-1];
				}
				uvn->last_child = &v[iv-1];
				en_q((void*)&v[iv-1]);
			}
		}
		uvn->c = COLOR_BLACK;
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
	print_graph_matrix();
	breadth_first_search();
	print_bfst();
	destory_graph();
	dup2(stdin_dup,STDIN_FILENO);
	close(fd);
	return;
}
