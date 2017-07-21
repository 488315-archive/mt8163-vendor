#include "MfllImageBuffer.h"
#include "MfllLog.h"

/* mtkcam related headers */
#include <UITypes.h>
#include <ImageFormat.h>
#include <Format.h>
#include <ImageBufferHeap.h>
#include <imagebuf/IIonImageBufferHeap.h>

#include <stdio.h> // FILE

#define BUFFER_USAGE    (eBUFFER_USAGE_SW_READ_OFTEN | \
                         eBUFFER_USAGE_SW_WRITE_OFTEN | \
                         eBUFFER_USAGE_HW_CAMERA_READWRITE)

#define ALIGN(w, a) (((w + (a-1)) / a) * a)

using namespace mfll;
using namespace NSCam;
using namespace NSCam::Utils::Format;
using android::sp;
using NSCam::IImageBuffer;

IMfllImageBuffer* IMfllImageBuffer::createInstance(const char *bufferName /* = "" */)
{
    mfllLogD("Rynn: create instance");
    return (IMfllImageBuffer*)new MfllImageBuffer(bufferName);
}

void IMfllImageBuffer::destroyInstance(void)
{
    decStrong((void*)this);
}

MfllImageBuffer::MfllImageBuffer(const char *bufferName /* = "" */)
{
    m_imgBuffer = NULL;
    m_imgHeap = NULL;
    m_imgChunk = NULL;
    m_spEventDispatcher = NULL;
    m_width = 0;
    m_height = 0;
    m_alignedWidth = 2;
    m_alignedHeight = 2;
    m_bufferSize = 0;
    m_bHasOwnership = true;
    m_bufferName = bufferName;
}

MfllImageBuffer::~MfllImageBuffer(void)
{

    mfllLogD("destroying MfllImageBuffer %s", m_bufferName.c_str());
    releaseBuffer();
}

enum MfllErr MfllImageBuffer::setResolution(unsigned int w, unsigned int h)
{
    enum MfllErr err = MfllErr_Ok;

    RWLock::AutoWLock _l(m_mutex);

    if (m_imgBuffer) {
        mfllLogW("%s: cannot set resolution due to inited", __FUNCTION__);
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_width = w;
    m_height = h;

lbExit:
    return err;
}

enum MfllErr MfllImageBuffer::setAligned(unsigned int aligned_w, unsigned int aligned_h)
{
    enum MfllErr err = MfllErr_Ok;

    RWLock::AutoWLock _l(m_mutex);

    if (m_imgBuffer) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_alignedWidth = aligned_w;
    m_alignedHeight = aligned_h;

lbExit:
    return err;
}

enum MfllErr MfllImageBuffer::setImageFormat(enum ImageFormat f)
{
    enum MfllErr err = MfllErr_Ok;

    RWLock::AutoWLock _l(m_mutex);

    /* If not created by MfllImageBuffer, we cannot convert image format */
    if (m_bHasOwnership == false) {
        err = MfllErr_NotSupported;
        goto lbExit;
    }

    if (m_imgBuffer) {
        mfllLogW("%s: cannot set image format due to inited", __FUNCTION__);
        err = MfllErr_NotSupported;
        goto lbExit;
    }

    m_format = f;

lbExit:
    return err;
}

enum MfllErr MfllImageBuffer::getResolution(unsigned int &w, unsigned int &h)
{
    RWLock::AutoRLock _l(m_mutex);
    w = m_width;
    h = m_height;
    return MfllErr_Ok;
}

enum MfllErr MfllImageBuffer::getAligned(unsigned int &w, unsigned int &h)
{
    RWLock::AutoRLock _l(m_mutex);
    w = m_alignedWidth;
    h = m_alignedHeight;
    return MfllErr_Ok;
}

enum ImageFormat MfllImageBuffer::getImageFormat(void)
{
    RWLock::AutoRLock _l(m_mutex);
    return m_format;
}

enum MfllErr MfllImageBuffer::initBuffer(void)
{
    enum MfllErr err = MfllErr_Ok;
    mfllFunctionIn();

    RWLock::AutoWLock _l(m_mutex);

