/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

package com.mediatek.engineermode.eminfo;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class UrcParser {
    private static final String TAG = "EmInfo";
    private static final int LENGTH_1_BYTE = 1;
    private static final int LENGTH_2_BYTES = 2;
    private static final int LENGTH_4_BYTES = 4;

    private static String sInput;
    private static int sOffset;
    private static Map<Integer, Parser> sParsers = null;

    public static Parser parse(int type, String data) {
        if (sParsers == null) {
            init();
        }
        sInput = data;
        sOffset = 0;
        Parser parser = sParsers.get(type);
        if (parser != null) {
            parser.parse();
        }
        return parser;
    }

    private static void init() {
        sParsers = new HashMap<Integer, Parser>();
        for (int i = 0; i < Content.MAPPING.length; i++) {
            int type = (Integer) Content.MAPPING[i][0];
            String struct = (String) Content.MAPPING[i][1];
            Parser p = createParser(struct, "");
            sParsers.put(type, p);
        }
    }

    private static Parser createParser(String type, String name) {
        Parser f = null;
        int i = name.indexOf("[");
        if (i > 0) {
            String dim = name.substring(i + 1);
            name = name.substring(0, i);
            String[] splited = dim.split("[\\[\\]]");
            List<Integer> dimensions = new ArrayList<Integer>();
            for (i = 0; i < splited.length; i++) {
                if (splited[i] != null && splited[i].length() > 0) {
                    dimensions.add(Integer.parseInt(splited[i]));
                }
            }
            f = new ArrayParser(type, name, dimensions);
        } else if (type.equals("uint8")) {
            f = new NumberParser(name, LENGTH_1_BYTE, false);
        } else if (type.equals("int8")) {
            f = new NumberParser(name, LENGTH_1_BYTE, true);
        } else if (type.equals("uint16")) {
            f = new NumberParser(name, LENGTH_2_BYTES, false);
        } else if (type.equals("int16")) {
            f = new NumberParser(name, LENGTH_2_BYTES, true);
        } else if (type.equals("uint32")) {
            f = new NumberParser(name, LENGTH_4_BYTES, false);
        } else if (type.equals("int32")) {
            f = new NumberParser(name, LENGTH_4_BYTES, true);
        } else if (type.equals("bool")) {
            f = new BooleanParser(name);
        } else if (type.startsWith("union")) {
            f = new UnionParser(name, Content.findStructDefinition(type));
        } else {
            f = new StructParser(name, Content.findStructDefinition(type));
        }
        return f;
    }

    private static int alignTo(int value, int alignment) {
        return (value + (alignment - 1)) & ~(alignment - 1);
    }

    /**
     * @param data
     *            the value of the bit
     * @param start
     *            the integer of the start position
     * @param bytes
     *            the length of the value
     * @param signed
     *            the boolean of the signed is false
     * @return the value of the String for every item
     */
    private static long getValueFromByte(String data, int start, int bytes, boolean signed) {
        if (data.length() < start + 2 * bytes) {
            return 0;
        }
        try {
            switch (bytes) {
            case LENGTH_1_BYTE:
                String sub = data.substring(start, start + 2);
                if (signed) {
                    short s = Short.valueOf(sub, 16);
                    Byte b = (byte) s;
                    return b;
                } else {
                    return Short.valueOf(sub, 16);
                }
            case LENGTH_2_BYTES:
                String low = data.substring(start, start + 2);
                String high = data.substring(start + 2, start + 4);
                String reverse = high + low;
                if (signed) {
                    int i = Integer.valueOf(reverse, 16);
                    Short s = (short) i;
                    return s;
                } else {
                    return Integer.valueOf(reverse, 16);
                }
            case LENGTH_4_BYTES:
                String byte1 = data.substring(start, start + 2);
                String byte2 = data.substring(start + 2, start + 4);
                String byte3 = data.substring(start + 4, start + 6);
                String byte4 = data.substring(start + 6, start + 8);
                String reverse2 = byte4 + byte3 + byte2 + byte1;
                if (signed) {
                    long lg = Long.valueOf(reverse2, 16);
                    Integer i = (int) lg;
                    return i;
                } else {
                    return Long.valueOf(reverse2, 16);
                }
            }
        } catch (NumberFormatException e) {
            return 0;
        }
        return 0;
    }

    public static abstract class Parser {
        protected String mName = null;
        protected int mSize = 1;
        protected int mAlignment = 1;

        protected void parse() {
            sOffset = alignTo(sOffset, mAlignment * 2);
            doParse();
        }

        abstract void doParse();

        int getInt() {
            throw new RuntimeException("");
        }

        long getLong() {
            throw new RuntimeException("");
        }

        boolean getBoolean() {
            throw new RuntimeException("");
        }

        public List<Integer> getIntArray() {
            throw new RuntimeException("");
        }

        List<Long> getLongArray() {
            throw new RuntimeException("");
        }

        public List<Boolean> getBooleanArray() {
            throw new RuntimeException("");
        }

        public List<Parser> getArray() {
            throw new RuntimeException("");
        }

        public int getInt(String key) {
            throw new RuntimeException("");
        }

        long getLong(String key) {
            throw new RuntimeException("");
        }

        boolean getBoolean(String key) {
            throw new RuntimeException("");
        }

        public Parser get(String key) {
            throw new RuntimeException("");
        }

        public List<Integer> getIntArray(String key) {
            throw new RuntimeException("");
        }

        List<Long> getLongArray(String key) {
            throw new RuntimeException("");
        }

        public List<Boolean> getBooleanArray(String key) {
            throw new RuntimeException("");
        }

        public List<Parser> getArray(String key) {
            throw new RuntimeException("");
        }
    }

    static class NumberParser extends Parser {
        private final int mBytes;
        private final boolean mSigned;
        private long mResult;

        public NumberParser(String name, int bytes, boolean signed) {
            mName = name;
            mBytes = bytes;
            mSigned = signed;
            mSize = bytes;
            mAlignment = bytes;
        }

        @Override
        void doParse() {
            mResult = getValueFromByte(sInput, sOffset, mBytes, mSigned);
            sOffset += 2 * mBytes;
        }

        @Override
        public int getInt() {
            if (mBytes == LENGTH_4_BYTES && !mSigned) {
                throw new RuntimeException("");
            }
            return (int) mResult;
        }

        @Override
        public long getLong() {
            return mResult;
        }
    }

    static class BooleanParser extends Parser {
        private boolean mResult;

        public BooleanParser(String name) {
            mName = name;
        }

        @Override
        void doParse() {
            mResult = getValueFromByte(sInput, sOffset, 1, true) != 0;
            sOffset += 2;
        }

        @Override
        public boolean getBoolean() {
            return mResult;
        }
    }

    static class ArrayParser extends Parser {
        List<Parser> mFields = new ArrayList<Parser>();

        public ArrayParser(String type, String name, List<Integer> dimensions) {
            mName = name;
            List<Integer> tmp = new ArrayList<Integer>(dimensions);
            int dim = tmp.size();
            int count = tmp.remove(0);
            for (int i = 0; i < count; i++) {
                if (dim == 1) {
                    mFields.add(createParser(type, String.valueOf(i)));
                } else {
                    mFields.add(new ArrayParser(type, String.valueOf(i), tmp));
                }
            }
            mSize = mFields.get(0).mSize * count;
            mAlignment = mFields.get(0).mAlignment;
        }

        @Override
        void doParse() {
            for (Parser p : mFields) {
                p.parse();
            }
        }

        @Override
        public List<Parser> getArray() {
            return mFields;
        }

        @Override
        public List<Integer> getIntArray() {
            List<Integer> ret = new ArrayList<Integer>();
            for (Parser p : mFields) {
                ret.add(p.getInt());
            }
            return ret;
        }

        @Override
        public List<Long> getLongArray() {
            List<Long> ret = new ArrayList<Long>();
            for (Parser p : mFields) {
                ret.add(p.getLong());
            }
            return ret;
        }

        @Override
        public List<Boolean> getBooleanArray() {
            List<Boolean> ret = new ArrayList<Boolean>();
            for (Parser p : mFields) {
                ret.add(p.getBoolean());
            }
            return ret;
        }
    }

    static class StructParser extends Parser {
        List<Parser> mFields = new ArrayList<Parser>();
        Map<String, Parser> mMapping = new HashMap<String, Parser>();

        public StructParser(String name, String[] fields) {
            mName = name;
            int size = 0;
            for (int i = 0; i < fields.length; i++) {
                String[] splited = fields[i].split(" ");
                String fieldType = splited[0];
                String fieldName = splited[1];
                Parser p = createParser(fieldType, fieldName);
                mFields.add(p);
                mMapping.put(p.mName, p);

                if (p.mAlignment > mAlignment) {
                    mAlignment = p.mAlignment;
                }
                size = alignTo(size, p.mAlignment);
                size += p.mSize;
            }
        }

        @Override
        void doParse() {
            for (Parser f : mFields) {
                f.parse();
            }
        }

        @Override
        public int getInt(String key) {
            return mMapping.get(key).getInt();
        }

        @Override
        long getLong(String key) {
            return mMapping.get(key).getLong();
        }

        @Override
        public boolean getBoolean(String key) {
            return mMapping.get(key).getBoolean();
        }

        @Override
        public Parser get(String key) {
            return mMapping.get(key);
        }

        @Override
        public List<Integer> getIntArray(String key) {
            return mMapping.get(key).getIntArray();
        }

        @Override
        List<Long> getLongArray(String key) {
            return mMapping.get(key).getLongArray();
        }

        @Override
        public List<Boolean> getBooleanArray(String key) {
            return mMapping.get(key).getBooleanArray();
        }

        @Override
        public List<Parser> getArray(String key) {
            return mMapping.get(key).getArray();
        }
    }

    static class UnionParser extends StructParser {
        public UnionParser(String name, String[] fields) {
            super(name, fields);
        }

        @Override
        void doParse() {
            int startOffset = sOffset;
            for (Parser f : mFields) {
                sOffset = startOffset;
                f.parse();
            }
            sOffset = startOffset + mSize;
        }
    }
}
