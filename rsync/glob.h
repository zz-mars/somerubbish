/* file   : glob.h 
 * author : grant chen 
 * date   : Jan 6 2013
 * */
#ifndef _GLOB
#define _GLOB
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/sctp.h>
#include<stdarg.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<pthread.h>
#include<errno.h>
#include<signal.h>
#include<inttypes.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<time.h>
#include<stdbool.h>
#include<stdint.h>
#include<tcrdb.h>
#include<openssl/md5.h>
#include<mqueue.h>

#define LISTENQ			128
#define RSYNC_HOST_IP	"127.0.0.1"
#define RSYNC_CLN_PORT	13567
#define RSYNC_SRV_PORT	13568
#define ONE_K               (1<<10)
#define ONE_M               (1<<20)
#define ONE_G               (1<<30)
#define bzero(p,n)          memset((p),0,(n))
/* type definition */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef void Sigfunc(int);
typedef struct _list_head{
	struct _list_head * next;
	struct _list_head * pre;
}list_head;
#define elem_offset(TYPE,MEMBER)	( (u32) &((TYPE*)0)->MEMBER)
//#define containerof(PTR,TYPE,MEMBER)	{(TYPE*)((u8*)PTR - elem_offset(TYPE,MEMBER);})
#define containerof(ptr,type,member)	({			\
		const typeof( ((type*)0)->member )*__mptr = (ptr);\
		(type*)((u8*)__mptr - elem_offset(type,member));})
#define IS_LAST_ENTRY(lst_hd)	((lst_hd)->next == (lst_hd))
#define IS_FIRST_ENTRY(lst_hd)	((lst_hd)->pre == (lst_hd))
#endif
