#include"glob.h"
#include"rsync.h"
#define CBUFSZ	(BLK_SZ+DELTA_BLOCK_ENTRY_SZ+1)
static void * do_delta_blk(void * arg)
{
	u64 d_off,blk_nr = ((thread_arg*)arg)->blk_nr;
	u32 d_old_len,d_new_len;
	u8 d_embeded;
	u8 buf[CBUFSZ];
	int blk_counter = 0;
	int connfd = ((thread_arg *)arg)->connfd;
	int fd = ((thread_arg *)arg)->fd;
	int n;
	u64 i;
	delta_block_entry dblk;
	for(i = 0;i<blk_nr;i++){
		if((n = Read(connfd,&dblk,DELTA_BLOCK_ENTRY_SZ)) != DELTA_BLOCK_ENTRY_SZ){
			perror("Read delta_block_entry");
			exit(1);
		}
		printf("---------------------Reading delta block	#%d\n",i);
		d_embeded = dblk.embeded;
		d_off = dblk.offset;
		d_old_len = dblk.old_len;
		d_new_len = dblk.new_len;
		blk_counter = d_off/BLK_SZ;
		if(d_off%BLK_SZ != 0){
			fprintf(stderr,"delta_blk offset isnot aligned to BLK_SZ!\n");
			goto thRead_exit;
		}
		printf("delta_blk_old_len #%d\n",d_old_len);
		printf("delta_blk_new_len #%d\n",d_new_len);
		printf("delta_blk_offset  #%d\n",d_off);
		if(d_embeded == BLK_SAME){
			/* no need to sync ,just continue */
			printf("delta_blk_emb #BLK_SAME\n");
			continue;
		}
		printf("delta_blk_emb #BLK_DIFF\n");
		/* need overWrite */
		bzero(buf,CBUFSZ);
		if((n = Read(connfd,buf,d_new_len)) != d_new_len){
			/* something is wrong */
			perror("Read from socket");
			goto thRead_exit;
		}
		*(buf + n) = '\0';
		printf("delta content \n%s\n",buf);
		lseek(fd,d_off,SEEK_SET);
		if((n = Write(fd,buf,d_new_len)) != d_new_len){
			/* something is wrong */
			perror("Write to file");
			goto thRead_exit;
		}
		printf("--------------------------------------delta blk #%d ok!\n",i);
	}
thRead_exit:
	pthread_exit(NULL);
}
int main(int argc,char * argv[])
{
	pthread_t thid;
	thread_arg arg;
	int fd,n;
	u8 buf[CBUFSZ];
	struct stat cstat;
	chunk_file_header cfh;
	server_file_header sfh;
	chunk_block_entry cblk;
	u32 block_sz;
	u64 block_nr;
	u64 c_off,file_sz,i,remaining_bytes;
	u64 srv_file_sz;
	u32 c_len;
	u8 * file_name;
    int connfd,clen;
    struct sockaddr_in addr;
	if(argc != 2){
		fprintf(stderr,"invalid argument!\n");
		exit(1);
	}
	file_name = argv[1];
	fd = open(file_name,O_RDWR);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	fstat(fd,&cstat);
	file_sz = cstat.st_size;
	bzero(&cfh,CHUNK_FILE_HEAD_SZ);
	block_sz = BLK_SZ;
	block_nr = file_sz/BLK_SZ;
	if(file_sz%BLK_SZ != 0){
		block_nr += 1;
	}
	cfh.file_sz = file_sz;
	cfh.block_sz = block_sz;
	strncpy(cfh.fn,file_name,FILE_NAME_LEN);
	printf("file_to_sync	# %s\n",cfh.fn);
	printf("file_sz			# %d\n",cfh.file_sz);
	printf("block_sz		# %d\n",cfh.block_sz);
	printf("block_nr		# %d\n",block_nr);
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,RSYNC_HOST_IP,(void *)&addr.sin_addr);
    addr.sin_port = htons(RSYNC_SRV_PORT);
    clen = sizeof(addr);
    if((connfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("Socket");
		goto over1;
    }
    if(connect(connfd,(struct sockaddr*)&addr,clen) == -1){
        perror("Connection");
		goto over1;
    }
	/*---------------- Write chunk_file_header to server ------------------*/
	if((n = Write(connfd,&cfh,CHUNK_FILE_HEAD_SZ)) != CHUNK_FILE_HEAD_SZ){
		perror("Write chunk_file_header");
		goto over1;
	}
	printf("------------------------ chunk file header send ok!-----------------\n");
	if((n = Read(connfd,&sfh,SERVER_FILE_HEADER_SZ)) != SERVER_FILE_HEADER_SZ){
		perror("Read server_file_header");
		goto over1;
	}
	printf("------------------------ server file header receive ok!-------------\n");
	if(sfh.err != E_OK){
		printf("server file header says some error happens!\n");
		goto over1;
	}
	srv_file_sz = sfh.file_sz;
	printf("server file size		# %d\n",srv_file_sz);
	if(srv_file_sz < file_sz){
		printf("client file too big,need trunc\n");
		if(ftruncate(fd,srv_file_sz) != 0){
			perror("ftruncate");
			goto over1;
		}
		fstat(fd,&cstat);
		file_sz = cstat.st_size;
		printf("after truncate file sz --- %d\n",file_sz);
	}else{
		printf("client file is smller than server file!,no need to truncate!\n");
	}
	block_nr = srv_file_sz/BLK_SZ;
	if(srv_file_sz%BLK_SZ != 0){
		block_nr += 1;
	}
	printf("block number in server # %d\n",block_nr);
	printf("client send chunk block according this block_number,reguardless of the real blk_number it has\n");
	/*********************************************************************/
	printf("now create a new thRead to receive delta_block!\n");
	arg.connfd = connfd;
	arg.fd = fd;
	arg.blk_nr = block_nr;
	if(pthread_create(&thid,NULL,do_delta_blk,(void*)&arg) != 0){
		perror("pthread_create");
		goto over1;
	}
	for(i = 0;i < block_nr;i++){
		printf("-------------------chunk_blk_entry		#%d\n",i);
		bzero(&cblk,CHUNK_BLOCK_ENTRY_SZ);
		c_off = i*block_sz;
		cblk.offset = c_off;
		printf("block offset		#%d\n",c_off);
		if(file_sz <= c_off){
			printf("while client file sz #%d\n",file_sz);
			printf("out of boundry,send chunk_blk_entry with len = 0\n");
			c_len = 0;
			cblk.len = 0;
			goto send_chunk_blk_entry;
		}
		remaining_bytes = file_sz - c_off;
		printf("client remaining_bytes -- %d\n",remaining_bytes);
		if(remaining_bytes >= block_sz){
			printf("remaining_bytes is not less than block_sz\n");
			c_len = block_sz;
		}else{
			printf("remaining_bytes is less than block_sz\n");
			c_len = remaining_bytes;
		}
		cblk.len = c_len;
		bzero(buf,CBUFSZ);
		lseek(fd,c_off,SEEK_SET);
		if((n = Read(fd,buf,c_len)) != c_len){
			perror("client Read file");
			fprintf(stderr,"%d bytes need but only %d bytes Read!\n",c_len,n);
			goto over1;
		}
		*(buf + n) = '\0';
		printf("content to calculate MD5 :\n%s\n",buf);
		cal_md5(buf,c_len,cblk.md5);
send_chunk_blk_entry:
		if((n = Write(connfd,&cblk,CHUNK_BLOCK_ENTRY_SZ)) != CHUNK_BLOCK_ENTRY_SZ){
			perror("client Write chunk_block_entry");
			goto over1;
		}
		printf("chunk_block_entry		#%d sent successfully!\n",i);
	}
over2:
	if(pthread_join(thid,NULL) != 0){
		perror("pthread join");
	}
over1:
	close(fd);
	close(connfd);
    return 0;
}