    if (m_imgBuffer) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    {
        /* create buffer */
        unsigned int imageSize = 0;
        err = createContinuousImageBuffer(m_format, imageSize);
        if (err == MfllErr_Ok)
            m_bufferSize = imageSize;
    }

lbExit:
    if (err != MfllErr_Ok) {
        if (err == MfllErr_Shooted) {
            mfllLogW("%s: Image buffer has been inited", __FUNCTION__);
            err = MfllErr_Ok; // set MfllErr_Ok.
        }
        else
            mfllLogE("%s: create buffer failed with code %d", __FUNCTION__, (int)err);
    }

    mfllFunctionOut();
    return err;
}

bool MfllImageBuffer::isInited(void)
{
    RWLock::AutoRLock _l(m_mutex);
    return m_imgBuffer ? true : false;
}

enum MfllErr MfllImageBuffer::convertImageFormat(const enum ImageFormat &f)
{
    enum MfllErr err = MfllErr_Ok;

    RWLock::AutoWLock _l(m_mutex);

    if (!m_bHasOwnership) {
        mfllLogE("%s: don't own the ownership", __FUNCTION__);
        err = MfllErr_NotSupported;
        goto lbExit;
    }

    if (m_imgBuffer == NULL) {
        mfllLogE("%s: image buffer hasn't been inited", __FUNCTION__);
        err = MfllErr_NullPointer;
        goto lbExit;
    }

    if (m_format == f) {
        mfllLogW("%s: no need to convert, it's already is.", __FUNCTION__);
        goto lbExit;
    }

    err = convertImageBufferFormat(f);
    if (err != MfllErr_Ok) {
        mfllLogE("%s: convert image buffer format failed", __FUNCTION__);
    }

    m_format = f;

lbExit:
    return err;
}

enum MfllErr MfllImageBuffer::setImageBuffer(void *lpBuffer)
{
    enum MfllErr err = MfllErr_Ok;
    IImageBuffer *pBuffer = NULL;

    mfllLogD("setImageBuffer");

    RWLock::AutoRLock _l(m_mutex);

    if (m_imgBuffer) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    pBuffer = reinterpret_cast<IImageBuffer*>(lpBuffer);
    pBuffer->incStrong((void*)pBuffer);

    m_bHasOwnership = false; // Don't have ownership
    m_imgOwnFromOther = pBuffer;
    m_imgBuffer = pBuffer;
    m_width = pBuffer->getImgSize().w;
    m_height = pBuffer->getImgSize().h;
    m_alignedWidth = 1;
    m_alignedHeight = 1;

    /* get image buffer size */
    m_bufferSize = 0;
    for (int i = 0; i < pBuffer->getPlaneCount(); i++)
        m_bufferSize += pBuffer->getBufSizeInBytes(i);

    m_format = mapImageFormat(pBuffer->getImgFormat());

lbExit:

    if (err != MfllErr_Ok) {
        if (err == MfllErr_Shooted) {
            mfllLogW("%s: Image buffer has been inited", __FUNCTION__);
            err = MfllErr_Ok; // set MfllErr_Ok.
        }
        else
            mfllLogE("%s: create buffer failed with code %d", __FUNCTION__, (int)err);
    }

    return MfllErr_Ok;
}

void* MfllImageBuffer::getVa(void)
{
    void *va = NULL;
    mfllFunctionIn();

    RWLock::AutoRLock _l(m_mutex);

    if (m_imgBuffer) {
        va = (void*)(long)m_imgBuffer->getBufVA(0);
    }

    mfllFunctionOut();
    return va;
}

size_t MfllImageBuffer::getRealBufferSize(void)
{
    size_t r = 0;

    RWLock::AutoRLock _l(m_mutex);

    if (m_imgBuffer)
        r = m_bufferSize;

    return r;
}
IImageBuffer* MfllImageBuffer::getImageBuffer(void)
{
    IImageBuffer *addr = NULL;
    RWLock::AutoRLock _l(m_mutex);

    if (m_imgBuffer)
        addr = m_imgBuffer;

    return addr;
}

enum MfllErr MfllImageBuffer::releaseBuffer(void)
{
    enum MfllErr err = MfllErr_Ok;

    mfllFunctionIn();

    RWLock::AutoWLock _l(m_mutex);

    releaseBufferNoLock();

lbExit:
    if (err == MfllErr_NullPointer) {
        mfllLogW("%s: m_imgBuffer is already NULL", __FUNCTION__);
        err = MfllErr_Ok; // set back ok!
    }

