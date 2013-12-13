#include"glob.h"
#include"rsync.h"
static u8 buf[BUFSIZ];
static u8 * ptr = buf + DELTA_BLOCK_ENTRY_SZ;
int main()
{
	u64 blk_nr,blk_off;
	u32 blk_len;
	server_file_header sfh;
	int i;
	chunk_block_entry cblk;
	u8 smd5[MD5_SZ+1];
	delta_block_entry dblk;
	u32 sz_to_send,n,out_of_loop_value;
	u64 serv_off,serv_file_sz,remaining_bytes;
	int fd;
	struct stat f_st;
	u32 clnt_blksz;
	u64 clnt_blknr,clnt_file_sz;
	u8 file_name[FILE_NAME_LEN];
	int port;
    int listenfd,connfd,len;
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in addr,cli_addr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,RSYNC_HOST_IP,(void*)&addr.sin_addr);
    addr.sin_port = htons(RSYNC_SRV_PORT);
    len = sizeof(addr);
    bind(listenfd,(struct sockaddr *)&addr,len);
    listen(listenfd,LISTENQ);
	len = sizeof(cli_addr);
	connfd = accept(listenfd,(struct sockaddr *)&cli_addr,&len);
	bzero(ip_str,INET_ADDRSTRLEN);
	inet_ntop(AF_INET,(void*)&cli_addr.sin_addr,ip_str,INET_ADDRSTRLEN);
	port = ntohs(cli_addr.sin_port);
	printf(" from %s,port %d\n",ip_str,port);
	/*-------------- Read chunk header ---------*/
	bzero(buf,BUFSIZ);
	n = Read(connfd,buf,CHUNK_FILE_HEAD_SZ);
	if(n != CHUNK_FILE_HEAD_SZ){
		fprintf(stderr,"Read chunk_file_head fail!\n");
		goto over2;
	}
	printf("---------------- chunk_file_header received! --------\n");
	/* init from chunk header */
	clnt_blksz = ((chunk_file_header *)buf)->block_sz;
	clnt_file_sz = ((chunk_file_header *)buf)->file_sz;
	bzero(file_name,FILE_NAME_LEN);
	strncpy(file_name,((chunk_file_header*)buf)->fn,FILE_NAME_LEN);
	printf("clnt_blksz   -- %d\n",clnt_blksz);
	printf("clnt_filesz  -- %d\n",clnt_file_sz);
	printf("file to sync -- %s\n",file_name);
	sfh.err = E_OK;
	fd = open(file_name,O_RDONLY);
	if(fd < 0){
		if(errno == ENOENT)
			sfh.err = E_FILE_NOT_EXIST;
		perror("open");
		goto over2;
	}
	lseek(fd,0,SEEK_SET);
	if(fstat(fd,&f_st) != 0){
		perror("fstat");
		goto over1;
	}
	serv_file_sz = f_st.st_size;
	sfh.file_sz = serv_file_sz;
	if((n = Write(connfd,&sfh,SERVER_FILE_HEADER_SZ)) != SERVER_FILE_HEADER_SZ){
		perror("Write server_file_header");
		goto over1;
	}
	printf("server_file_header sent successfully!\n");
	if(sfh.err != E_OK){
		goto over1;
	}
	blk_nr = serv_file_sz/clnt_blksz;
	if(serv_file_sz % clnt_blksz != 0){
		blk_nr++;
	}
	printf("serv_file_sz		#%d\n",serv_file_sz);
	printf("serv_block_nr		#%d\n",blk_nr);
	for(i = 0;i < blk_nr;i++){
		bzero(&cblk,CHUNK_BLOCK_ENTRY_SZ);
		if((n = Read(connfd,&cblk,CHUNK_BLOCK_ENTRY_SZ)) != CHUNK_BLOCK_ENTRY_SZ){
			perror("Read chunk blk entry");
			goto over1;
		}
		printf("---------chunk_block_entry		#%d :\n",i);
		printf("chunk_blk_offset	#%d\n",cblk.offset);
		printf("chunk_blk_len		#%d\n",cblk.len);
		sz_to_send = DELTA_BLOCK_ENTRY_SZ;
		dblk.old_len = cblk.len;
		dblk.offset = cblk.offset;
		dblk.embeded = BLK_SAME;
		blk_off = i*clnt_blksz;
		blk_len = clnt_blksz;
		if(blk_off != cblk.offset){
			printf("blk_off == %d\n",blk_off);
			fprintf(stderr,"blk_off != cblk.offset\n");
			exit(1);
		}
		if(i == (blk_nr - 1)){
			blk_len = serv_file_sz - blk_off;
		}
		dblk.new_len = blk_len;
		if((n = Read(fd,ptr,blk_len)) != blk_len){
			perror("Read blk");
			goto over1;
		}
		*(ptr + n) = '\0';
		printf("serv_blk_data --------\n");
		printf("%s\n",ptr);
		if(cblk.len == 0){
			printf("chunk_block_entry len is 0,send the block and data to client\n");
			printf("no need to calculate MD5\n");
			dblk.embeded = BLK_DIFF;
			sz_to_send += n;
			printf("%d bytes data will be appened to delta_block!\n",n);
			goto send_delta_blk;
		}
		cal_md5(ptr,clnt_blksz,smd5);
		if(!MD5_EQUAL(cblk.md5,smd5)){
			printf("--------- md5 different ----------\n");
			dblk.embeded = BLK_DIFF;
			sz_to_send += n;
			printf("%d bytes data will be appened to delta_block!\n",n);
		}else{
			printf("--------- md5 same ----------\n");
		}
send_delta_blk:
		memcpy(buf,&dblk,DELTA_BLOCK_ENTRY_SZ);
		if((n = Write(connfd,buf,sz_to_send)) != sz_to_send){
			perror("Write socket");
			goto over1;
		}
		printf("DELTA_BLOCK_ENTRY_SZ sz	%d\n",DELTA_BLOCK_ENTRY_SZ);
		printf("%d bytes sent to client!\n",n);
	}
over1:
	close(fd);
over2:
	close(connfd);
	return 0;
}
