
struct page_header {
	uint32_t page_status;
	uint32_t valid_length;
	unsigned char buffer[0];
};

struct buffer_header {
	uint32_t guard_band;
	uint32_t allocate_index;
	uint32_t free_index;
	uint32_t read_index;
	uint32_t write_index;
	uint32_t page_size;
	uint32_t data_buffer_size;
	struct page_header pages[0];
};

typedef int (*ccci_shm_srv_read_callback)(void);

struct ccci_ccb_config {
	CCCI_USER user_id;
	unsigned char core_id;
	unsigned int dl_page_size;
	unsigned int ul_page_size;
	unsigned int dl_buff_size;
	unsigned int ul_buff_size;
};

struct ccci_ccb_control_user {
	CCCI_USER user_id;
	unsigned int buffer_num;
	unsigned char *base_address;
	unsigned int total_length;
	int fd;
	struct ccci_ccb_control_buff *buffers;
};

struct ccci_ccb_control_buff {
	unsigned int dl_page_num;
	unsigned int ul_page_num;
	struct buffer_header *dl_header;
	struct buffer_header *ul_header;
	unsigned int *dl_guard_band_e;
	unsigned int *ul_guard_band_e;
};

enum {
	PAGE_STATUS_INIT = 0,
	PAGE_STATUS_ALLOC = 0x11111111,
	PAGE_STATUS_WRITE_DONE = 0x22222222,
};

enum {
	P_CORE = 0,
	VOLTE_CORE,
};

enum {
	CCB_USER_INVALID = 0,
	CCB_USER_OK,
	CCB_USER_ERR,
};

#define HEADER_MAGIC_BEFORE 0xAABBCCDD
#define HEADER_MAGIC_AFTER 0xDDBBCCAA
#define TAIL_MAGIC_BEFORE 0xEEFF0011
#define TAIL_MAGIC_AFTER 0x1100FFEE
