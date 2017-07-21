/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#define MTK_LOG_ENABLE 1
#include "ppl_agent.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <cutils/properties.h>

/* Has 3 type to store the ppl data:
 * 1. /data/nvram/dm/ppl_config:
 *   The origianl type, use nvram partition without write protection
 *   to write/read file.
 * 2. block device supplied by fs_mgr:
 *   For nvram partition with write protection on M MT6755, use this
 *   block to write/read raw data directly.
 * 3. /nvcfg/ppl/ppl_config:
 *   For nvram partition with write protection on & after M MT6797,
 *   use this ext4 partition to write/read file.
 */
#define CONTROL_DATA_ROOT_PATH "/data/nvram/dm/"

#ifdef MTK_WRITE_ON_PROTECTION
#ifndef RAW_DATA_PARTITION
#define CONTROL_DATA_ROOT_PATH "/nvcfg/ppl/"
#endif
#endif

#define CONTROL_DATA_FILE_PATH CONTROL_DATA_ROOT_PATH "ppl_config"

#define FSTAB_PREFIX "/fstab."
#define PPL_MNT_POINT "/ppl"

/* hard limit */
#define MAX_FILE_SIZE (4*1024)
#define BINDER_READ_NO_EXCEPTION (0)

#define STATUS_ENABLED 0x2
#define STATUS_LOCKED 0x4
#define STATUS_SIM_LOCKED 0x8

void PPLAgent::instantiate() {
    while (true) {
        PPLAgent *agent = new PPLAgent();
        status_t ret = defaultServiceManager()->addService(descriptor, agent);
        if (ret == OK) {
            ALOGI("Registered to Service Manager. writeOnProtect=%d, rawDataPart=%d",
                    isSupportWriteOnProtection(), isRawDataPartition());
            return;
        }

        ALOGW("Register FAILED. Retry in 5s.");
        sleep(5);
    }
}

PPLAgent::PPLAgent() {
    ALOGI("PPLAgent created start");
    readFstab();
    rawDataPath = getDataPath();
    ALOGI("PPLAgent created end");
}

status_t BnPPLAgent::onTransact(uint32_t code, const Parcel &data,
        Parcel *reply, uint32_t flags) {

    ALOGI("OnTransact(%u,%u)", code, flags);
    reply->writeInt32(BINDER_READ_NO_EXCEPTION);//used for readException

    switch (code) {
    case TRANSACTION_readControlData: {
        ALOGI("readControlData enter");
        data.enforceInterface(descriptor);
        int size = 0;
        char * ret = readControlData(size);
        if (ret == NULL) {
            reply->writeInt32(-1);
        } else {
            reply->writeInt32(size);
            reply->write(ret, size);
            free(ret);
        }
        ALOGI("readControlData exit");
        return NO_ERROR;
    }
        break;
    case TRANSACTION_writeControlData: {
        ALOGI("writeControlData enter");
        data.enforceInterface(descriptor);
        int len = data.readInt32();
        if (len == -1) { // array is null
            reply->writeInt32(0);
        } else {
            char buff[len];
            data.read(buff, len);
            reply->writeInt32(writeControlData(buff, len));
        }
        ALOGI("writeControlData exit");
        return NO_ERROR;
    }
        break;
    case TRANSACTION_needLock: {
        ALOGI("needLock enter");
        data.enforceInterface(descriptor);
        reply->writeInt32(needLock());
        ALOGI("needLock exit");
        return NO_ERROR;
    }
        break;
    default:
        return BBinder::onTransact(code, data, reply, flags);
    }

    return NO_ERROR;
}

int PPLAgent::isSupportWriteOnProtection() {
    #ifdef MTK_WRITE_ON_PROTECTION
        return 1;
    #endif
    return 0;
}

int PPLAgent::isRawDataPartition() {
    #ifdef RAW_DATA_PARTITION
        return 1;
    #endif
    return 0;
}

char* PPLAgent::readControlData(int & size) {
    ALOGI("readControlData enter");

    int fd = -1;
    char *buff;
    union sizeUnion {
        int size;
        char buf[4];
    } su;

    if (isSupportWriteOnProtection() && isRawDataPartition()) {
        if ((fd = open(rawDataPath, O_RDONLY)) > 0) {
            read(fd, su.buf, 4);
            size = su.size;
            ALOGD("readControlData raw size = %d", size);
            buff = (char *) malloc(size);
            read(fd, buff, size);
            close(fd);
            return buff;
        } else {
            ALOGE("readControlData open raw failed:%d", fd);
            ALOGE("readControlData errno = %s", strerror(errno));
            return NULL;
        }
    }

    if (-1 == (fd = open(CONTROL_DATA_FILE_PATH, O_RDONLY))) {
        ALOGD("open control data file error = %s", strerror(errno));
        return NULL;
    } else {
        // get file size
        struct stat file_stat;
        bzero(&file_stat, sizeof(file_stat));
        if (-1 == stat(CONTROL_DATA_FILE_PATH, &file_stat)) {
            ALOGD("stat control data file error = %s", strerror(errno));
            close(fd);
            return NULL;
        }
        size = file_stat.st_size;
        buff = (char *) malloc(size);
        if (-1 == read(fd, buff, size)) {
            ALOGD("read ControlData error = %s", strerror(errno));
            close(fd);
            return NULL;
        }
        close(fd);
        ALOGI("readControlData from file exit");
        return buff;
    }
}

