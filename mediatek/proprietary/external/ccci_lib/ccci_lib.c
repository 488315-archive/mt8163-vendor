/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ccci_lib.c
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   
 *
 * Author:
 * -------
 *   
 *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <android/log.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <pthread.h>
#include "hardware/ccci_intf.h"
#include "ccci_lib.h"

//----------------debug log define-----------------//
#define LOGV(...)	do{ __android_log_print(ANDROID_LOG_VERBOSE, "ccci_lib", __VA_ARGS__); }while(0)

#define LOGD(...)	do{ __android_log_print(ANDROID_LOG_DEBUG, "ccci_lib", __VA_ARGS__); }while(0)

#define LOGI(...)	do{ __android_log_print(ANDROID_LOG_INFO, "ccci_lib", __VA_ARGS__); }while(0)

#define LOGW(...)	do{ __android_log_print(ANDROID_LOG_WARN, "ccci_lib", __VA_ARGS__); }while(0)

#define LOGE(...)	do{ __android_log_print(ANDROID_LOG_ERROR, "ccci_lib", __VA_ARGS__); }while(0)

static int parse_info(char raw_data[], int raw_size, char name[], char val[], int size)
{
	int i,j=0;
	char tmp_name[32];
	char tmp_val[32];
	int state = 0;

	LOGD("parse_info name:%s", name);

	for(i=0; i<raw_size; i++) {
		switch(state) {
		case 0: // Init
			if(raw_data[i] == '[') {
				j=0;
				state = 1;
			}
			break;

		case 1: // Read name
			if(raw_data[i] == ']') {
				state =2; // name parse done
				tmp_name[j] = '\0';
				j = 0;
			} else if((raw_data[i] == '\r')||(raw_data[i] == '\n')) {
				j = 0;
				state = 0;
			} else {
				tmp_name[j] = raw_data[i];
				j++;
			}
			break;

		case 2: // Get ':'
			if(raw_data[i] == ':') {
				state = 3; //divider found
				tmp_val[0] = 0;
			} else
				state = 0; // Invalid format
			break;

		case 3: // Read value
			if((raw_data[i] == '\r')||(raw_data[i] == '\n')) {
				state =4; // value parse done
				tmp_val[j]='\0';
				j = 0;
			} else {
				tmp_val[j] = raw_data[i];
				j++;
				break;
			}

		case 4: // Check if name is match
			if(strcmp(tmp_name, name)==0) {
				// Copy value
				snprintf(val, size, "%s", tmp_val);
				return 0;
			}
			state = 0;
			break;
		default:
			break;
		}
	}
	return -1;
}

int query_kcfg_setting(char name[], char val[], int size)
{
	char *raw_buf;
	int fd;
	int ret;

	fd = open("/sys/kernel/ccci/kcfg_setting", O_RDONLY);
	if(fd < 0) {
		LOGE("open sys file fail(%d)", errno);
		return -1;
	}
	raw_buf = (char*)malloc(4096);
	if(NULL == raw_buf) {
		LOGE("allock memory fail");
		close(fd);
		return -2;
	}
	ret = read(fd, raw_buf, 4096);
	if(ret <= 0) {
		LOGE("read info fail ret%d(%d)", ret, errno);
		ret = -3;
		goto _Exit;
	}
	LOGD("read info:%s", raw_buf);

	//------------------------------------------------
	ret = parse_info(raw_buf, ret, name, val, size);
	//------------------------------------------------

_Exit:
	free(raw_buf);
	close(fd);

	return ret;
}

