#include"glob.h"
#define DBGMSG
static void init_partial_match_table(char * to_find,int pmt[],int tflen)
{
	int i,j;
	char *s = to_find,*e;
	for(i=1;i<tflen;i++){
		pmt[i] = 0;
		for(j=1;j<=i;j++){
			e = s + i - j + 1;
			if(strncmp(s,e,j) == 0 && j > pmt[i]){
				pmt[i] = j;
			}
		}
	}
	pmt[0] = 0;
	return;
}
static int find_pattern(char * src,int len,char * pattern,int tflen,int pmt[])
{
	int tfindex = 0,index = 0;
	int remaining_bytes;
	char a,b;
	while(tfindex < tflen){
#ifdef DBGMSG
		printf("-------------start------------\n");
#endif
		if((tflen - tfindex) > (len - index)){
			return 1;
		}
		a = *(src + index);
		b = *(pattern + tfindex);
#ifdef DBGMSG
		printf("index = #%d\n",index);
		printf("tfindex = #%d\n",tfindex);
		printf("now compare : %c & %c\n",a,b);
#endif
		if(a == b){
			index++;
			tfindex++;
		}else if(tfindex == 0){
			index++;
		}else{
#ifdef DBGMSG
//			printf("index -= %d\n",pmt[tfindex-1]);
			printf("tfindex = %d\n",pmt[tfindex-1]);
#endif
//			index -= pmt[tfindex-1];
			tfindex = pmt[tfindex-1];
		}
#ifdef DBGMSG
		printf("--------------end-------------\n");
#endif
	}
	if(tfindex == tflen){
		return 0;
	}
	return 1;
}
int main(int argc,char * argv[])
{
	int i;
	int * pmt;
	char * src,*to_find;
	int slen,tflen;
	if(argc != 3){
		fprintf(stderr,"invalid argument!\n");
		return 1;
	}
	to_find = argv[1];
	src = argv[2];
	slen = strlen(src);
	tflen = strlen(to_find);
	pmt = (int *)malloc(tflen*sizeof(int));
	init_partial_match_table(to_find,pmt,tflen);
	for(i=0;i<tflen;i++){
		printf("%d ",pmt[i]);
	}
	printf("\n");
	if(find_pattern(src,slen,to_find,tflen,pmt) == 0){
		printf("pattern found!\n");
	}else{
		printf("pattern not found!\n");
	}
	return 0;
}
