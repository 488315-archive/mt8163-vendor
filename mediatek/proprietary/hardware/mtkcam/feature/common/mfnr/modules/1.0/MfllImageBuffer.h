#ifndef __MfllImageBuffer_H__
#define __MfllImageBuffer_H__

#include "IMfllImageBuffer.h"

/* mtkcam related headers */
#include <IImageBuffer.h>

#include <utils/RWLock.h> // android::RWLock

#include <string>

using NSCam::IImageBuffer;
using NSCam::IImageBufferHeap;
using android::sp;
using android::RWLock;
using std::string;

namespace mfll {

class MfllImageBuffer : public IMfllImageBuffer {
public:
    MfllImageBuffer(const char *bufferName = "");
protected:
    virtual ~MfllImageBuffer(void);

/* Image information */
private:
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_alignedWidth;
    unsigned int m_alignedHeight;
    unsigned int m_bufferSize;
    enum ImageFormat m_format;
    sp<IMfllEvents> m_spEventDispatcher;
    IMfllCore *m_pCore;

private:
    /* saves alias image buffer */
    IImageBuffer *m_imgBuffer;
    IImageBufferHeap *m_imgHeap; // Notice, ion fd is related with heap

    sp<IImageBuffer> m_imgChunk; // saves blob chunk
    sp<IImageBuffer> m_imgOwnFromOther; // contains ownership is not belong to MfllImageBuffer
    bool m_bHasOwnership;
    std::string m_bufferName;

/* thread-safe mutex */
private:
    mutable RWLock m_mutex;

/* inlines function */
public:
    virtual enum MfllErr setResolution(unsigned int w, unsigned int h);
    virtual enum MfllErr setAligned(unsigned int aligned_w, unsigned int aligned_h);
    virtual enum MfllErr setImageFormat(enum ImageFormat f);
    virtual unsigned int getWidth(void) const { return m_width; }
    virtual unsigned int getHeight(void) const { return m_height; }
    virtual enum MfllErr getResolution(unsigned int &w, unsigned int &h);
    virtual enum MfllErr getAligned(unsigned int &w, unsigned int &h);
    virtual enum ImageFormat getImageFormat(void);

/* implementations */
public:
    virtual enum MfllErr initBuffer(void);
    virtual bool isInited(void);
    virtual enum MfllErr convertImageFormat(const enum ImageFormat &f);
    virtual enum MfllErr setImageBuffer(void *lpBuffer);
    virtual void* getVa(void);
    virtual size_t getRealBufferSize(void);
    virtual IImageBuffer *getImageBuffer(void);
    virtual enum MfllErr releaseBuffer(void);
    virtual enum MfllErr registerEventDispatcher(const sp<IMfllEvents> &e);
    virtual enum MfllErr saveFile(const char *name);
    virtual enum MfllErr loadFile(const char *name);

    virtual void setName(const char *name)
    { m_bufferName = name; }

    virtual enum MfllErr setMfllCore(IMfllCore *c)
    { m_pCore = c; return MfllErr_Ok; }
private:

private:
    /**
     *  To create a continuous image buffer, only for YUV2 and YV16.
     */
    enum MfllErr createContinuousImageBuffer(const enum ImageFormat &f, unsigned int &imageSize);
    enum MfllErr convertImageBufferFormat(const enum ImageFormat &f);
    enum MfllErr releaseBufferNoLock(void);

public:
    /* helper function to map mfll::ImageFormat to NSCam::eImgFmt */
    static MUINT32 mapImageFormat(const enum ImageFormat &fmt);
    static enum ImageFormat mapImageFormat(const MUINT32 &fmt);
}; /* MfllImageBuffer */

}; /* namespace mfll */
#endif /* __MfllImageBuffer_H__ */