int query_prj_cfg_setting(char name[], char val[], int size)
{
	char prop_value[PROPERTY_VALUE_MAX] = {0};

	//-- For MTK_TELEPHONY_BOOTUP_MODE_SLOT1
	#ifdef MTK_TELEPHONY_BOOTUP_MODE_SLOT1
	if(strcmp(name, "BOOTUP_MODE_SLOT1")==0) {
		snprintf(val, size, "%s", MTK_TELEPHONY_BOOTUP_MODE_SLOT1);
		return 0;
	}
	#endif

	//-- For MTK_TELEPHONY_BOOTUP_MODE_SLOT2
	#ifdef MTK_TELEPHONY_BOOTUP_MODE_SLOT2
	if(strcmp(name, "BOOTUP_MODE_SLOT2")==0) {
		snprintf(val, size, "%s", MTK_TELEPHONY_BOOTUP_MODE_SLOT2);
		return 0;
	}
	#endif

	//-- For MTK_IMS_SUPPORT
	if(strcmp(name, "MTK_IMS_SUPPORT")==0) {
		property_get("ro.mtk_ims_support", prop_value, "");
		if(strcmp(prop_value, "1")==0) {
			snprintf(val, size, "1");
			return 0;
		}
	}

	//-- For MTK_VOLTE_SUPPORT
	if(strcmp(name, "MTK_VOLTE_SUPPORT")==0) {
		property_get("ro.mtk_volte_support", prop_value, "");
		if(strcmp(prop_value, "1")==0) {
			snprintf(val, size, "1");
			return 0;
		}
	}
	
	//-- For MTK_ECCCI_C2K
	#ifdef MTK_ECCCI_C2K
	if(strcmp(name, "MTK_ECCCI_C2K")==0) {
		snprintf(val, size, "1");
		return 0;
	}
	#endif
	
	//-- For MTK_CCCI_PERMISSION_CHECK_SUPPORT
	#ifdef MTK_CCCI_PERMISSION_CHECK_SUPPORT
	if(strcmp(name, "MTK_CCCI_PERMISSION_CHECK_SUPPORT")==0) {
		snprintf(val, size, "1");
		return 0;
	}
	#endif
	return -1;
}

/************ for users who control share memory by themslves **********/
int ccci_smem_get(CCCI_MD md_id, CCCI_USER user_id, unsigned char **address, unsigned int *length)
{
	char dev_port[32];
	int fd, ret;
	unsigned int addr, len;

	snprintf(dev_port, 32, "%s", ccci_get_node_name(user_id, md_id));
	fd = open(dev_port, O_RDWR);
	if (fd < 0) {
		LOGE("open %s failed, errno=%d, user%d, md%d", dev_port, errno, user_id, md_id);
		return -ENODEV;
	}

	ret = ioctl(fd, CCCI_IOC_SMEM_BASE, &addr);
	if (ret) {
		LOGE("CCCI_IOC_SMEM_BASE fail on %s, err=%d\n", dev_port, errno);
		close(fd);
		return ret;
	}

	ret = ioctl(fd, CCCI_IOC_SMEM_LEN, &len);
	if (ret) {
		LOGE("CCCI_IOC_SMEM_LEN fail on %s, err=%d\n", dev_port, errno);
		close(fd);
		return ret;
	}
	LOGD("mmap on %s(%d) for addr=0x%x, len=%d\n", dev_port, fd, addr, len);
	*address = (void *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	*length = len;
	if (*address == MAP_FAILED) {
		LOGE("mmap on %s failed, %d\n", dev_port, errno);
		close(fd);
		return -EFAULT;
	}
	return fd;
}

int ccci_smem_put(int fd, unsigned char *address, unsigned int length)
{
	close(fd);
	LOGD("munmap on (%d) for addr=%p, len=%d\n", fd, address, length);
	return munmap(address, length);
}

/************ for md_init to check share memory **********/

static struct ccci_ccb_config ccci_ccb_ports[] = {
	//{USR_SMEM_CCB_DHL, P_CORE, 1024, 512, 4096, 1024},
	{USR_SMEM_CCB_DHL, P_CORE, (128*1024+8), 512, ((128*1024+8)*32), 1024},
	{USR_SMEM_CCB_DHL, P_CORE, 2048, 128, 16384, 1024},
};

int ccci_ccb_get_config(CCCI_MD md_id, CCCI_USER user_id, unsigned int buffer_id, struct ccci_ccb_size *config)
{	
	unsigned int i, count;

	if (md_id != MD_SYS1 || !config)
		return -EINVAL;
	count = 0;
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		if (count++ == buffer_id) {
			config->ccb_page_header_size = 8; // FIXME, hardcode
			config->dl_page_size = ccci_ccb_ports[i].dl_page_size;
			config->ul_page_size = ccci_ccb_ports[i].ul_page_size;
			config->dl_buff_size = ccci_ccb_ports[i].dl_buff_size;
			config->ul_buff_size = ccci_ccb_ports[i].ul_buff_size;
			return 0;
		}
	}
	return -EFAULT;
}

