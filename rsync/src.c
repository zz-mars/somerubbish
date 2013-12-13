#include"glob.h"
#include"rsync.h"
#define DUP_FOUND		01
#define DUP_NOT_FOUND	00
#define CHUNK_BLK_HASH	256
#define EVERY_READ_BYTES	1024
static u8 buf[BUFSIZ];
static u8 * ptr = buf + DELTA_BLOCK_ENTRY_SZ;
/* delta block */
static delta_file_header dfh;
static u64 delta_block_nr = 0;
static list_head * dblk_link_list_head = NULL;
static list_head * dblk_link_list_tail = NULL;
static inline delta_block_entry * new_dblk_ent()
{
	delta_block_entry * dblk = (delta_block_entry*)malloc(DELTA_BLOCK_ENTRY_SZ);
	if(dblk == NULL){
		perror("malloc for a new delta_block_entry");
		return NULL;
	}
	dblk->dbe_list.pre = &(dblk->dbe_list);
	dblk->dbe_list.next = &(dblk->dbe_list);
	return dblk;
}
static inline void dblke_add_to_tail(delta_block_entry * dblke)
{
	if(dblk_link_list_head == NULL && dblk_link_list_tail == NULL){
		dblk_link_list_head = &(dblke->dbe_list);
		dblk_link_list_tail = &(dblke->dbe_list);
		return;
	}
	dblk_link_list_tail->next = &(dblke->dbe_list);
	dblke->dbe_list.pre = dblk_link_list_tail;
	dblk_link_list_tail = &(dblke->dbe_list);
	return;
}
static void update_delta_block_list(u64 delta_region_off,u64 dup_blk_off,u32 n,const chunk_block_entry * cblk,u32 block_sz)
{
	u64 off,bytes_left;
	delta_block_entry * dblke;
	off = delta_region_off;
	while(off < dup_blk_off){
		dblke = new_dblk_ent();
		dblke->offset = off;
		dblke->dup_flag = N_DUP_BLOCK;
		dblke->len = block_sz;
		bytes_left = dup_blk_off - off;
		if(bytes_left >= block_sz){
			dblke->len = bytes_left;
		}
		dblke_add_to_tail(dblke);
		delta_block_nr++;
		off += dblke->len;
	}
	if(cblk == NULL){
		return;
	}
	dblke = new_dblk_ent();
	dblke->offset = dup_blk_off;
	dblke->len = n;
	dblke->dup_flag = DUP_BLOCK;
	dblke->dst_block_no = cblk->block_no;
	dblke_add_to_tail(dblke);
	return;
}
static u32 send_delta_block(int fd,int connfd)
{
	u32 d_len;
	u64 d_off;
	u32 size_to_send = DELTA_BLOCK_ENTRY_SZ;
	delta_block_entry * dblk;
	list_head * dlst = dblk_link_list_head;
	if(dlst == NULL){
		printf("no delta block to send!\n");
		return 0;
	}
	while(1){
		dblk = containerof(dlst,delta_block_entry,dbe_list);
		d_len = dblk->len;
		d_off = dblk->offset;
		if(dblk->dup_flag == N_DUP_BLOCK){
			size_to_send += d_len;
		}
		lseek(fd,d_off,SEEK_SET);
		if(Read(fd,ptr,d_len) != d_len){
			fprintf(stderr,"send_delta_block : less bytes read from fd!\n");
			return 1;
		}
		memcpy(buf,dblk,DELTA_BLOCK_ENTRY_SZ);
		if(Write(connfd,buf,size_to_send) != size_to_send){
			fprintf(stderr,"send_delta_block : less bytes written to socket!\n");
			return 2;
		}
		if(IS_LAST_ENTRY(dlst)){
			break;
		}
		dlst = dlst->next;
	}
	return 0;
}
static void free_dblk_list(void)
{
	u8 last_dblk = 0;
	delta_block_entry * dblk;
	list_head * lst = dblk_link_list_head;
	if(lst == NULL)
		return;
	while(1){
		dblk = containerof(lst,delta_block_entry,dbe_list);
		if(IS_LAST_ENTRY(lst)){
			last_dblk = 1;
		}
		free(dblk);
		if(last_dblk == 1){
			break;
		}
		lst = lst->next;
	}
	return;
}
/* buffer */
static int buf_file_fd;/* opened fd of buffered file */
static u64 buf_file_sz;/* file size */
static u64 buf_file_off;/* corresponding file offset */
static u32 buf_bytes;/* total bytes in buffer */
/* chunk block entry hash table */
static list_head * chunk_blk_entry_hash[CHUNK_BLK_HASH] = {NULL};
#define ROLLING_CHECKSUM_HASH(rolling_chksm)	((rolling_chksm)%CHUNK_BLK_HASH)
static void buf_init(int fd,u64 file_sz)
{
	buf_file_fd = fd;
	buf_file_sz = file_sz;
	buf_file_off = 0;
	buf_bytes = 0;
	return;
}
static u32 fill_buf(u64 off,u32 len)
{
	/* fill buffer with data whose offset is 'off'
	 *								length is 'len'
	 * return value is the bytes filled into buffer.
	 * */
	u32 n;
	bzero(buf,BUFSIZ);
	lseek(buf_file_fd,off,SEEK_SET);
	n = read(buf_file_fd,buf,len);
	if((n == len) || \
			(n < len && (n + off) == buf_file_sz)){
		/* accepted case */
		buf_file_off = off;
		buf_bytes = n;
		return 0;
	}
	return 1;
}
static u8 * read_from_buf(u64 offset,u32 bytes,u32 * n)
{
	u64 buf_first_byte;
	u64 buf_last_byte;
	u64 req_first_byte;
	u64 req_last_byte;
retry_read_from_buf:
	buf_first_byte = buf_file_off;
	buf_last_byte = buf_file_off + buf_bytes;
	req_first_byte = offset;
	req_last_byte = offset + bytes;
	if(req_last_byte <= buf_last_byte && req_first_byte >= buf_first_byte){
		/* ------------------- accepted case 0 ---------------------*/
		*n = bytes;
		return (buf + req_first_byte - buf_first_byte);
	}
	if(req_first_byte >= buf_first_byte && \
			req_first_byte < buf_last_byte && \
			req_last_byte >= buf_last_byte && \
			buf_last_byte == buf_file_sz){
		/*------------------ accepted case 1 --------------------*/
		*n = buf_last_byte - req_first_byte;
		return (buf + req_first_byte - buf_first_byte);
	}
	/* ---------- unaccepted case --------- */
	if(fill_buf(offset - 1,EVERY_READ_BYTES) == 0){
		goto retry_read_from_buf;
	}
	fprintf(stderr,"fill buffer fail!\n");
	return NULL;
}
int main()
{
	/* rsync variables */
	u8 file_name[FILE_NAME_LEN] = {'\0'};
	u32 block_sz;
	u32 i;
	list_head ** lst_hd;
	list_head * lh_pre,* lh_next;
	u32 cblk_hash_v;
	/* chunk block */
	u64 chunk_block_nr;
	chunk_file_header cfh;
	reply_to_chunk_file_header rplh;
	chunk_block_entry * cblk;
	chunk_block_entry * cblk_array;
	chunk_block_entry * cblk_p;
	chunk_block_entry * cblk_current;
	u64 chunk_block_no;
	u32 chunk_block_rollin_chksm;
	u32 chunk_block_len;
	u8 * chunk_block_md5;
	u32 brk_v;
	u32 is_first_entry,is_last_entry;
	/* searching for dup blocks */
	u64 remaining_bytes;
	u64 delta_region_off;
	u64 dup_blk_off;
	u32 dup_blk_len;
	u32 rolling_chksm;
	u8 md5[MD5_SZ];
	u32 n;
	u8 * p;
	u32 akl,bkl,skl;
	u8 dup_found;
	u8 oc,nc;
	/* file to sync */
	int fd;
	struct stat file_stat;
	u64 file_sz;
	/* socket variables */
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
	if(Read(connfd,&cfh,CHUNK_FILE_HEAD_SZ) != CHUNK_FILE_HEAD_SZ){
		perror("read chunk_file_header");
		goto over1;
	}
	printf("--------- read chunk_blk_header ok -----------\n");
	block_sz = cfh.block_sz;
	chunk_block_nr = cfh.block_nr;
	strncpy(file_name,cfh.fn,strlen(cfh.fn));
	printf("---	block_sz		-- # %d\n",block_sz);
	printf("---	block_nr		-- # %d\n",chunk_block_nr);
	printf("---	file_to_sync	-- # %s\n",file_name);
	rplh.err = E_OK;
	if((fd = open(file_name,O_RDONLY)) < 0){
		perror("open src file");
		if(errno == ENOENT){
			/* when file not exist,goto send_delta_file_header
			 * need special disposition */
			/* tell dst to delete file */
			rplh.err = E_SRC_FILE_NOT_EXIST;
			goto SEND_RPL_TO_CFH;
		}
		goto over1;
	}
	if(fstat(fd,&file_stat) != 0){
		perror("fstat");
		goto over2;
	}
	file_sz = file_stat.st_size;
	if(file_sz == 0){
		rplh.err = E_SRC_FILE_NO_BLK;
	}
SEND_RPL_TO_CFH:
	/* send reply to chunk file header */
	if(Write(connfd,&rplh,RPL_TO_CHUNK_FILE_HEADER_SZ) != RPL_TO_CHUNK_FILE_HEADER_SZ){
		perror("write rpl to chunk file header");
		goto over2;
	}
	if(rplh.err != E_OK){
		goto over2;
	}
	/* dst file has 0 block */
	if(chunk_block_nr == 0){
		/* need special disposition */
		printf("dst file has no block!\n");
		update_delta_block_list(0,file_sz,0,NULL,block_sz);
		goto send_delta_file_header;
	}
	cblk_array = (chunk_block_entry*)malloc(chunk_block_nr*CHUNK_BLOCK_ENTRY_SZ);
	if(cblk_array == NULL){
		perror("malloc for chunk_block_entry_array");
		goto over2;
	}
	/* receive chunk block entry */
	for(i = 0;i < chunk_block_nr;i++){
		cblk_p = cblk_array + i;
		if(Read(connfd,cblk_p,CHUNK_BLOCK_ENTRY_SZ) != CHUNK_BLOCK_ENTRY_SZ){
			perror("read chunk_block_entry");
			goto over3;
		}
		chunk_block_rollin_chksm = cblk_p->rolling_chksm;
		chunk_block_len = cblk_p->block_len;
		chunk_block_md5 = cblk_p->md5;
		cblk_p->cbe_hash.next = &(cblk_p->cbe_hash);
		cblk_p->cbe_hash.pre = &(cblk_p->cbe_hash);
		cblk_hash_v = ROLLING_CHECKSUM_HASH(cblk_p->rolling_chksm);
		lst_hd = &chunk_blk_entry_hash[cblk_hash_v];
		if(*lst_hd == NULL){
			*lst_hd = &(cblk_p->cbe_hash);
			continue;
		}
		while(1){
			brk_v = 0;
			cblk_current = containerof(*lst_hd,chunk_block_entry,cbe_hash);
			if(cblk_current->block_len == chunk_block_len && \
		ROLLING_CHKSM_EQUAL(cblk_current->rolling_chksm,chunk_block_rollin_chksm)){
				if(MD5_EQUAL(cblk_current->md5,chunk_block_md5)){
					printf("skip identical chunk_block_entry\n");
					brk_v = 1;
					break;
				}
			}
			if(cblk_current->rolling_chksm > chunk_block_rollin_chksm){
				/* insert into the place before cblk_current */
				brk_v = 2;
				break;
			}
			if(IS_LAST_ENTRY(*lst_hd)){
				/* insert to tail */
				brk_v = 3;
				break;
			}
			lst_hd = &((*lst_hd)->next);
		}
		switch(brk_v){
			case 1:
				break;
			case 2:
				is_first_entry = 0;
				if(IS_FIRST_ENTRY(*lst_hd)){
					is_first_entry = 1;
				}else{
					lh_pre = (*lst_hd)->pre;
				}
				(*lst_hd)->pre = &(cblk_p->cbe_hash);
				cblk_p->cbe_hash.next = *lst_hd;
				if(is_first_entry == 1){
					chunk_blk_entry_hash[cblk_hash_v] = &(cblk_p->cbe_hash);
				}else{
					lh_pre->next = &(cblk_p->cbe_hash);
					cblk_p->cbe_hash.pre = lh_pre;
				}
				break;
			case 3:
				(*lst_hd)->next = &(cblk_p->cbe_hash);
				cblk_p->cbe_hash.pre = *lst_hd;
				break;
			default:
				break;
		}
	}
	printf("%d chunk block entrys received!\n",i);
	/* all chunk_block_entryS have been received 
	 * Now find the duplicate block from the src file */
	dup_blk_off = 0;
	dup_blk_len = block_sz;
	delta_region_off = 0;
	/* initialize buffer */
	buf_init(fd,file_sz);
	dup_found = DUP_FOUND;
	/* find duplicate block 
	 * and generate the list of delta_block */
	while(dup_blk_off < file_sz){
		n = 0;
		p = read_from_buf(dup_blk_off,dup_blk_len,&n);
		if(p == NULL){
			fprintf(stderr,"some errors happened when read_from_buf\n");
			goto over3;
		}
		/* else p is the pointer of the block 
		 * which is going to be checked 
		 * and the number of bytes is 'n' */
		if(dup_found == DUP_FOUND){
			/* start a new search or
			 * current block is the last block of src_file */
			printf("calculate the rolling_checksum with cal_rollin_cksm \n");
			akl = 0,bkl = 0,skl = 0;
			skl = cal_rollin_cksm(p,&akl,&bkl,n);
		}else if(dup_found == DUP_NOT_FOUND){
			/* the only case that rolling_checksum is calculated with last value */
			printf("calculate the rolling_checksum with cal_rollin_cksm_plus_1 \n");
			oc = *(p - 1);
			nc = (n < dup_blk_len?0:(*(p + n - 1)));
			skl = cal_rollin_cksm_plus_1(oc,nc,&akl,&bkl,dup_blk_len);
		}
		dup_found = DUP_NOT_FOUND;
		rolling_chksm = skl;
		cblk_hash_v = ROLLING_CHECKSUM_HASH(rolling_chksm);
		lst_hd = &(chunk_blk_entry_hash[cblk_hash_v]);
		/* searching */
		if(*lst_hd == NULL){
			/* no match,
			 * right shift one byte and continue */
			goto MODIFY_SOME_VAR_AND_GOTO_NEXT_LOOP;
		}
		while(1){
			cblk_current = containerof(*lst_hd,chunk_block_entry,cbe_hash);
			if(cblk_current->block_len == n && \
					ROLLING_CHKSM_EQUAL(cblk_current->rolling_chksm,rolling_chksm)){
				/* same block_len and rolling_chksm,
				 * need further checking md5 */
				cal_md5(p,n,md5);
				if(MD5_EQUAL(cblk_current->md5,md5)){
					dup_found = DUP_FOUND;
					break;
				}
			}
			if(cblk_current->rolling_chksm > rolling_chksm){
				/* because collided chunk_blk_entry are sorted 
				 * no need to search further here */
				break;
			}
			if(IS_LAST_ENTRY(*lst_hd)){
				/* the last entry */
				break;
			}
			lst_hd = &((*lst_hd)->next);
		}
MODIFY_SOME_VAR_AND_GOTO_NEXT_LOOP:
		if(dup_found == DUP_FOUND){
			/* dup_block found,
			 * 1) make delta_block_entry list,
			 * 2) reset : delta_region_off 
			 *			  delta_region_len
			 *			  dup_blk_off 
			 *	mention that dup_blk_off should be right shifted 'n' bytes */
			update_delta_block_list(delta_region_off,dup_blk_off,n,cblk_current,dup_blk_len);
			dup_blk_off += n;
			delta_region_off = dup_blk_off;
		}else if(dup_found == DUP_NOT_FOUND){
			/* dup_block not found,
			  right shift one byte and continue */
			dup_blk_off++;
		}
	}
	printf("finding dup_block over,delta_block_entry has been in the list!\n");
	/* sending delta_file_header */
send_delta_file_header:
	dfh.block_nr = delta_block_nr;
	if(Write(connfd,&dfh,DELTA_FILE_HEADER_SZ) != DELTA_FILE_HEADER_SZ){
		fprintf(stderr,"write delta_file_header fail!\n");
		goto over3;
	}
	if(send_delta_block(fd,connfd) != 0){
		fprintf(stderr,"some errors happened when sending delta_block!\n");
	}
over3:
	if(chunk_block_nr == 0){
		goto over2;
	}
	free(cblk_array);
over2:
	close(fd);
over1:
	close(connfd);
	return 0;
}
