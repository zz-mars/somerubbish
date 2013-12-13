#ifndef _MD_TYPE
#define _MD_TYPE
/* IO_Node,used to record IO infomation */
typedef struct{
	char io_type;//IO_READ or IO_WRITE
	u64 offset;
	time_t io_time;
	/* to make a list of io_node */
	char ion_pre[IO_NODE_KEY_LEN];
	char ion_next[IO_NODE_KEY_LEN];
}IO_Node;
/* IO_Q stat infomation */
typedef struct{
	int head;
	int tail;
	int first_write;
	time_t io_q_array[IO_Q_LEN];
}IO_Q;
/* replica info
 * dtc files are also taken as one of the replicas,
 * which are identified by host ip and starting_dorectory */
typedef struct{
	char IS_DTC_FILE; //DTC_FILE or CACHE_FILE
//	char host_ip[INET_ADDRSTRLEN];
	char starting_directory[FILE_PATH_LEN];
	char io_node_ptr[IO_NODE_KEY_LEN];
	char rep_consistency; // is this replica in consistent stat
}REPLICA;
typedef struct{
	struct stat stat_info;
	REPLICA my_rep[REPLICA_NUM];
	IO_Q ioq;
	char io_node_q_head[IO_NODE_KEY_LEN];
	char io_node_q_tail[IO_NODE_KEY_LEN];
}Meta_Data;
/* files are listed according to their io_time */
typedef struct{
	char fn_pre[FILE_PATH_LEN];
	char fn_next[FILE_PATH_LEN];
	time_t mt;
	char dirty;//O_DIRTY or O_CLEAN
}File_Node;
typedef struct{
	char head[FILE_PATH_LEN];
	char tail[FILE_PATH_LEN];
	char update[FILE_PATH_LEN];
}FN_List;
#endif