static int ccci_ccb_init_user(CCCI_MD md_id, CCCI_USER user_id)
{
	int fd;
	unsigned char *base_address, *ptr;
	unsigned int length, i, data;
	unsigned int ret = 0;
	unsigned int request = 0;
	unsigned int *guard_band;
	struct buffer_header *header_ptr;

	if (md_id != MD_SYS1)
		return -EINVAL;

	fd = ccci_smem_get(md_id, user_id, &base_address, &length);
	if (fd < 0)
		return -ENODEV;
	base_address += 4;
	length -= 4;

	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		request += ccci_ccb_ports[i].dl_buff_size;
		request += ccci_ccb_ports[i].ul_buff_size;
	}
	LOGD("mmap on user%d md%d for addr=%p, len=%d, request=%d\n", user_id, md_id, base_address, length, request);
	if (request > length) {
		ret = -EFAULT;
		goto exit;
	}

	ptr = base_address;
	// downlink
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		LOGD("uer %d DL, slot %d, address=%p (%d, %d)\n", user_id, i, ptr,
				ccci_ccb_ports[i].dl_page_size,
				ccci_ccb_ports[i].dl_buff_size);
		header_ptr = (struct buffer_header *)ptr;
		ptr += sizeof(struct buffer_header);
		header_ptr->page_size = ccci_ccb_ports[i].dl_page_size;
		header_ptr->data_buffer_size = ccci_ccb_ports[i].dl_buff_size;
		ptr += ccci_ccb_ports[i].dl_buff_size;
		guard_band = (unsigned int *)ptr;
		*guard_band = TAIL_MAGIC_BEFORE;
		ptr += sizeof(unsigned int);
	}
	// uplink
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		LOGD("uer %d UL, slot %d, address=%p (%d, %d)\n", user_id, i, ptr,
				ccci_ccb_ports[i].ul_page_size,
				ccci_ccb_ports[i].ul_buff_size);
		header_ptr = (struct buffer_header *)ptr;
		ptr += sizeof(struct buffer_header);
		header_ptr->page_size = ccci_ccb_ports[i].ul_page_size;
		header_ptr->data_buffer_size = ccci_ccb_ports[i].ul_buff_size;
		ptr += ccci_ccb_ports[i].ul_buff_size;
		guard_band = (unsigned int *)ptr;
		*guard_band = TAIL_MAGIC_BEFORE;
		ptr += sizeof(unsigned int);
	}
exit:
	data = ret ? CCB_USER_ERR : CCB_USER_OK;
	ioctl(fd, CCCI_IOC_SMEM_SET_STATE, &data);
	ccci_smem_put(fd, base_address - 4, length + 4);
	LOGD("init user%d md%d ret=%d", user_id, md_id, ret);
	return ret;
}

