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
 * MediaTek Inc. (C) 2015. All rights reserved.
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
package com.mediatek.camera.util.xmp;

import android.util.Log;

import com.adobe.xmp.XMPException;
import com.adobe.xmp.XMPMeta;
import com.adobe.xmp.XMPMetaFactory;
import com.adobe.xmp.XMPSchemaRegistry;
import com.adobe.xmp.options.PropertyOptions;
import com.adobe.xmp.options.SerializeOptions;
import com.adobe.xmp.properties.XMPProperty;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.io.RandomAccessFile;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

/**
 * XmpInterface.
 *
 */
public class XmpInterface {
    private final static String TAG = "MtkGallery2/Xmp/XmpInterface";

    public static final String MEDIATEK_SEGMENT_NAMESPACE = "http://ns.mediatek.com/segment/";
    public static final String MTK_SEGMENT_PREFIX = "MSegment";
    public static final String SEGMENT_MASK_WIDTH = "SegmentMaskWidth";
    public static final String SEGMENT_MASK_HEIGHT = "SegmentMaskHeight";
    public static final String SEGMENT_X = "SegmentX";
    public static final String SEGMENT_Y = "SegmentY";
    public static final String SEGMENT_LEFT = "SegmentLeft";
    public static final String SEGMENT_TOP = "SegmentTop";
    public static final String SEGMENT_RIGHT = "SegmentRight";
    public static final String SEGMENT_BOTTOM = "SegmentBottom";

    public static final String SEGMENT_FACE_STRUCT_NAME = "FDInfo";
    public static final String SEGMENT_FACE_FIELD_NS = "FD";
    public static final String SEGMENT_FACE_PREFIX = "FD";
    public static final String SEGMENT_FACE_LEFT = "FaceLeft";
    public static final String SEGMENT_FACE_TOP = "FaceTop";
    public static final String SEGMENT_FACE_RIGHT = "FaceRight";
    public static final String SEGMENT_FACE_BOTTOM = "FaceBottom";
    public static final String SEGMENT_FACE_RIP = "FaceRip";
    public static final String SEGMENT_FACE_COUNT = "FaceCount";

    public static final int SOI = 0xFFD8;
    public static final int SOS = 0xFFDA;
    public static final int APP1 = 0xFFE1;
    public static final int APP15 = 0xFFEF;
    public static final int DQT = 0xFFDB;
    public static final int DHT = 0xFFC4;

    public final static int WRITE_XMP_AFTER_SOI = 0;
    public final static int WRITE_XMP_BEFORE_FIRST_APP1 = 1;
    public final static int WRITE_XMP_AFTER_FIRST_APP1 = 2;
    // 10kb
    public final static int FIXED_BUFFER_SIZE = 1024 * 10;

    private static final int TEMP_NUM3 = 3;
    private static final int TEMP_NUM4 = 4;
    private static final int TEMP_NUM7 = 7;
    private static final int TEMP_NUM8 = 8;
    private static final int TEMP_NUM16 = 16;
    private static final int TEMP_NUM24 = 24;
    private static final int TEMP_NUM32 = 32;
    private static final int TEMP_NUMFF = 0xff;

    private ArrayList<Section> mParsedSectionsForGallery;
    private XMPSchemaRegistry mRegister;

    /**
     * ByteArrayInputStreamExt.
     *
     */
    public static class ByteArrayInputStreamExt extends ByteArrayInputStream {
        /**
         * ByteArrayInputStreamExt.
         *
         * @param buf
         *            buffer
         */
        public ByteArrayInputStreamExt(byte[] buf) {
            super(buf);
            Log.d(TAG, "<ByteArrayInputStreamExt> new instance, buf count 0x"
                    + Integer.toHexString(buf.length));
        }

        /**
         * read unsigned shot.
         *
         * @return read result
         */
        public final int readUnsignedShort() {
            int hByte = read();
            int lByte = read();
            return hByte << TEMP_NUM8 | lByte;
        }

        /**
         * high byte first int.
         *
         * @return read result
         */
        public final int readInt() {
            int firstByte = read();
            int secondByte = read();
            int thirdByte = read();
            int forthByte = read();
            return firstByte << TEMP_NUM24 | secondByte << TEMP_NUM16 | thirdByte << TEMP_NUM8
                    | forthByte;
        }

        /**
         * low byte first int.
         *
         * @return read result
         */
        public final int readReverseInt() {
            int forthByte = read();
            int thirdByte = read();
            int secondByte = read();
            int firstByte = read();
            return firstByte << TEMP_NUM24 | secondByte << TEMP_NUM16 | thirdByte << TEMP_NUM8
                    | forthByte;
        }