    mfllFunctionOut();
    return MfllErr_Ok;
}

enum MfllErr MfllImageBuffer::registerEventDispatcher(const sp<IMfllEvents> &e)
{
    RWLock::AutoWLock _l(m_mutex);
    m_spEventDispatcher = e;
    return MfllErr_Ok;
}

enum MfllErr MfllImageBuffer::saveFile(const char *name)
{
    enum MfllErr err = MfllErr_Ok;
    FILE *fp = NULL;
    size_t fileSize = 0;
    size_t resultSize = 0;
    size_t bufferSize = 0;

    mfllFunctionIn();

    RWLock::AutoWLock _l(m_mutex);

    if (m_imgBuffer == NULL) {
        err = MfllErr_NullPointer;
        goto lbExit;
    }

    /* check file */
    fp = fopen(name, "wb");
    if (fp == NULL) {
        err = MfllErr_BadArgument;
        goto lbExit;
    }

    /* get buffer size */
    bufferSize = getRealBufferSize();

    /* save file */
    {
        void *addr = (void*)(long)m_imgBuffer->getBufVA(0);
        mfllLogD("%s: write file with size %d to %s", __FUNCTION__, bufferSize, name);
        fwrite(addr, 1L, bufferSize, fp); // write to file.
    }

lbExit:
    if (fp)
        fclose(fp);

    if (err == MfllErr_NullPointer) {
        mfllLogE("%s: can't save file with NULL pointer", __FUNCTION__);
    }
    else if (err == MfllErr_UnexpectedError) {
        mfllLogE("%s: save file failed with unexpected fail", __FUNCTION__);
    }

    mfllFunctionOut();

    return err;
}

enum MfllErr MfllImageBuffer::loadFile(const char *name)
{
    enum MfllErr err = MfllErr_Ok;
    MBOOL bRet = MTRUE;
    FILE *fp = NULL;
    size_t fileSize = 0;
    size_t resultSize = 0;
    size_t bufferSize = 0;

    RWLock::AutoWLock _l(m_mutex);

    /* check imgBuf */
    if (m_imgBuffer == NULL) {
        err = MfllErr_NullPointer;
        goto lbExit;
    }

    /* check file */
    fp = fopen(name, "rb");
    if (fp == NULL) {
        err = MfllErr_BadArgument;
        goto lbExit;
    }

    /* tell file size */
    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    bufferSize = getRealBufferSize();

    /* check file size with aligned */
    if (bufferSize != fileSize) {
        err = MfllErr_BadArgument;
        goto lbExit;
    }
    {
        void *addr = (void*)(long)m_imgBuffer->getBufVA(0);
        fread(addr, 1L, bufferSize, fp);
    }

lbExit:
    if (fp)
        fclose(fp);

    if (err == MfllErr_NullPointer) {
        mfllLogE("%s: load file failed with NULL image buffer", __FUNCTION__);
    }
    else if (err == MfllErr_BadArgument) {
        if (bufferSize != fileSize)
            mfllLogE("%s: file size doesn't match, bufferSize = %d, fileSize = %d", __FUNCTION__, bufferSize, fileSize);
        else
            mfllLogE("%s: file %s cannot be opened", __FUNCTION__, name);
    }

