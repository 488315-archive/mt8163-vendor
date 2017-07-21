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

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_HWUTILS_HWTRANSFORM_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_HWUTILS_HWTRANSFORM_H_
//

#include <common.h>


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCamHW {

/******************************************************************************
 * Simplified transform matrix for camera hw module.
 * This simlified transform matrix considers translation and scaling.
 * (rotation is not considered.)
 *  v' = Mv = (TST)*v;
 *  v = [x, y, 1]^T, T: translation, S: scaling
 *
 *  x' = c_00 * x + c_02
 *  y' = c_11 * y + c_12
 ******************************************************************************/
class simplifiedMatrix
{
public:
                        simplifiedMatrix(float c00 = 1.f, float c02 = 0.f, float c11 = 1.f, float c12 = 0.f)
                            : c_00(c00), c_02(c02), c_11(c11), c_12(c12)
                        {}

    MBOOL               getInverse(simplifiedMatrix& inv) const {
                            if( c_00 == 0.f || c_11 == 0.f ) return MFALSE;
                            //
                            inv = simplifiedMatrix(1.f / c_00, -c_02/c_00, 1.f / c_11, -c_12/c_11);
                            return MTRUE;
                        }

    MVOID               transform(NSCam::MPoint const& o, NSCam::MPoint& t) const {
                            t.x = c_00 * o.x + c_02;
                            t.y = c_11 * o.y + c_12;
                        }

    MVOID               transform(NSCam::MSize const& o, NSCam::MSize& t) const {
                            t.w = c_00 * o.w;
                            t.h = c_11 * o.h;
                        }

    MVOID               transform(NSCam::MRect const& o, NSCam::MRect& t) const {
                            transform(o.p, t.p);
                            transform(o.s, t.s);
                        }

    simplifiedMatrix    operator*(const simplifiedMatrix& mat) {
                            return simplifiedMatrix(
                                    this->c_00 * mat.c_00,
                                    this->c_00 * mat.c_02 + this->c_02,
                                    this->c_11 * mat.c_11,
                                    this->c_11 * mat.c_12 + this->c_12
                                    );
                        }

    MVOID               dump(const char* const str) const; //debug

private:
    float c_00;
    float c_02;
    float c_11;
    float c_12;
};
typedef simplifiedMatrix    HwMatrix;


class HwTransHelper
{
public:
                        HwTransHelper(MINT32 const openId);
                        ~HwTransHelper();

public:
    /***************************************************************************
     * get the transform matrix from active array to certain sensor mode
     *
     * @param[in]   sensorMode: the target sensor mode coordinates
     *
     * @param[out]  mat: the transform matrix
     *
     * @return
     *  -MTRUE indicates the matrix is valid
     *  -MFALSE indicates the matrix is not valid
     */
    MBOOL               getMatrixFromActive(MUINT32 const sensorMode, HwMatrix& mat);

    /***************************************************************************
     * get the transform matrix from certain sensor mode to active array
     *
     * @param[in]   sensorMode: the current sensor mode cooridinates to be
     *                          transformed
     *
     * @param[out]  mat: the transform matrix
     *
     * @return
     *  -MTRUE indicates the matrix is valid
     *  -MFALSE indicates the matrix is not valid
     */
    MBOOL               getMatrixToActive(MUINT32 const sensorMode, HwMatrix& mat);

protected:
    MINT32 const        mOpenId;
};


};  //namespace NSCamHW
/******************************************************************************
 *
 ******************************************************************************/

#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_HWUTILS_HWTRANSFORM_H_

