/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef _JSON_UTIL_H_
#define _JSON_UTIL_H_

#include <vector>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

#define FACE_TAG    "face_detections"

using namespace rapidjson;

struct FD_DATA_STEREO_T
{
    int left;
    int top;
    int right;
    int bottom;
    int rotation;   //0~11

    FD_DATA_STEREO_T()
        : left(-9999)
        , top(-9999)
        , right(-9999)
        , bottom(-9999)
        , rotation(-1) {}

    FD_DATA_STEREO_T(int l, int t, int r, int b, int rot)
        : left(l)
        , top(t)
        , right(r)
        , bottom(b)
        , rotation(rot) {}
};

class JSON_Util
{
public:
    JSON_Util()
            : _faceResult(NULL)
            , _mergeResult(NULL) {}
    ~JSON_Util() {
        if(_faceResult) {
            delete [] _faceResult;
            _faceResult = NULL;
        }

        if(_mergeResult) {
            delete [] _mergeResult;
            _mergeResult = NULL;
        }
    }

    char *facesToJSON(std::vector<FD_DATA_STEREO_T> faces)
    {
        Document document;
        document.SetObject();
        Document::AllocatorType& allocator = document.GetAllocator();

        Value faceArray(kArrayType);
        for(std::vector<FD_DATA_STEREO_T>::iterator it = faces.begin(); it != faces.end(); ++it) {
            Value face(kObjectType);
            face.AddMember("left",      Value(it->left).Move(), allocator);
            face.AddMember("top",       Value(it->top).Move(), allocator);
            face.AddMember("right",     Value(it->right).Move(), allocator);
            face.AddMember("bottom",    Value(it->bottom).Move(), allocator);
            face.AddMember("rotation-in-plane", Value(it->rotation).Move(), allocator);  //0, 1, ...11

            faceArray.PushBack(face.Move(), allocator);
        }
        document.AddMember(FACE_TAG, faceArray, allocator);

        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.

        const char *jsonString = sb.GetString();

        if(_faceResult) {
            delete [] _faceResult;
            _faceResult = NULL;
        }

        if(jsonString) {
            const int STR_LEN = strlen(jsonString);
            if(STR_LEN > 0) {
                _faceResult = new char[STR_LEN+1];
                strcpy(_faceResult, jsonString);
            }
        }

        return _faceResult;
    }

    char *mergeJSON(const char *jsonStr1, const char *jsonStr2)
    {
        Document document1;
        document1.Parse(jsonStr1);
        assert(document1.IsObject());
        Document::AllocatorType& allocator = document1.GetAllocator();

        Document document2;
        document2.Parse(jsonStr2);
        assert(document2.IsObject());

        assert(document2.HasMember(FACE_TAG));
        if(document2.HasMember(FACE_TAG)) {
            document1.AddMember(FACE_TAG, document2[FACE_TAG], allocator);
        }

        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        document1.Accept(writer);    // Accept() traverses the DOM and generates Handler events.

        const char *jsonString = sb.GetString();
        if(_mergeResult) {
            delete [] _mergeResult;
            _mergeResult = NULL;
        }
        char *result = NULL;
        if(jsonString) {
            const int STR_LEN = strlen(jsonString);
            if(STR_LEN > 0) {
                _mergeResult = new char[STR_LEN+1];
                strcpy(_mergeResult, jsonString);
            }
        }

        return _mergeResult;
    }
private:
    char *_faceResult;
    char *_mergeResult;
};
#endif