static int ccci_ccb_check_user(CCCI_MD md_id, CCCI_USER user_id)
{
	int fd;
	unsigned char *base_address, *ptr;
	unsigned int length, i, data;
	unsigned int ret = 0;
	struct buffer_header *header_ptr;

	if (md_id != MD_SYS1)
		return -EINVAL;

	fd = ccci_smem_get(md_id, user_id, &base_address, &length);
	if (fd < 0)
		return -ENODEV;
	base_address += 4;
	length -= 4;
	LOGD("mmap on user%d md%d to addr=%p, len=%d\n", user_id, md_id, base_address, length);

	ioctl(fd, CCCI_IOC_SMEM_GET_STATE, &data);
	if (data != CCB_USER_OK) {
		LOGE("check user %d state wrong %d\n", user_id, data);
		return -EINVAL;
	}

	ptr = base_address;
	// downlink
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		header_ptr = (struct buffer_header *)ptr;
		ptr += sizeof(struct buffer_header);
		if (header_ptr->page_size != ccci_ccb_ports[i].dl_page_size ||
			header_ptr->data_buffer_size != ccci_ccb_ports[i].dl_buff_size) {
			LOGE("AP/MD DL setting diff: AP (%d %d); MD (%d %d)\n",
				ccci_ccb_ports[i].dl_page_size, ccci_ccb_ports[i].dl_buff_size,
				header_ptr->page_size, header_ptr->data_buffer_size);
			ret = -EINVAL;
			goto exit;
		}
		ptr += ccci_ccb_ports[i].dl_buff_size;
		ptr += sizeof(unsigned int);
	}
	// uplink
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		header_ptr = (struct buffer_header *)ptr;
		ptr += sizeof(struct buffer_header);
		if (header_ptr->page_size != ccci_ccb_ports[i].ul_page_size ||
			header_ptr->data_buffer_size != ccci_ccb_ports[i].ul_buff_size) {
			LOGE("AP/MD UL setting diff: AP (%d %d); MD (%d %d)\n",
				ccci_ccb_ports[i].ul_page_size, ccci_ccb_ports[i].ul_buff_size,
				header_ptr->page_size, header_ptr->data_buffer_size);
			ret = -EINVAL;
			goto exit;
		}
		ptr += ccci_ccb_ports[i].ul_buff_size;
		ptr += sizeof(unsigned int);
	}
exit:
	data = ret ? CCB_USER_ERR : CCB_USER_OK;
	ioctl(fd, CCCI_IOC_SMEM_SET_STATE, &data);
	ccci_smem_put(fd, base_address - 4, length + 4);
	LOGD("check user%d md%d ret=%d", user_id, md_id, ret);
	return ret;
}

int ccci_ccb_init_users(int md_id)
{
	ccci_ccb_init_user(md_id, USR_SMEM_CCB_DHL);
	return 0;
}

int ccci_ccb_check_users(int md_id)
{
	ccci_ccb_check_user(md_id, USR_SMEM_CCB_DHL);
	return 0;
}

static struct ccci_ccb_config *ccci_ccb_query_config(unsigned int user_id, unsigned int buffer_id)
{
	unsigned int i;
	unsigned int buffer_count = 0;

	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id == user_id) {
			if (buffer_count == buffer_id)
				return &ccci_ccb_ports[i];
			buffer_count++;
		}
	}
	return NULL;
}

/************ for users who use share memory through CCB API **********/
static struct ccci_ccb_control_user *user;

/*
 * return value: <0: error; >0: buffer slot number; =0: not possible
 * register/unregiser must be paired
 */