    return err;
}

static void calculate_stride_offset_size(
    unsigned int width,
    unsigned int height,
    unsigned int alignedw,
    unsigned int alignedh,
    ImageFormat f,
    MUINT32 strideInBytes[], // size should be 3
    MUINT32 addrOffset[], // size should be 3
    MUINT32 &imageSize
)
{
    /* calculate total size, stride, and address of each planes */
    MUINT32 imgFmt = MfllImageBuffer::mapImageFormat(f);
    MUINT32 planeCount = queryPlaneCount(imgFmt);
    imageSize = 0; // in bytes
    for (int i = 0; i < planeCount; i++) {
        unsigned int w = queryPlaneWidthInPixels(imgFmt, i, alignedw);
        unsigned int h = queryPlaneHeightInPixels(imgFmt, i, alignedh);
        unsigned int bitsPerPixel = queryPlaneBitsPerPixel(imgFmt, i);

        strideInBytes[i] = (w * bitsPerPixel) / 8;
        addrOffset[i] = imageSize;

        imageSize += (w * h * bitsPerPixel) / 8; // calcuate in bytes

        mfllLogD("%s: plane %d, stride = %d, addr_offset = %d", __FUNCTION__, i, strideInBytes[i], addrOffset[i]);
    }

    mfllLogD("%s: create buffer with (w,h,alignedw,alignedh,fmt)=(%d,%d,%d,%d,%d)", __FUNCTION__, width, height, alignedw, alignedh, (int)imgFmt);
    mfllLogD("%s: buffer size = %d", __FUNCTION__, imageSize);

}

/**
 *  create alias image buffer, notice that, we need to release IImageBufferHeap,
 *  or it cause fd leak.
 *
 *  @param width            Width of the original image buffer, in pixel
 *  @param height           Height of the original image buffer, in pixel
 *  @param alginedw         Aligned width of the image buffer you wanna create, in pixel
 *  @param alignedh         Aligned height of the image buffer you wanna create, in pixel
 *  @param f                Image format you wanna create
 *  @param startAddr        The start address of the first image plane, if your image 
 *                          format which has more than a plane, the second and the following
 *                          plane address will be startAddr + addrOffset[n]
 *  @param fd               The ION fd of the blob image buffer
 *  @param strideInByte[]   Describe stride in bytes for the planes
 *  @param addrOffset[]     Address offset for every plane, the offset of the first
 *                          plane should be 0.
 *  @param[out] ppHeap      IImageBufferHeap of the IImageBuffer, caller MUST release
 *                          this IImageBufferHeap after used
 */
static IImageBuffer* encapsulate_image_buffer(
    unsigned int width,
    unsigned int height,
    unsigned int alignedw,
    unsigned int alignedh,
    ImageFormat f,
    MUINTPTR startAddr,
    MINT32 fd,
    MUINT32 strideInBytes[], // size should be 3
    MUINT32 addrOffset[], // size should be 3
    IImageBufferHeap **ppHeap // heap that we have to manage it
)
{
    IImageBufferAllocator *allocator = NULL;
    IImageBufferHeap *pHeap = NULL;
    IImageBuffer *pBuf = NULL;
    MUINT32 imgFmt = MfllImageBuffer::mapImageFormat(f);
    MUINT32 planeCount = queryPlaneCount(imgFmt);
    MINT32 bufBoundaryInBytes[3] = {0}; // always be zero

    /**
     *  Notice that, image size should be (width x algined height) due to that
     *  we wanna make vertical padding, the PA (physical address) will be calculated
     *  by BaseImageBuffer following strideInBytes x height
     */
    IImageBufferAllocator::ImgParam extParam(imgFmt, MSize(width, alignedh), strideInBytes, bufBoundaryInBytes, planeCount);

    MINT32      _memID[3];
    MUINTPTR    _virtAddr[3];
    for (int i = 0; i < 3; i++) {
        _memID[i] = fd;
        _virtAddr[i] = startAddr + addrOffset[i];
    }

    if (planeCount > 1) {
        /* non-continuous buffer */
        PortBufInfo_v1 portBufInfo = PortBufInfo_v1(_memID, _virtAddr, planeCount);
        pHeap = ImageBufferHeap::create(MFLL_LOG_KEYWORD, extParam, portBufInfo);
    }
    else {
        /* A plane */
        PortBufInfo_v1 portBufInfo = PortBufInfo_v1(_memID[0], _virtAddr[0]);
        pHeap = ImageBufferHeap::create(MFLL_LOG_KEYWORD, extParam, portBufInfo);
    }

    if (pHeap == NULL) {
        mfllLogE("%s: create heap of blob failed", __FUNCTION__);
        goto lbExit;
    }

    /* release heap first */
    if (*ppHeap) {
        (*ppHeap)->decStrong((void*)*ppHeap);
        *ppHeap = NULL;
    }
    *ppHeap = pHeap;

    /* create image buffer */
    pBuf = pHeap->createImageBuffer();
    if (pBuf == NULL) {
        pHeap->decStrong((void*)pHeap);
        mfllLogE("%s: create image buffer from blob heap fail", __FUNCTION__);
        goto lbExit;
    }
    /* Great! You got an image buffer */

lbExit:
    return pBuf;
}

/**
 *  The idea to create a continuous image bufer is:
 */
enum MfllErr MfllImageBuffer::createContinuousImageBuffer(const enum ImageFormat& f, unsigned int &imageSize)
{
    enum MfllErr err = MfllErr_Ok;
    MUINT32 imgFmt = mapImageFormat(f);
    MUINT32 planeCount = queryPlaneCount(imgFmt);
    MINT32 bufBoundaryInBytes[3] = {0}; // always be zero
    MUINT32 strideInBytes[3] = {0};
    MUINT32 addrOffset[3] = {0};
    MUINT32 width, height, alignedw, alignedh;
    MUINT32 bpp = 0;
    imageSize = 0;
    void *bufferVa = NULL;
    int fd = 0;

