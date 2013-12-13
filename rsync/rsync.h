#ifndef _RSYNC_H
/* containerof(ptr,type,member) */
#define _RSYNC_H
#define FILE_NAME_LEN	128

#define E_OK				00
#define E_SRC_FILE_NOT_EXIST	01
#define E_SRC_FILE_NO_BLK		02

#define BLOCK_SZ		32

#define DUP_BLOCK		00
#define N_DUP_BLOCK		01

#define MD5_SZ			16
#define MD5_EQUAL(p1,p2)	(memcmp((p1),(p2),MD5_SZ) == 0)
#define ROLLING_CHKSUM_M	(1<<16)
#define ROLLING_CHKSM_EQUAL(r1,r2)	((r1) == (r2))
typedef struct _chunk_file_header{
	u32 block_sz;/* block size */
	u64 block_nr;/* how many blocks in dst file */
	u8 fn[FILE_NAME_LEN];/* file to be synced */
}chunk_file_header;
#define CHUNK_FILE_HEAD_SZ		(sizeof(chunk_file_header))
typedef struct _reply_to_chunk_file_header{
	u8 err;
}reply_to_chunk_file_header;
#define RPL_TO_CHUNK_FILE_HEADER_SZ	(sizeof(reply_to_chunk_file_header))
typedef struct _chunk_block_entry{
	u64 block_no;
	u32 block_len;
	u32 rolling_chksm;
	u8 md5[MD5_SZ];
	list_head cbe_hash;
}chunk_block_entry;
#define CHUNK_BLOCK_ENTRY_SZ	(sizeof(chunk_block_entry))

typedef struct _delta_file_header{
	u64 block_nr;
}delta_file_header;
#define DELTA_FILE_HEADER_SZ	(sizeof(delta_file_header))
typedef struct _delta_block_entry{
	u64 offset;
	u32 len;
	u8 dup_flag;
	u64 dst_block_no;
	list_head dbe_list;
}delta_block_entry;
#define DELTA_BLOCK_ENTRY_SZ	(sizeof(delta_block_entry))
extern ssize_t Read(int fd,void * buf,size_t count);
extern ssize_t Write(int fd,void * buf,size_t count);
extern void cal_md5(u8 * ptr,u32 len,u8 md5[]);
extern u32 cal_rollin_cksm(u8 * const ptr,u32 * const aklz,u32 * const bklz,u32 blk_len);
extern u32 cal_rollin_cksm_plus_1(u8 o,u8 n,u32 * const akl,u32 * const bkl,u32 blk_len);
#endif