int ccci_ccb_register(CCCI_MD md_id, CCCI_USER user_id)
{
	unsigned int i;
	unsigned char *ptr;
	unsigned int *guard_band;
	unsigned int data = 0xFF;
	struct ccci_ccb_control_buff *buff;
	
	if (md_id != MD_SYS1)
		return -EINVAL;
	if (user) {
		LOGE("user%d md%d already registered\n", user_id, md_id);
		return -EFAULT;
	}
	user = malloc(sizeof(struct ccci_ccb_control_user));
	if (!user) {
		LOGE("alloc user%d md%d struct fail\n", user_id, md_id);
		return -EFAULT;
	}
	memset(user, 0, sizeof(struct ccci_ccb_control_user));
	user->fd = ccci_smem_get(md_id, user_id, &user->base_address, &user->total_length);
	if (user->fd < 0) {
		free(user);
		return -ENODEV;
	}
	user->base_address += 4;
	user->total_length -= 4;

	LOGD("register user%d md%d: base=%p, len=%d\n", user_id, md_id,
		user->base_address, user->total_length);
	ioctl(user->fd, CCCI_IOC_SMEM_GET_STATE, &data);
	if (data != CCB_USER_OK) {
		free(user);
		LOGE("register user %d state wrong %d\n", user_id, data);
		return -EINVAL;
	}
		
	for (i = 0; i < sizeof(ccci_ccb_ports) / sizeof(struct ccci_ccb_config); i++) {
		if (ccci_ccb_ports[i].user_id != user_id)
			continue;
		user->buffer_num++;
	}
	if (!user->buffer_num) {
		free(user);
		LOGE("user %d of MD %d has no smem info\n", user_id, md_id);
		return -EINVAL;
	}
	user->buffers = malloc(sizeof(struct ccci_ccb_control_buff) * user->buffer_num);
	if (!user->buffers) {
		free(user);
		LOGE("alloc user%d md%d buffer struct fail\n", user_id, md_id);
		return -EFAULT;
	}
	memset(user->buffers, 0, sizeof(struct ccci_ccb_control_buff) * user->buffer_num);
	
	user->user_id = user_id;
	ptr = user->base_address;
	for (i = 0; i < user->buffer_num; i++) {
		buff = user->buffers + i;
		buff->dl_header = (struct buffer_header *)ptr;
		buff->dl_header->free_index = buff->dl_header->read_index;
		ptr += sizeof(struct buffer_header) + buff->dl_header->data_buffer_size;
		if (buff->dl_header->guard_band == 0) {
			LOGE("register: DL buffer %d pattern wrong\n", i);
			free(user->buffers);
			free(user);
			return -EFAULT;
		}
		LOGD("register user%d md%d DL%d, pattern=%x\n", user_id, md_id, i, buff->dl_header->guard_band);
		buff->dl_page_num = buff->dl_header->data_buffer_size / buff->dl_header->page_size;
		guard_band = (unsigned int *)ptr;
		*guard_band = TAIL_MAGIC_AFTER;
		buff->dl_guard_band_e = guard_band;
		ptr += sizeof(unsigned int);
	}	
	for (i = 0; i < user->buffer_num; i++) {
		buff = user->buffers + i;
		buff->ul_header = (struct buffer_header *)ptr;
		buff->ul_header->write_index = buff->ul_header->allocate_index;
		ptr += sizeof(struct buffer_header) + buff->ul_header->data_buffer_size;
		if (buff->ul_header->guard_band == 0) {
			LOGE("register: UL buffer %d pattern wrong\n", i);
			free(user->buffers);
			free(user);
			return -EFAULT;
		}
		LOGD("register user%d md%d UL%d, pattern=%x\n", user_id, md_id, i, buff->ul_header->guard_band);
		buff->ul_page_num = buff->ul_header->data_buffer_size / buff->ul_header->page_size;
		guard_band = (unsigned int *)ptr;
		*guard_band = TAIL_MAGIC_AFTER;
		buff->ul_guard_band_e = guard_band;
		ptr += sizeof(unsigned int);
	}
	return user->buffer_num;
}

int ccci_ccb_unregister()
{
	unsigned int i;
	int ret;
	struct ccci_ccb_control_buff *buff;

	if (!user)
		return -EFAULT;
	for (i = 0; i < user->buffer_num; i++) {
		buff = user->buffers + i;
		*(buff->dl_guard_band_e) = TAIL_MAGIC_BEFORE;
		*(buff->ul_guard_band_e) = TAIL_MAGIC_BEFORE;
	}
	ret = ccci_smem_put(user->fd, user->base_address - 4, user->total_length + 4);
	free(user->buffers);
	free(user);
	user = NULL;
	return ret;
}

/*
 * should be called after registered successfully
 */
int ccci_ccb_query_status()
{
	unsigned int i;
	int ret = CCB_USER_OK;
	struct ccci_ccb_control_buff *buff;

	if (!user)
		return -EFAULT;
	for (i = 0; i < user->buffer_num; i++) {
		buff = user->buffers + i;
		if (buff->dl_header->guard_band != HEADER_MAGIC_AFTER) {
			LOGD("user%d DL %d not ready\n", user->user_id, i);
			ret = CCB_USER_ERR;
			break;
		}
		if (buff->ul_header->guard_band != HEADER_MAGIC_AFTER) {
			LOGD("user%d UL %d not ready\n", user->user_id, i);
			ret = CCB_USER_ERR;
			break;
		}
	}

	return ret == CCB_USER_OK ? 0 : -EAGAIN;
}

