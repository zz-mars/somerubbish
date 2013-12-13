#include"glob.h"
#include"rsync.h"
#define CBUFSZ	(BLK_SZ+DELTA_BLOCK_ENTRY_SZ+1)
#define NEW_FILE_SUFFIX	".rep"
static u8 buf[BUFSIZ];
int main(int argc,char * argv[])
{
	chunk_file_header cfh;
	reply_to_chunk_file_header rplcfh;
	chunk_block_entry cblk;
	u64 c_off,remaining_bytes;
	u32 c_len;
	delta_file_header dfh;
	/* delta block entry */
	u32 dblk_nr;
	delta_block_entry dblk;
	u64 d_off;
	u32 d_len;
	u8 d_dup_flag;
	u64 d_dst_block_no;
	u8 * file_name;
	u8 n_file_name[FILE_NAME_LEN];
	u32 file_name_len;
	u32 i,n;
	int fd,n_fd;
	struct stat fstt;
	u64 file_sz;
	u64 block_nr;
	u32 block_sz;
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
	fstat(fd,&fstt);
	file_sz = fstt.st_size;
	block_sz = BLOCK_SZ;
	block_nr = file_sz/BLOCK_SZ;
	if(file_sz%BLOCK_SZ != 0){
		block_nr += 1;
	}
	cfh.block_sz = block_sz;
	cfh.block_nr = block_nr;
	strncpy(cfh.fn,file_name,strlen(file_name));
	printf("file_to_sync	# %s\n",cfh.fn);
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
	/* ------------- receive rpl_to_chunk_file_header ------------- */
	if((n = Read(connfd,&rplcfh,RPL_TO_CHUNK_FILE_HEADER_SZ)) != RPL_TO_CHUNK_FILE_HEADER_SZ){
		perror("Read reply_to_chunk_file_header");
		goto over1;
	}
	printf("------------ reply_to_chunk_file_header receive ok! ---------\n");
	switch(rplcfh.err){
		case E_SRC_FILE_NOT_EXIST:
			printf("src file not exist,rm file in dst!\n");
			close(fd);
			if(unlink(file_name) != 0){
				perror("unlink file");
				exit(1);
			}
			goto over1;
		case E_SRC_FILE_NO_BLK:
			printf("src file no blk,truncate file in dst!\n");
			if(ftruncate(fd,0) != 0){
				perror("ftruncate dst file");
				exit(1);
			}
			goto over1;
		case E_OK:
			break;
	}
	/* send chunk_block_entry */
	for(i = 0;i < block_nr;i++){
		printf("-------------------chunk_blk_entry		#%d\n",i);
		bzero(&cblk,CHUNK_BLOCK_ENTRY_SZ);
		c_off = i*block_sz;
		cblk.block_no = i;
		remaining_bytes = file_sz - c_off;
		if(remaining_bytes >= block_sz){
			printf("remaining_bytes is not less than block_sz\n");
			c_len = block_sz;
		}else{
			printf("remaining_bytes is less than block_sz\n");
			c_len = remaining_bytes;
		}
		cblk.block_len = c_len;
		bzero(buf,BUFSIZ);
		lseek(fd,c_off,SEEK_SET);
		if((n = Read(fd,buf,c_len)) != c_len){
			perror("client Read file");
			fprintf(stderr,"%d bytes need but only %d bytes Read!\n",c_len,n);
			goto over1;
		}
		if(c_len < block_sz){
			bzero(buf+n,block_sz - c_len);
		}
		cblk.rolling_chksm = cal_rollin_cksm(buf,NULL,NULL,block_sz);
		cal_md5(buf,c_len,cblk.md5);
		if((n = Write(connfd,&cblk,CHUNK_BLOCK_ENTRY_SZ)) != CHUNK_BLOCK_ENTRY_SZ){
			perror("client Write chunk_block_entry");
			goto over1;
		}
	}
	if(Read(connfd,&dfh,DELTA_FILE_HEADER_SZ) != DELTA_FILE_HEADER_SZ){
		perror("read delta_file_header");
		goto over1;
	}
	dblk_nr = dfh.block_nr;
	bzero(n_file_name,FILE_NAME_LEN);
	file_name_len = strlen(file_name);
	strncpy(n_file_name,file_name,file_name_len);
	strncpy(n_file_name+file_name_len,NEW_FILE_SUFFIX,strlen(NEW_FILE_SUFFIX));
	n_fd = open(n_file_name,O_CREAT | O_WRONLY,0660);
	if(n_fd < 0){
		goto over1;
	}
	for(i = 0;i <  dblk_nr;i++){
		if(Read(connfd,&dblk,DELTA_BLOCK_ENTRY_SZ) != DELTA_BLOCK_ENTRY_SZ){
			perror("Read delta_block_entry");
			exit(1);
		}
		printf("---------------------Reading delta block	#%d\n",i);
		d_dup_flag = dblk.dup_flag;
		d_off = dblk.offset;
		d_len = dblk.len;
		printf("delta_blk_off #%d\n",d_off);
		printf("delta_blk_len #%d\n",d_len);
		if(d_dup_flag == DUP_BLOCK){
			d_dst_block_no = dblk.dst_block_no;
			printf("corresponding dst_block_no		#%d\n",d_dst_block_no);
			lseek(fd,d_dst_block_no*block_sz,SEEK_SET);
			if(Read(fd,buf,d_len) != d_len){
				perror("read data from dst file");
				goto over2;
			}
		}else{
			printf("data is in the following!\n");
			if(Read(connfd,buf,d_len) != d_len){
				perror("read data from socket");
				goto over2;
			}
		}
		lseek(n_fd,d_off,SEEK_SET);
		if(Write(n_fd,buf,d_len) != d_len){
			perror("write to new file");
			goto over2;
		}
	}
over2:
	close(n_fd);
over1:
	close(connfd);
	close(fd);
    return 0;
}