        /**
         * seek.
         *
         * @param offset
         *            offext
         * @throws IOException
         *             exception
         */
        public void seek(long offset) throws IOException {
            if (offset > count - 1) {
                throw new IOException("offset out of buffer range: offset " + offset
                        + ", buffer count " + count);
            }
            pos = (int) offset;
        }

        /**
         * get file pointer.
         *
         * @return result
         */
        public long getFilePointer() {
            return pos;
        }

        /**
         * read.
         *
         * @param buffer
         *            buffer
         * @return result
         */
        public int read(byte[] buffer) {
            return read(buffer, 0, buffer.length);
        }
    }

    /**
     * ByteArrayOutputStreamExt.
     *
     */
    public static class ByteArrayOutputStreamExt extends ByteArrayOutputStream {
        /**
         * constructor.
         *
         * @param size
         *            size
         */
        public ByteArrayOutputStreamExt(int size) {
            super(size);
        }

        /**
         * write short.
         *
         * @param val
         *            write data
         */
        public final void writeShort(int val) {
            int hByte = val >> TEMP_NUM8;
            int lByte = val & TEMP_NUMFF;
            write(hByte);
            write(lByte);
        }

        /**
         * write int.
         *
         * @param val
         *            data
         */
        public final void writeInt(int val) {
            int firstByte = val >> TEMP_NUM24;
            int secondByte = (val >> TEMP_NUM16) & TEMP_NUMFF;
            int thirdByte = (val >> TEMP_NUM8) & TEMP_NUMFF;
            int forthByte = val & TEMP_NUMFF;
            write(firstByte);
            write(secondByte);
            write(thirdByte);
            write(forthByte);
        }
    }

    /**
     * Section.
     *
     */
    public static class Section {
        // e.g. 0xffe1, exif
        public int mMarker;
        // marker offset from start of file
        public long mOffset;
        // app length, follow spec, include 2 length bytes
        public int mLength;
        public boolean mIsXmpMain;
        public boolean mIsXmpExt;
        public boolean mIsExif;
        public boolean mIsJpsData;
        public boolean mIsJpsMask;
        public boolean mIsDepthData;
        public boolean mIsXmpDepth;
        public boolean mIsSegmentMask;

        /**
         * Section.
         *
         * @param marker
         *            marker
         * @param offset
         *            offset
         * @param length
         *            length
         * @param isXmpMain
         *            isXmpMain
         * @param isXmpExt
         *            isXmpExt
         * @param isExif
         *            isExif
         * @param isJpsData
         *            isJpsData
         * @param isJpsMask
         *            isJpsMask
         * @param isDepthData
         *            isDepthData
         * @param isXmpDepth
         *            isXmpDepth
         * @param isSegmentMask
         *            isSegmentMask
         */
        public Section(int marker, long offset, int length, boolean isXmpMain, boolean isXmpExt,
                boolean isExif, boolean isJpsData, boolean isJpsMask, boolean isDepthData,
                boolean isXmpDepth, boolean isSegmentMask) {
            this.mMarker = marker;
            this.mOffset = offset;
            this.mLength = length;
            this.mIsXmpMain = isXmpMain;
            this.mIsXmpExt = isXmpExt;
            this.mIsExif = isExif;
            this.mIsJpsData = isJpsData;
            this.mIsJpsMask = isJpsMask;
            this.mIsDepthData = isDepthData;
            this.mIsXmpDepth = isXmpDepth;
            this.mIsSegmentMask = isSegmentMask;
        }
    }

    /**
     * XmpInterface.
     *
     * @param register
     *            register
     */
    public XmpInterface(XMPSchemaRegistry register) {
        mRegister = register;
    }

    /**
     * setSectionInfo.
     *
     * @param sections
     *            sections array
     */
    public void setSectionInfo(ArrayList<Section> sections) {
        mParsedSectionsForGallery = sections;
    }

