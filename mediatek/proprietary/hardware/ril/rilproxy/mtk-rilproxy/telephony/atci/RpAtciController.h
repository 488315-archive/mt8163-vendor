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

#ifndef __RFX_ATCI_CONTROLLER_H__
#define __RFX_ATCI_CONTROLLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RfxController.h"

/*****************************************************************************
 * Class RpAtciController
 *****************************************************************************/
extern "C"
void *sendDataToAtci(void *arg);

class AtciResponseData {
public:
    AtciResponseData() :
    m_atci_socket_fd(-1),
    m_response_data(NULL),
    m_response_data_length(0),
    m_buffer_ready(NULL),
    m_write_mutex(NULL),
    m_write_cond(NULL),
    m_buffer_mutex(NULL),
    m_buffer_cond(NULL) {
    }

    AtciResponseData(int fd, char* data, int length, int* ready,
            pthread_mutex_t* writeMutex, pthread_cond_t* writeCond,
            pthread_mutex_t* bufferMutex, pthread_cond_t* bufferCond) :
    m_atci_socket_fd(fd),
    m_response_data(data),
    m_response_data_length(length),
    m_buffer_ready(ready),
    m_write_mutex(writeMutex),
    m_write_cond(writeCond),
    m_buffer_mutex(bufferMutex),
    m_buffer_cond(bufferCond) {
    }

    ~AtciResponseData() {
    }
    void setData(char *data) {
        m_response_data = data;
    }
    void setDataLength(int len) {
        m_response_data_length = len;
    }
public:
    int m_atci_socket_fd;
    char* m_response_data;
    int m_response_data_length;
    int* m_buffer_ready;
    pthread_mutex_t* m_write_mutex;
    pthread_cond_t* m_write_cond;
    pthread_mutex_t* m_buffer_mutex;
    pthread_cond_t* m_buffer_cond;
};

class RpAtciController : public RfxController {
    RFX_DECLARE_CLASS(RpAtciController); // Required: declare this class

public:
    RpAtciController();

    virtual ~RpAtciController();
    void responseToAtci(char *responseStr, int strLen);

// Override
protected:
    virtual bool onHandleAtciRequest(const sp<RfxMessage>& message);
    virtual bool onHandleAtciResponse(const sp<RfxMessage>& message);
    virtual void onInit();
    virtual bool onHandleRequest(const sp<RfxMessage>& message);
    virtual bool onHandleResponse(const sp<RfxMessage>& message);
    virtual bool onHandleUrc(const sp<RfxMessage>& message);

private:
    friend void *::sendDataToAtci(void *arg);

private:
    pthread_mutex_t writeMutex;
    pthread_cond_t writeCond;
    pthread_mutex_t bufferMutex;
    pthread_cond_t bufferCond;
    int bufferReady;
    AtciResponseData* atciResponseData;

    bool isGsmCard(int cardType);
    bool isCdmaCard(int cardType);
    bool isGsmCdmaCard(int cardType);
    bool containsCdma(int cardType);
    bool containsGsm(int cardType);
    char* strdupReadString(Parcel* p);
    int checkTechGroupByEsuo();
};
#endif /* __RFX_ATCI_CONTROLLER_H__ */

