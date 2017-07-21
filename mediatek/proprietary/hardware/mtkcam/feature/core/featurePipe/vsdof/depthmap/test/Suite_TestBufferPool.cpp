/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <gtest/gtest.h>

#include "../TuningBufferPool.h"
// TODO: make sure the isp_reg.h header location
#include <ImageFormat.h>
#include <drv/isp_reg.h>

#include <featurePipe/core/include/ImageBufferPool.h>

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

#define ALLOC_COUNT 10
TEST(TuningBufferPool, Alloc)
{
    android::sp<TuningBufferPool> pool;
    SmartTuningBuffer buffer[ALLOC_COUNT];
    int i, available, total;


    total = 0;
    available = 0;

    pool = TuningBufferPool::create("unittest", sizeof(dip_x_reg_t), MFALSE);
    EXPECT_TRUE(pool != NULL);

    for( i = 0; i < ALLOC_COUNT/2; ++i )
    {
    ++total;
    ++available;
    EXPECT_TRUE(pool->allocate());
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);
    }

    buffer[0] = pool->request();
    --available;
    EXPECT_TRUE(buffer[0] != NULL);
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);

    buffer[0] = NULL;
    ++available;
    EXPECT_TRUE(buffer[0] == NULL);
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);

    for( i = 0; i < ALLOC_COUNT/2; ++i )
    {
    buffer[i] = pool->request();
    --available;
    EXPECT_TRUE(buffer[i] != NULL);
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);
    }

    EXPECT_EQ(pool->allocate(2), 2);
    available+=2;
    total+=2;
    while( total < ALLOC_COUNT )
    {
    ++total;
    ++available;
    EXPECT_TRUE(pool->allocate());
    }
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);

    for( i = ALLOC_COUNT/2; i < total; ++i )
    {
    buffer[i] = pool->request();
    --available;
    EXPECT_TRUE(buffer[i] != NULL);
    }
    EXPECT_EQ(pool->peakPoolSize(), total);
    EXPECT_EQ(pool->peakAvailableSize(), available);

    ++total;
    ++available;
    EXPECT_TRUE(pool->allocate());
    // test data member is accessiable which is allocated by pool
    SmartTuningBuffer testBuf = pool->request();
    --available;
    dip_x_reg_t* data = reinterpret_cast<dip_x_reg_t*>(testBuf->mpVA);
    data->DIP_X_ANR_CON2.Raw = 51;
    data->rsv_4490[26] = 6;
    memset((void*)testBuf->mpVA, 0, sizeof(dip_x_reg_t));

    testBuf = NULL;
    ++available;
    EXPECT_EQ(pool->peakAvailableSize(), available);
    for( i = 0; i < ALLOC_COUNT; ++i )
    {
    buffer[i] = NULL;
    ++available;
    }
    EXPECT_EQ(pool->peakAvailableSize(), available);

    TuningBufferPool::destroy(pool);
    EXPECT_TRUE(pool == NULL);

}


}; // namespace NSFeaturePipe
}; // namespace NSCamFeature
}; // namespace NSCam