    /* check image format if valid*/
    if (imgFmt == 0) {
        mfllLogE("%s: Invalid input image format=%d", __FUNCTION__, (int)f);
        return MfllErr_BadArgument;
    }

    width = m_width;
    height = m_height;
    alignedw = m_alignedWidth;
    alignedh = m_alignedHeight;

    /* algin width and height */
    alignedw = ALIGN(width, alignedw);
    alignedh = ALIGN(height, alignedh);

    /* calculate total size, stride, and address of each planes */
    calculate_stride_offset_size(width, height, alignedw, alignedh, f, strideInBytes, addrOffset, imageSize);

    /* create continuous blob heap and get virtual address */
    {
        sp<IImageBuffer> spTemp = NULL;
        sp<IImageBufferHeap> pHeap = NULL;
        IImageBufferAllocator::ImgParam imgParam(imageSize, 0); // blob buffer.

        pHeap = IIonImageBufferHeap::create(MFLL_LOG_KEYWORD, imgParam);
        if (pHeap == NULL) {
            mfllLogE("%s: create blob buffer heap fail", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        spTemp = pHeap->createImageBuffer();
        spTemp->lockBuf(MFLL_LOG_KEYWORD, BUFFER_USAGE);
        // spTemp->syncCache(eCACHECTRL_INVALID); // hw->cpu
        bufferVa = (void*)(long long)spTemp->getBufVA(0); // get buffer VA1
        fd = spTemp->getFD(); // FD
        m_imgChunk = spTemp;

    }

    /* encapsulate tempBuf into external IImageBuffer */
    {
        IImageBuffer *pBuf = encapsulate_image_buffer(
                width, height, // image resolution
                alignedw, alignedh, // aligned image resolution
                f, // image format 
                (MUINTPTR)(long long)bufferVa, // the start address (VA)
                fd, // ION fd
                strideInBytes, // strideInBytes[3]
                addrOffset, // addrOffset[3]
                &m_imgHeap); // output heap

        if (pBuf == NULL) {
            mfllLogE("%s: create image buffer from blob heap fail", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        /* Great! You got an image buffer */
        m_imgBuffer = pBuf; // hold the buffer
    }

    if (!m_imgBuffer->lockBuf(MFLL_LOG_KEYWORD, BUFFER_USAGE)) {
        mfllLogE("%s: lock buffer fail", __FUNCTION__);
        goto lbExit;
    }

    mfllLogD("%s: allocated buffer %p with size %d", __FUNCTION__, (void*)m_imgBuffer, imageSize);

lbExit:

    return err;
}

enum MfllErr MfllImageBuffer::convertImageBufferFormat(const enum ImageFormat &f)
{
    enum MfllErr err = MfllErr_Ok;
    MBOOL ret = MTRUE;
    MUINT32 imgFmt = mapImageFormat(f);
    MUINT32 planeCount = queryPlaneCount(imgFmt);
    MINT32 bufBoundaryInBytes[3] = {0}; // always be zero
    MUINT32 strideInBytes[3] = {0};
    MUINT32 addrOffset[3] = {0};
    MUINT32 width, height, alignedw, alignedh;
    MUINT32 bpp = 0;
    MUINT32 imageSize = 0;
    void *bufferVa = NULL;
    int fd = 0;

    if (m_imgBuffer == NULL) {
        mfllLogE("%s: m_imgBuffer is NULL", __FUNCTION__);
        err = MfllErr_NullPointer;
        goto lbExit;
    }

    /* only supports YUV2 <--> YV16 */
    if ((f != ImageFormat_Yuy2) && (f != ImageFormat_Yv16)) {
        mfllLogE("%s: Unsupport image format", __FUNCTION__);
        err = MfllErr_BadArgument;
        goto lbExit;
    }
    if ((m_format != ImageFormat_Yuy2) && (m_format != ImageFormat_Yv16)) {
        mfllLogE("%s: The buffer format cannot be converted", __FUNCTION__);
        err = MfllErr_NotSupported;
        goto lbExit;
    }

    /* for thread-safe */
    width = m_width;
    height = m_height;
    alignedw = m_alignedWidth;
    alignedh = m_alignedHeight;

    /* algin width and height */
    alignedw = ALIGN(width, alignedw);
    alignedh = ALIGN(height, alignedh);

    /* calculate total size, stride, and address of each planes */
    calculate_stride_offset_size(width, height, alignedw, alignedh, f, strideInBytes, addrOffset, imageSize);

    /* encapsulate tempBuf into external IImageBuffer */
    {
        IImageBufferHeap *pHeap = NULL;
        IImageBuffer *pBuf = encapsulate_image_buffer(
                width, height, 
                alignedw, alignedh,
                f, 
                m_imgBuffer->getBufVA(0), 
                m_imgBuffer->getFD(), 
                strideInBytes, 
                addrOffset, 
                &pHeap);

        if (pBuf == NULL) {
            mfllLogE("%s: create image buffer from blob heap fail", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        m_imgBuffer->unlockBuf(MFLL_LOG_KEYWORD); // unlock old one.
        m_imgBuffer->decStrong((void*)m_imgBuffer); // release old one
        m_imgBuffer = pBuf; // hold the buffer

        /* Great! You get a new image buffer */
        if (m_imgHeap) { // release heap to avoid fd leak
            m_imgHeap->decStrong((void*)m_imgHeap);
        }
        m_imgHeap = pHeap;
    }

    if (!m_imgBuffer->lockBuf(MFLL_LOG_KEYWORD, BUFFER_USAGE)) {
        mfllLogE("%s: lock buffer fail", __FUNCTION__);
        goto lbExit;
    }

    mfllLogD("%s: allocated buffer %p with size %d", __FUNCTION__, (void*)m_imgBuffer, imageSize);

lbExit:

    return err;
}

enum MfllErr MfllImageBuffer::releaseBufferNoLock(void)
{
    /* If MfllImageBuffer has ownwership, release IImageBuffer here */
    if (m_bHasOwnership) {
        if (m_imgBuffer)
            m_imgBuffer->unlockBuf(MFLL_LOG_KEYWORD);
        if (m_imgChunk.get())
            m_imgChunk->unlockBuf(MFLL_LOG_KEYWORD);
    }
    if (m_imgBuffer) {
        mfllLogD("release alias buffer [+]");
        /* release strong reference count */
        m_imgBuffer->decStrong((void*)m_imgBuffer);
        mfllLogD("release alias buffer [-]");
    }
    if (m_imgHeap) {
        mfllLogD("release alias heap");
        m_imgHeap->decStrong((void*)m_imgHeap);
    }
    

    m_bufferSize = 0;
    m_imgOwnFromOther = NULL;
    m_imgBuffer = NULL;
    m_imgChunk = NULL;
    m_imgHeap = NULL;
    m_bHasOwnership = true;

    return MfllErr_Ok;
}

MUINT32 MfllImageBuffer::mapImageFormat(const enum ImageFormat &fmt)
{
    MUINT32 f = 0;
    switch(fmt) {
    case ImageFormat_Yuy2:
        f = eImgFmt_YUY2;
        break;
    case ImageFormat_Yv16:
        f = eImgFmt_YV16;
        break;
    case ImageFormat_Raw10:
        f = eImgFmt_BAYER10;
        break;
    case ImageFormat_Raw8:
        f = eImgFmt_BAYER8;
        break;
    case ImageFormat_Y8:
        f = eImgFmt_Y8; /* using bayer8 instead */
        break;
    default:
        f = 0xFFFFFFFF;
        mfllLogE("%s: Unsupport format 0x%x", __FUNCTION__, fmt);
        break;
    }
    return f;
}

enum ImageFormat MfllImageBuffer::mapImageFormat(const MUINT32 &fmt)
{
    enum ImageFormat f;
    switch(fmt) {
    case eImgFmt_YUY2:
        f = ImageFormat_Yuy2;
        break;
    case eImgFmt_YV16:
        f = ImageFormat_Yv16;
        break;
    case eImgFmt_BAYER10:
        f = ImageFormat_Raw10;
        break;
    case eImgFmt_BAYER8:
        f = ImageFormat_Raw8;
        break;
    case eImgFmt_Y8:
        f = ImageFormat_Y8;
        break;
    default:
        f = ImageFormat_Size;
        mfllLogE("%s: Unsupport format 0x%x", __FUNCTION__, fmt);
        break;
    }
    return f;
}

