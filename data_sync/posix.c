#include"glob.h"
ssize_t Read(int fd,void * buf,size_t count)
{
	int n = 0;
	int i;
	u8 * p = buf;
	while(n != count){
		i = read(fd,buf+n,count-n);
		if(i < 0 && errno == EINTR)
			continue;
		n += i;
	}
	return n;
}
ssize_t Write(int fd,void * buf,size_t count)
{
	int n = 0;
	int i;
	u8 * p = buf;
	while(n != count){
		i = write(fd,buf+n,count-n);
		if(i < 0 && errno == EINTR)
			continue;
		n += i;
	}
	return n;
}