static inline struct page_header *ccci_ccb_get_page(struct buffer_header *buff_h, unsigned int index)
{
	unsigned char *ptr = (unsigned char *)buff_h->pages;
	ptr += buff_h->page_size * index;
	return  (struct page_header *)ptr;
}

/*
 * buffer ID is private to each user, and numbered from 0
 * return value: =NULL: error; !=NULL: valid address to write data
 */
unsigned char *ccci_ccb_write_alloc(unsigned int buffer_id)
{
	struct ccci_ccb_control_buff *buff;
	struct page_header *page = NULL;
	unsigned int available, total;

	if (!user)
		return NULL;
	if (buffer_id > user->buffer_num)
		return NULL;
	buff = user->buffers + buffer_id;
	
	if (buff->ul_header->guard_band != HEADER_MAGIC_AFTER) {
		LOGE("write_alloc of user%d buffer%d: MD not ready on UL, pattern=0x%0x\n",
			user->user_id, buffer_id, buff->ul_header->guard_band);
		return NULL;
	}

	total = buff->ul_page_num;
	if (buff->ul_header->allocate_index == buff->ul_header->free_index) {
		available = total - 1;
	} else if (buff->ul_header->allocate_index > buff->ul_header->free_index) {
		available = total - buff->ul_header->allocate_index + buff->ul_header->free_index - 1;
	} else {
		available = buff->ul_header->free_index - buff->ul_header->allocate_index - 1;
	}
	if (available) {
		page = ccci_ccb_get_page(buff->ul_header, buff->ul_header->allocate_index);
		page->page_status = PAGE_STATUS_ALLOC;
		buff->ul_header->allocate_index++;
		if(buff->ul_header->allocate_index >= total)
			buff->ul_header->allocate_index = 0;
	}
	LOGD("write alloc of user%d buffer%d alloc=%d free=%d\n", user->user_id, buffer_id,
		buff->ul_header->allocate_index, buff->ul_header->free_index);
	if (page)
		return page->buffer;
	else
		return NULL;
}

int ccci_ccb_write_done(unsigned int buffer_id, unsigned char *address, unsigned int length)
{
	struct ccci_ccb_control_buff *buff;
	struct page_header *page = NULL;
	struct ccci_ccb_config *config;
	int ret;
	unsigned int data, i;
	
	if (!user)
		return -EFAULT;
	if (buffer_id > user->buffer_num)
		return -EINVAL;
	buff = user->buffers + buffer_id;

	if (buff->ul_header->guard_band != HEADER_MAGIC_AFTER) {
		LOGE("write_done of user%d buffer%d: MD not ready on UL, pattern=0x%0x\n",
			user->user_id, buffer_id, buff->ul_header->guard_band);
		return -EINVAL;
	}
	if (length > buff->ul_header->page_size) {
		LOGE("write_done of user%d buffer%d: invalid length=%d\n", user->user_id, buffer_id, length);
		return -EINVAL;
	}

	// search for the page
	for (i = 0; i < buff->ul_page_num; i++) {
		page = ccci_ccb_get_page(buff->ul_header, i);
		if (page->buffer == address)
			break;
	}
	if (!page) {
		LOGE("write done of user%d buffer%d: invalid address=%p\n", user->user_id, buffer_id, address);
		return -EINVAL;
	}
	if (page->page_status != PAGE_STATUS_ALLOC) {
		LOGE("write done of user%d buffer%d: invalid page%d, address=%p, status=%d\n",
			user->user_id, buffer_id, i, address, page->page_status);
		return -EINVAL;
	}
	page->page_status = PAGE_STATUS_WRITE_DONE;
	page->valid_length = length;
	// update write index
	i = buff->ul_header->write_index;
	while (i != buff->ul_header->allocate_index) {
		page = ccci_ccb_get_page(buff->ul_header, i);
		if (page->page_status == PAGE_STATUS_WRITE_DONE) {
			buff->ul_header->write_index++;
			if(buff->ul_header->write_index >= buff->ul_page_num)
				buff->ul_header->write_index = 0;
		} else {
			break;
		}
		i++;
		if (i >= buff->ul_page_num)
			i = 0;
	}
	LOGD("write done of user%d buffer%d: OK, i=%d write=%d\n",
		user->user_id, buffer_id, i, buff->ul_header->write_index);
	// send tx notify
	config = ccci_ccb_query_config(user->user_id, buffer_id);
	data = config->core_id;
	ret = ioctl(user->fd, CCCI_IOC_SMEM_TX_NOTIFY, &data);
	return ret;
}

