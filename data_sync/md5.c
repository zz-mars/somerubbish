#include"glob.h"
#include"rsync.h"
void cal_md5(u8 * ptr,u32 len,u8 md5[])
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx,ptr,len);
	bzero(md5,MD5_SZ + 1);
	MD5_Final(md5,&ctx);
	return;
}