    /**
     * copyFileWithFixBuffer.
     *
     * @param rafIn
     *            file
     * @param rafOut
     *            file
     */
    public void copyFileWithFixBuffer(RandomAccessFile rafIn, RandomAccessFile rafOut) {
        byte[] readBuffer = new byte[FIXED_BUFFER_SIZE];
        int readCount = 0;
        long lastReadPosition = 0;
        try {
            while ((readCount = rafIn.read(readBuffer)) != -1) {
                if (readCount == FIXED_BUFFER_SIZE) {
                    rafOut.write(readBuffer);
                } else {
                    rafOut.write(readBuffer, 0, readCount);
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "<copyFileWithFixBuffer> IOException", e);
        }
    }

    /**
     * getXmpMetaFromFile.
     *
     * @param filePath
     *            file path
     * @return result
     */
    public XMPMeta getXmpMetaFromFile(String filePath) {
        File srcFile = new File(filePath);
        if (!srcFile.exists()) {
            Log.d(TAG, "<getXmpMetaFromFile> " + filePath + " not exists!!!");
            return null;
        }

        RandomAccessFile rafIn = null;
        XMPMeta meta = null;
        try {
            rafIn = new RandomAccessFile(filePath, "r");
            Section sec = null;
            for (int i = 0; i < mParsedSectionsForGallery.size(); i++) {
                sec = mParsedSectionsForGallery.get(i);
                if (sec.mIsXmpMain) {
                    rafIn.seek(sec.mOffset + 2);
                    int len = rafIn.readUnsignedShort() - 2;
                    int xmpLen = len - XmpResource.XMP_HEADER_START.length();
                    byte[] buffer = new byte[xmpLen];
                    rafIn.skipBytes(XmpResource.XMP_HEADER_START.length());
                    rafIn.read(buffer, 0, buffer.length);
                    meta = XMPMetaFactory.parseFromBuffer(buffer);
                    if (meta == null) {
                        Log.d(TAG, "<getXmpMetaFromFile> parsed XMPMeta is null, create one!!!");
                        meta = XMPMetaFactory.create();
                    } else {
                        Log.d(TAG, "<getXmpMetaFromFile> return parsed XMPMeta");
                    }
                    return meta;
                }
            }
            meta = XMPMetaFactory.create();
            Log.d(TAG, "<getXmpMetaFromFile> no xmp main, then create XMPMeta!!!");
            return meta;
        } catch (IOException e) {
            Log.e(TAG, "<getXmpMetaFromFile> IOException ", e);
            return null;
        } catch (XMPException e) {
            Log.e(TAG, "<getXmpMetaFromFile> XMPException ", e);
            return null;
        } finally {
            try {
                if (rafIn != null) {
                    rafIn.close();
                    rafIn = null;
                }
            } catch (IOException e) {
                Log.e(TAG, "<getXmpMetaFromFile> IOException when close ", e);
            }
        }
    }

    /**
     * copyToStreamWithFixBuffer.
     *
     * @param is
     *            is
     * @param os
     *            os
     */
    public void copyToStreamWithFixBuffer(ByteArrayInputStreamExt is, ByteArrayOutputStreamExt os) {
        byte[] readBuffer = new byte[FIXED_BUFFER_SIZE];
        int readCount = 0;
        long lastReadPosition = 0;
        try {
            Log.d(TAG, "<copyToStreamWithFixBuffer> copy remain jpg data begin!!!");
            while ((readCount = is.read(readBuffer)) != -1) {
                if (readCount == FIXED_BUFFER_SIZE) {
                    os.write(readBuffer);
                } else {
                    os.write(readBuffer, 0, readCount);
                }
            }
            Log.d(TAG, "<copyToStreamWithFixBuffer> copy remain jpg data end!!!");
        } catch (IOException e) {
            Log.e(TAG, "<copyToStreamWithFixBuffer> Exception", e);
        }
    }

    /**
     * writeSectionToFile.
     *
     * @param rafIn
     *            rafIn
     * @param rafOut
     *            rafOut
     * @param sec
     *            sec
     */
    public void writeSectionToFile(RandomAccessFile rafIn, RandomAccessFile rafOut, Section sec) {
        try {
            rafOut.writeShort(sec.mMarker);
            rafOut.writeShort(sec.mLength);

            rafIn.seek(sec.mOffset + TEMP_NUM4);
            byte[] buffer = null;
            buffer = new byte[sec.mLength - 2];
            rafIn.read(buffer, 0, buffer.length);
            rafOut.write(buffer);
        } catch (IOException e) {
            Log.e(TAG, "<writeSectionToFile> IOException", e);
        }
    }

    /**
     * writeSectionToStream.
     *
     * @param is
     *            is
     * @param os
     *            os
     * @param sec
     *            sec
     */
    public void writeSectionToStream(ByteArrayInputStreamExt is, ByteArrayOutputStreamExt os,
            Section sec) {
        try {
            Log.d(TAG, "<writeSectionToStream> write section 0x" +
                    Integer.toHexString(sec.mMarker));
            os.writeShort(sec.mMarker);
            os.writeShort(sec.mLength);
            is.seek(sec.mOffset + TEMP_NUM4);
            byte[] buffer = null;
            buffer = new byte[sec.mLength - 2];
            is.read(buffer, 0, buffer.length);
            os.write(buffer);
        } catch (IOException e) {
            Log.e(TAG, "<writeSectionToStream> IOException", e);
        }
    }

    /**
     * registerNamespace.
     *
     * @param nameSpace
     *            namespace
     * @param prefix
     *            prefix
     */
    public void registerNamespace(String nameSpace, String prefix) {
        try {
            mRegister.registerNamespace(nameSpace, prefix);
        } catch (XMPException e) {
            Log.e(TAG, "<registerNamespace> XMPException", e);
        }
    }

    /**
     * setPropertyString.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param propName
     *            propName
     * @param value
     *            value
     */
    public void setPropertyString(XMPMeta meta, String nameSpace, String propName, String value) {
        if (meta == null) {
            Log.d(TAG, "<setPropertyString> meta is null, return!!!");
            return;
        }
        try {
            meta.setProperty(nameSpace, propName, value);
        } catch (XMPException e) {
            Log.e(TAG, "<setPropertyString> XMPException", e);
        } catch (NullPointerException e) {
            // when jpg has this propName, it will throw NullPointerException
            Log.e(TAG, "<setPropertyString> NullPointerException!!!");
        }
    }

    /**
     * setPropertyInteger.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param propName
     *            propName
     * @param value
     *            value
     */
    public void setPropertyInteger(XMPMeta meta, String nameSpace, String propName, int value) {
        if (meta == null) {
            Log.d(TAG, "<setPropertyInteger> meta is null, return!!!");
            return;
        }
        try {
            meta.setPropertyInteger(nameSpace, propName, value);
        } catch (XMPException e) {
            Log.e(TAG, "<setPropertyInteger> XMPException", e);
        } catch (NullPointerException e) {
            // when jpg has this propName, it will throw NullPointerException
            Log.e(TAG, "<setPropertyInteger> NullPointerException!!!", e);
        }
    }

    /**
     * setPropertyBoolean.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param propName
     *            propName
     * @param value
     *            value
     */
    public void setPropertyBoolean(XMPMeta meta, String nameSpace, String propName, boolean value) {
        if (meta == null) {
            Log.d(TAG, "<setPropertyBoolean> meta is null, return!!!");
            return;
        }
        try {
            meta.setPropertyBoolean(nameSpace, propName, value);
        } catch (XMPException e) {
            Log.e(TAG, "<setPropertyBoolean> XMPException", e);
        } catch (NullPointerException e) {
            // when jpg has this propName, it will throw NullPointerException
            Log.e(TAG, "<setPropertyBoolean> NullPointerException!!!", e);
        }
    }

    /**
     * getPropertyBoolean.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param propName
     *            propName
     * @return result
     */
    public boolean getPropertyBoolean(XMPMeta meta, String nameSpace, String propName) {
        if (meta == null) {
            Log.d(TAG, "<getPropertyBoolean> meta is null, return false!!!");
            return false;
        }
        try {
            return meta.getPropertyBoolean(nameSpace, propName);
        } catch (XMPException e) {
            Log.e(TAG, "<getPropertyBoolean> XMPException", e);
            return false;
        } catch (NullPointerException e) {
            // when jpg has this propName, it will throw NullPointerException
            Log.e(TAG, "<getPropertyBoolean> NullPointerException!!!", e);
            return false;
        }
    }

    /**
     * getPropertyInteger.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param propName
     *            propName
     * @return result
     */
    public int getPropertyInteger(XMPMeta meta, String nameSpace, String propName) {
        if (meta == null) {
            Log.d(TAG, "<getPropertyInteger> meta is null, return -1!!!");
            return -1;
        }
        try {
            return meta.getPropertyInteger(nameSpace, propName);
        } catch (XMPException e) {
            Log.e(TAG, "<getPropertyInteger> XMPException", e);
            return -1;
        } catch (NullPointerException e) {
            // when jpg has this propName, it will throw NullPointerException
            Log.e(TAG, "<getPropertyInteger> NullPointerException!!!", e);
            return -1;
        }
    }

    /**
     * serialize.
     *
     * @param meta
     *            meta
     * @return result
     */
    public byte[] serialize(XMPMeta meta) {
        try {
            return XMPMetaFactory.serializeToBuffer(meta, new SerializeOptions()
                    .setUseCompactFormat(true).setOmitPacketWrapper(true));
        } catch (XMPException e) {
            Log.e(TAG, "<serialize> XMPException", e);
        }
        Log.d(TAG, "<serialize> return null!!!");
        return null;
    }

    /**
     * writeBufferToFile.
     *
     * @param desFile
     *            desFile
     * @param buffer
     *            buffer
     * @return result
     */
    public static boolean writeBufferToFile(String desFile, byte[] buffer) {
        if (buffer == null) {
            Log.d(TAG, "<writeBufferToFile> buffer is null");
            return false;
        }
        File out = new File(desFile);
        if (out.exists()) {
            out.delete();
        }
        FileOutputStream fops = null;
        try {
            if (!(out.createNewFile())) {
                Log.d(TAG, "<writeBufferToFile> createNewFile error");
                return false;
            }
            fops = new FileOutputStream(out);
            fops.write(buffer);
            return true;
        } catch (IOException e) {
            Log.e(TAG, "<writeBufferToFile> IOException", e);
            return false;
        } finally {
            try {
                if (fops != null) {
                    fops.close();
                    fops = null;
                }
            } catch (IOException e) {
                Log.e(TAG, "<writeBufferToFile> close, IOException", e);
            }
        }
    }

    /**
     * readFileToBuffer.
     *
     * @param filePath
     *            filePath
     * @return result
     */
    public byte[] readFileToBuffer(String filePath) {
        File inFile = new File(filePath);
        if (!inFile.exists()) {
            Log.d(TAG, "<readFileToBuffer> " + filePath + " not exists!!!");
            return null;
        }

        RandomAccessFile rafIn = null;
        try {
            rafIn = new RandomAccessFile(inFile, "r");
            int len = (int) inFile.length();
            byte[] buffer = new byte[len];
            rafIn.read(buffer);
            return buffer;
        } catch (IOException e) {
            Log.e(TAG, "<readFileToBuffer> Exception ", e);
            return null;
        } finally {
            try {
                if (rafIn != null) {
                    rafIn.close();
                    rafIn = null;
                }
            } catch (IOException e) {
                Log.e(TAG, "<readFileToBuffer> close IOException ", e);
            }
        }
    }

    /**
     * writeStringToFile.
     *
     * @param desFile
     *            desFile
     * @param value
     *            value
     */
    public void writeStringToFile(String desFile, String value) {
        if (value == null) {
            Log.d(TAG, "<writeStringToFile> input string is null, return!!!");
            return;
        }
        File out = new File(desFile);
        PrintStream ps = null;
        try {
            if (out.exists()) {
                out.delete();
            }
            if (!(out.createNewFile())) {
                Log.d(TAG, "<writeStringToFile> createNewFile error");
                return;
            }
            ps = new PrintStream(out);
            ps.println(value);
            ps.flush();
        } catch (IOException e) {
            Log.e(TAG, "<writeStringToFile> Exception ", e);
        } finally {
            out = null;
            if (ps != null) {
                ps.close();
                ps = null;
            }
        }
    }

    /**
     * parseAppInfo.
     *
     * @param filePath
     *            filePath
     * @return result
     */
    public ArrayList<Section> parseAppInfo(String filePath) {
        RandomAccessFile raf = null;
        try {
            raf = new RandomAccessFile(filePath, "r");
            int value = raf.readUnsignedShort();
            if (value != SOI) {
                Log.d(TAG, "<parseAppInfo> error, find no SOI");
            }
            int marker = -1;
            long offset = -1;
            int length = -1;
            ArrayList<Section> sections = new ArrayList<Section>();

            while ((value = raf.readUnsignedShort()) != -1 && value != SOS) {
                marker = value;
                offset = raf.getFilePointer() - 2;
                length = raf.readUnsignedShort();
                sections.add(new Section(marker, offset, length, false, false, false, false, false,
                        false, false, false));
                raf.skipBytes(length - 2);
            }
            // write exif/isXmp flag
            for (int i = 0; i < sections.size(); i++) {
                checkIfXmpOrExifOrJpsInApp1(raf, sections.get(i));
                Log.d(TAG, "<parseAppInfo> marker 0x"
                        + Integer.toHexString(sections.get(i).mMarker)
                        + ", offset 0x" + Long.toHexString(sections.get(i).mOffset) + ", length 0x"
                        + Integer.toHexString(sections.get(i).mLength) + ", isExif "
                        + sections.get(i).mIsExif + ", isXmpMain " + sections.get(i).mIsXmpMain
                        + ", isXmpExt " + sections.get(i).mIsXmpExt + ", isJPSData "
                        + sections.get(i).mIsJpsData + ", isJPSMask " + sections.get(i).mIsJpsMask
                        + ", isDepthData " + sections.get(i).mIsDepthData + ", isXmpDepth "
                        + sections.get(i).mIsXmpDepth + ", isSegmentMask "
                        + sections.get(i).mIsSegmentMask);
            }
            return sections;
        } catch (IOException e) {
            Log.e(TAG, "<parseAppInfo> IOException, path " + filePath, e);
            return null;
        } finally {
            try {
                if (raf != null) {
                    raf.close();
                    raf = null;
                }
            } catch (IOException e) {
                Log.e(TAG, "<parseAppInfo> IOException, path " + filePath, e);
            }
        }
    }

    /**
     * parseAppInfoFromStream.
     *
     * @param is
     *            input stream
     * @return result
     */
    public ArrayList<Section> parseAppInfoFromStream(ByteArrayInputStreamExt is) {
        if (is == null) {
            Log.d(TAG, "<parseAppInfoFromStream> input stream is null!!!");
            return null;
        }
        try {
            // reset position at the file start
            is.seek(0);
            int value = is.readUnsignedShort();
            if (value != SOI) {
                Log.d(TAG, "<parseAppInfoFromStream> error, find no SOI");
            }
            Log.d(TAG, "<parseAppInfoFromStream> parse begin!!!");
            int marker = -1;
            long offset = -1;
            int length = -1;
            ArrayList<Section> sections = new ArrayList<Section>();

            while ((value = is.readUnsignedShort()) != -1 && value != SOS) {
                marker = value;
                offset = is.getFilePointer() - 2;
                length = is.readUnsignedShort();
                sections.add(new Section(marker, offset, length, false, false, false, false, false,
                        false, false, false));
                is.skip(length - 2);
            }

            // write exif/isXmp flag
            for (int i = 0; i < sections.size(); i++) {
                checkIfXmpOrExifOrJpsInStream(is, sections.get(i));
                Log.d(TAG, "<parseAppInfoFromStream> marker 0x"
                        + Integer.toHexString(sections.get(i).mMarker) + ", offset 0x"
                        + Long.toHexString(sections.get(i).mOffset) + ", length 0x"
                        + Integer.toHexString(sections.get(i).mLength) + ", isExif "
                        + sections.get(i).mIsExif + ", isXmpMain " + sections.get(i).mIsXmpMain
                        + ", isXmpExt " + sections.get(i).mIsXmpExt + ", isJPSData "
                        + sections.get(i).mIsJpsData + ", isJPSMask " + sections.get(i).mIsJpsMask
                        + ", isDepthData " + sections.get(i).mIsDepthData + ", isXmpDepth "
                        + sections.get(i).mIsXmpDepth + ", isSegmentMask "
                        + sections.get(i).mIsSegmentMask);
            }
            // reset position at the file start
            is.seek(0);
            Log.d(TAG, "<parseAppInfoFromStream> parse end!!!");
            return sections;
        } catch (IOException e) {
            Log.e(TAG, "<parseAppInfoFromStream> IOException ", e);
            return null;
        }
    }

    /**
     * return marker: write xmp after this marker.
     *
     * @param sections
     *            section array
     * @return result
     */
    public int findProperLocationForXmp(ArrayList<Section> sections) {
        for (int i = 0; i < sections.size(); i++) {
            Section sec = sections.get(i);
            if (sec.mMarker == APP1) {
                if (sec.mIsExif) {
                    return WRITE_XMP_AFTER_FIRST_APP1;
                } else {
                    return WRITE_XMP_BEFORE_FIRST_APP1;
                }
            }
        }
        // means no app1, write after SOI
        return WRITE_XMP_AFTER_SOI;
    }

    /**
     * printXMPMeta.
     *
     * @param meta
     *            XMPMeta
     * @param title
     *            title
     */
    public static void printXMPMeta(XMPMeta meta, String title) {
        String name = meta.getObjectName();
        if (name != null && name.length() > 0) {
            Log.d(TAG, title + " (Name: '" + name + "'):");
        } else {
            Log.d(TAG, title + ":");
        }
        Log.d(TAG, meta.dumpObject());
    }

    /**
     * setStructField.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param structName
     *            structName
     * @param fieldNS
     *            fieldNS
     * @param fieldName
     *            fieldName
     * @param fieldValue
     *            fieldValue
     */
    public void setStructField(XMPMeta meta, String nameSpace, String structName, String fieldNS,
            String fieldName, String fieldValue) {
        if (meta == null) {
            Log.d(TAG, "<setStructField> meta is null, return!!!");
            return;
        }
        try {
            meta.setStructField(nameSpace, structName, fieldNS, fieldName, fieldValue);
        } catch (XMPException e) {
            Log.e(TAG, "<setStructField> XMPException", e);
        } catch (NullPointerException e) {
            Log.e(TAG, "<setStructField> NullPointerException!!!", e);
        }
    }

    /**
     * getStructFieldInt.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param structName
     *            structName
     * @param fieldNS
     *            fieldNS
     * @param fieldName
     *            fieldName
     * @return result
     */
    public int getStructFieldInt(XMPMeta meta, String nameSpace, String structName, String fieldNS,
            String fieldName) {
        if (meta == null) {
            Log.d(TAG, "<getStructFieldInt> meta is null, return!!!");
            return -1;
        }
        try {
            XMPProperty property = meta.getStructField(nameSpace, structName, fieldNS, fieldName);
            return Integer.valueOf((String) property.getValue());
        } catch (XMPException e) {
            Log.e(TAG, "<getStructFieldInt> XMPException", e);
            return -1;
        } catch (NullPointerException e) {
            Log.e(TAG, "<getStructFieldInt> NullPointerException!!!", e);
            return -1;
        }
    }

    /**
     * array index: start from 1, not 0.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param arrayName
     *            arrayName
     * @param index
     *            index
     * @param itemValue
     *            itemValue
     */
    public void setArrayItem(XMPMeta meta, String nameSpace, String arrayName, int index,
            String itemValue) {
        if (meta == null) {
            Log.d(TAG, "<setArrayItem> meta is null, return!!!");
            return;
        }
        try {
            meta.setArrayItem(nameSpace, arrayName, index, itemValue);
        } catch (XMPException e) {
            Log.e(TAG, "<setArrayItem> XMPException", e);
        } catch (NullPointerException e) {
            Log.e(TAG, "<setArrayItem> NullPointerException!!!", e);
        }
    }

    /**
     * appendArrayItem.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param arrayName
     *            arrayName
     * @param arrayOption
     *            arrayOption
     * @param itemValue
     *            itemValue
     * @param itmeOption
     *            itmeOption
     */
    public void appendArrayItem(XMPMeta meta, String nameSpace, String arrayName,
            PropertyOptions arrayOption, String itemValue, PropertyOptions itmeOption) {
        if (meta == null) {
            Log.d(TAG, "<appendArrayItem> meta is null, return!!!");
            return;
        }
        try {
            meta.appendArrayItem(nameSpace, arrayName, arrayOption, itemValue, itmeOption);
        } catch (XMPException e) {
            Log.e(TAG, "<appendArrayItem> XMPException", e);
        } catch (NullPointerException e) {
            Log.e(TAG, "<appendArrayItem> NullPointerException!!!", e);
        }
    }

    /**
     * appendArrayItem.
     *
     * @param meta
     *            meta
     * @param nameSpace
     *            nameSpace
     * @param arrayName
     *            arrayName
     * @param itemValue
     *            itemValue
     */
    public void appendArrayItem(XMPMeta meta, String nameSpace, String arrayName,
            String itemValue) {
        if (meta == null) {
            Log.d(TAG, "<appendArrayItem> meta is null, return!!!");
            return;
        }
        try {
            meta.appendArrayItem(nameSpace, arrayName, itemValue);
        } catch (XMPException e) {
            Log.e(TAG, "<appendArrayItem> XMPException", e);
        } catch (NullPointerException e) {
            Log.e(TAG, "<appendArrayItem> NullPointerException!!!", e);
        }
    }

    /**
     * checkIfXmpOrExifOrJpsInApp1.
     *
     * @param raf
     *            raf
     * @param section
     *            section
     */
    private void checkIfXmpOrExifOrJpsInApp1(RandomAccessFile raf, Section section) {
        if (section == null) {
            Log.d(TAG, "<checkIfXmpOrExifOrJpsInApp1> section is null!!!");
            return;
        }
        byte[] buffer = null;
        String str = null;
        try {
            if (section.mMarker == APP15) {
                raf.seek(section.mOffset + 2 + 2 + TEMP_NUM4);
                buffer = new byte[TEMP_NUM7];
                raf.read(buffer, 0, buffer.length);
                str = new String(buffer);
                if (XmpResource.TYPE_JPS_DATA.equals(str)) {
                    section.mIsJpsData = true;
                    return;
                }
                if (XmpResource.TYPE_JPS_MASK.equals(str)) {
                    section.mIsJpsMask = true;
                    return;
                }
                if (XmpResource.TYPE_DEPTH_DATA.equals(str)) {
                    section.mIsDepthData = true;
                    return;
                }
                if (XmpResource.TYPE_XMP_DEPTH.equals(str)) {
                    section.mIsXmpDepth = true;
                    return;
                }
                if (XmpResource.TYPE_SEGMENT_MASK.equals(str)) {
                    section.mIsSegmentMask = true;
                    return;
                }
            } else if (section.mMarker == APP1) {
                raf.seek(section.mOffset + TEMP_NUM4);
                buffer = new byte[XmpResource.XMP_EXT_MAIN_HEADER1.length()];
                raf.read(buffer, 0, buffer.length);
                str = new String(buffer);
                if (XmpResource.XMP_EXT_MAIN_HEADER1.equals(str)) {
                    section.mIsXmpExt = true;
                    return;
                }
                str = new String(buffer, 0, XmpResource.XMP_HEADER_START.length());
                if (XmpResource.XMP_HEADER_START.equals(str)) {
                    section.mIsXmpMain = true;
                    return;
                }
                str = new String(buffer, 0, XmpResource.EXIF_HEADER.length());
                if (XmpResource.EXIF_HEADER.equals(str)) {
                    section.mIsExif = true;
                    return;
                }
            }
        } catch (UnsupportedEncodingException e) {
            Log.e(TAG, "<checkIfXmpOrExifOrJpsInApp1> UnsupportedEncodingException" + e);
        } catch (IOException e) {
            Log.e(TAG, "<checkIfXmpOrExifOrJpsInApp1> IOException" + e);
        }
    }

    private void checkIfXmpOrExifOrJpsInStream(ByteArrayInputStreamExt is, Section section) {
        if (is == null || section == null) {
            Log.d(TAG, "<checkIfXmpOrExifOrJpsInStream> input stream or section is null!!!");
            return;
        }
        byte[] buffer = null;
        String str = null;
        try {
            if (section.mMarker == APP15) {
                is.seek(section.mOffset + 2 + 2 + TEMP_NUM4);
                buffer = new byte[TEMP_NUM7];
                is.read(buffer, 0, buffer.length);
                str = new String(buffer);
                if (XmpResource.TYPE_JPS_DATA.equals(str)) {
                    section.mIsJpsData = true;
                    return;
                }
                if (XmpResource.TYPE_JPS_MASK.equals(str)) {
                    section.mIsJpsMask = true;
                    return;
                }
                if (XmpResource.TYPE_DEPTH_DATA.equals(str)) {
                    section.mIsDepthData = true;
                    return;
                }
                if (XmpResource.TYPE_XMP_DEPTH.equals(str)) {
                    section.mIsXmpDepth = true;
                    return;
                }
                if (XmpResource.TYPE_SEGMENT_MASK.equals(str)) {
                    section.mIsSegmentMask = true;
                    return;
                }
            } else if (section.mMarker == APP1) {
                is.seek(section.mOffset + TEMP_NUM4);
                buffer = new byte[XmpResource.XMP_EXT_MAIN_HEADER1.length()];
                is.read(buffer, 0, buffer.length);
                str = new String(buffer);
                if (XmpResource.XMP_EXT_MAIN_HEADER1.equals(str)) {
                    // ext main header is same as ext slave header
                    section.mIsXmpExt = true;
                    return;
                }
                str = new String(buffer, 0, XmpResource.XMP_HEADER_START.length());
                if (XmpResource.XMP_HEADER_START.equals(str)) {
                    section.mIsXmpMain = true;
                    return;
                }
                str = new String(buffer, 0, XmpResource.EXIF_HEADER.length());
                if (XmpResource.EXIF_HEADER.equals(str)) {
                    section.mIsExif = true;
                    return;
                }
            }
        } catch (UnsupportedEncodingException e) {
            Log.e(TAG, "<checkIfXmpOrExifOrJpsInStream> UnsupportedEncodingException" + e);
        } catch (IOException e) {
            Log.e(TAG, "<checkIfXmpOrExifOrJpsInStream> IOException" + e);
        }
    }

    private int locateXmpDataEnd(byte[] buffer) {
        int i = buffer.length - 1;
        for (; i > TEMP_NUM3; i--) {
            if (buffer[i] == '>' && buffer[i - 1] == 'a' && buffer[i - 2] == 't'
                    && buffer[i - TEMP_NUM3] == 'e' && buffer[i - TEMP_NUM4] == 'm') {
                return i + 1;
            }
        }
        Log.d(TAG, "<locateXmpDataEnd> error, can not find XmpDataEnd!!!");
        return -1;
    }
}