static inline unsigned int ccci_ccb_read_available(struct ccci_ccb_control_buff *buff)
{
	int available = buff->dl_header->write_index -  buff->dl_header->read_index;
	if (available < 0)
		available += buff->dl_page_num;
	return available;
}

/*
 * input is bitmask of all buffer slots which this user has
 * return value: <0: error; >0: bitmask of availbe buffer slots; =0: nothing new
 */
int ccci_ccb_poll(unsigned int bitmask)
{
	unsigned int i, available;
	unsigned int ret = 0;
	struct ccci_ccb_control_buff *buff;
retry:	
	if (!user)
		return -EFAULT;

	for (i = 0; i < user->buffer_num; i++) {
		if (bitmask & (1 << i)) {
			buff = user->buffers + i;
			if (ccci_ccb_read_available(buff))
				ret |= (1 << i);
		}
	}

	LOGD("poll on user%d ret=%d\n", user->user_id, ret);
	if (!ret) {
		ioctl(user->fd, CCCI_IOC_SMEM_RX_POLL, NULL);
		goto retry;
	}
	return ret;
}

int ccci_ccb_read_get(unsigned int buffer_id, unsigned char **address, unsigned int *length)
{
	struct ccci_ccb_control_buff *buff;
	struct page_header *page;
	unsigned int available;

	if (!user)
		return -EFAULT;
	if (buffer_id > user->buffer_num)
		return -EINVAL;
	buff = user->buffers + buffer_id;

	available = ccci_ccb_read_available(buff);
	if (available) {
		page = ccci_ccb_get_page(buff->dl_header, buff->dl_header->read_index);
		if (page->page_status != PAGE_STATUS_WRITE_DONE) {
			LOGE("read get of user%d buffer%d: invalid status=%d, read=%d\n", user->user_id, buffer_id,
				page->page_status, buff->dl_header->read_index);
			return -EFAULT;
		}
		*address = page->buffer;
		*length = page->valid_length;
		LOGD("read get of user%d buffer%d: read=%d\n", user->user_id, buffer_id, buff->dl_header->read_index);
		buff->dl_header->read_index++;
		if(buff->dl_header->read_index >= buff->dl_page_num)
			buff->dl_header->read_index = 0;
	} else {
		*address = NULL;
		*length = 0;
		return -EAGAIN;
	}
	return 0;
}

int ccci_ccb_read_done(unsigned int buffer_id)
{
	struct ccci_ccb_control_buff *buff;
	unsigned int free_index_tobe;

	if (!user)
		return -EFAULT;
	if (buffer_id > user->buffer_num)
		return -EINVAL;
	buff = user->buffers + buffer_id;

	free_index_tobe = buff->dl_header->free_index + 1;
	if (free_index_tobe >= buff->dl_page_num)
		free_index_tobe = 0;
	if (buff->dl_header->read_index != free_index_tobe) {
		LOGE("read done of user%d buffer%d: invalid index, read=%d, free=%d\n", user->user_id, buffer_id,
				buff->dl_header->read_index, buff->dl_header->free_index);
		return -EFAULT;
	}
	LOGD("read done of user%d buffer%d: OK free=%d\n", user->user_id, buffer_id, buff->dl_header->free_index);
 	buff->dl_header->free_index = free_index_tobe;
 	return 0;
}