int PPLAgent::writeControlData(char* data, int size) {
    ALOGD("writeControlData enter. data size = %d", size);
    if (data == NULL || size == 0 || size > MAX_FILE_SIZE) {
        return 0;
    }
    int fd = -1;

    /* write data to raw */
    if (isSupportWriteOnProtection() && isRawDataPartition()) {
        if ((fd = open(rawDataPath, O_WRONLY | O_TRUNC)) >= 0) {
            write(fd, &size , 4);
            write(fd, data, size);
            fsync(fd);
            close(fd);
            ALOGD("writeControlData to raw done");
            //wirte to file for debug
            if ((fd = open(CONTROL_DATA_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0775)) >= 0) {
                write(fd, &size , 4);
                write(fd, data, size);
                fsync(fd);
                close(fd);
                ALOGD("writeControlData to backup file exit");
            }
            return 1;
        } else {
            ALOGE("writeControlData open raw failed:%d", fd);
            ALOGE("writeControlData errno = %s", strerror(errno));
            return 0;
        }
    }

    if (-1 == (fd = open(CONTROL_DATA_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0775))) {
        ALOGD("open control data file error = %s", strerror(errno));
        return 0;
    } else {
        write(fd, data, size);
        fsync(fd);
        close(fd);
        FileOp_BackupToBinRegionForDM();
        ALOGI("writeControlData to file exit");
        return 1;
    }
}

int PPLAgent::needLock() {
    int fd = -1;
    int offset;

    if (isSupportWriteOnProtection() && isRawDataPartition()){
        if (-1 == (fd = open(rawDataPath, O_RDONLY))) {
            ALOGD("open control data raw error = %s", strerror(errno));
            return 0;
        }
        offset = 5; //size offset 4
    } else {
        if (-1 == (fd = open(CONTROL_DATA_FILE_PATH, O_RDONLY))) {
            ALOGD("open control data file error = %s", strerror(errno));
            return 0;
        }
        offset = 1;
    }

    // get ControlData.status which is at the second byte
    if (-1 == lseek(fd, offset, SEEK_SET)) {
        ALOGD("lseek %d byte error!", offset);
        close(fd);
        return 0;
    }
    char cstatus;
    if (-1 == read(fd, &cstatus, 1)) {
        ALOGD("read to get ControlData.status error = %s", strerror(errno));
        close(fd);
        return 0;
    }
    close(fd);

    int istatus = cstatus;
    ALOGD("istatus = %d\n", istatus);
    if ((istatus & STATUS_ENABLED) == STATUS_ENABLED
        && ((istatus & STATUS_LOCKED) == STATUS_LOCKED
        || (istatus & STATUS_SIM_LOCKED) == STATUS_SIM_LOCKED)) {
        return 1;
    } else {
        return 0;
    }
}

int PPLAgent::readFstab() {
    char fstab_filename[PROPERTY_VALUE_MAX + sizeof(FSTAB_PREFIX)];
    char propbuf[PROPERTY_VALUE_MAX];

    ALOGD("readFstab\n");
    property_get("ro.hardware", propbuf, "");
    snprintf(fstab_filename, sizeof(fstab_filename), FSTAB_PREFIX"%s", propbuf);
    ALOGD("fstab_filename = %s\n", fstab_filename);

    fstab = fs_mgr_read_fstab(fstab_filename);
    if (NULL == fstab) {
        ALOGD("failed to open %s\n", fstab_filename);
        return -1;
    }
    return 0;
}

char* PPLAgent::getDataPath() {
    struct fstab_rec *rec = NULL;
    char *source = NULL;

    ALOGD("getDataPath\n");
    rec = fs_mgr_get_entry_for_mount_point(fstab, PPL_MNT_POINT);
    if (NULL == rec) {
        ALOGE("failed to get entry for %s\n", PPL_MNT_POINT);
        return NULL;
    }

    asprintf(&source, "%s", rec->blk_device);
    ALOGD("getDataPath = %s\n", source);
    return source;
}

int PPLAgent::freeFstab() {
    fs_mgr_free_fstab(fstab);
    return 0;
}

int main(int argc, char *argv[]) {
    umask(000);
    ALOGD("CONTROL_DATA_ROOT_PATH = %s", CONTROL_DATA_ROOT_PATH);
    if (-1 == access(CONTROL_DATA_ROOT_PATH, F_OK)) {
        if (-1 == mkdir(CONTROL_DATA_ROOT_PATH, 0775)) {
            ALOGD("make control data path error = %s", strerror(errno));
            return 0;
        }
    }

    PPLAgent::instantiate();
    ProcessState::self()->startThreadPool();
    ALOGI("PPLAgent Service is now ready");
    IPCThreadState::self()->joinThreadPool();
    return 0;
}

