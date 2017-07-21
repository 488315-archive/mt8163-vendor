/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.text.util;

import android.os.SystemProperties;
import android.test.AndroidTestCase;
import android.test.suitebuilder.annotation.SmallTest;
import android.text.method.LinkMovementMethod;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.URLSpan;
import android.text.util.Linkify;
import android.text.util.Linkify.MatchFilter;
import android.text.util.Linkify.TransformFilter;
import android.util.Log;
import android.widget.TextView;

/**
 * LinkifyTest tests {@link Linkify}.
 */
public class LinkifyTest extends AndroidTestCase {

    private static final String TAG = "LinkifyTest";

    private static final String LINKIFY_CUSTOM_TEST = "linkify.custom.test";


    @SmallTest
    public void testNothing() throws Exception {
        TextView tv;

        tv = new TextView(getContext());
        tv.setText("Hey, foo@google.com, call 415-555-1212.");

        assertFalse(tv.getMovementMethod() instanceof LinkMovementMethod);
        assertTrue(tv.getUrls().length == 0);
    }

    @SmallTest
    public void testNormal() throws Exception {
        TextView tv;

        tv = new TextView(getContext());
        tv.setAutoLinkMask(Linkify.ALL);
        tv.setText("Hey, foo@google.com, call 415-555-1212.");

        assertTrue(tv.getMovementMethod() instanceof LinkMovementMethod);
        assertTrue(tv.getUrls().length == 2);
    }

    @SmallTest
    public void testUnclickable() throws Exception {
        TextView tv;

        tv = new TextView(getContext());
        tv.setAutoLinkMask(Linkify.ALL);
        tv.setLinksClickable(false);
        tv.setText("Hey, foo@google.com, call 415-555-1212.");

        assertFalse(tv.getMovementMethod() instanceof LinkMovementMethod);
        assertTrue(tv.getUrls().length == 2);
    }


    public void testCustomLink() throws Exception {

        if (SystemProperties.getBoolean(LINKIFY_CUSTOM_TEST, false) == false) {
            Log.d(TAG, "Custom test is not run. Set prop linkify.custom.test to true to run.");
            return;
        }

        String[] testStrings = {
            //0
            "中文www.baidu.com中文",
            "中文www.baidu.com中文www.baidu.com",
            "中文www.baidu.com中文www.baidu.com中文",
            "中文www.baidu.com中文WWW.BAIDU.COM",
            //This case conflicts with Google CTS case
            //"abchttp://www.baidu.comabc",
            //5
            "123中文www.baidu.com",
            "中文www.baidu.com，登陆",
            "中文1008611中文www.baidu.com中文test@163.com",
            "1.中文www.baidu.com",
            "abc.中文www.baidu.com",
            //10
            "全角符号，www.baidu.com。中文www.baidu.com 空格www.baidu.com,半角符号www.baidu.com",
            "英文符号,www.baidu.com.中文www.baidu.com中文,空格 www.baidu.com",
            "10010，中文：12.87元，使用全角符号，中文www.baidu.com，中文。",
            "中文www.baidu.com，全角符号",
            "http://123.com:3000/mp3鏈結",
             //15
            "www.baidu.com。 1555666。",
            "中国电信www.baidu.com,中国电信",
            "中国电信Http://www.baidu.COM中国电信",
            "中国电信HTtp://www.baidu.COM中国电信",
            "中国电信HTTp://www.baidu.COM中国电信",
            //20
            "中国电信192.168.100.11/index.htm",
            "192.168.100.11/index.htm中国电信",
            "中国电信http://wh0810.f3.comsz.com/xjy/music/北京欢迎你.mp3中国电信",
            "中国电信www.baidu.COM,中国电信",
            "中国电信WWW.baidu.com,中国电信",
            //25
            "请到HTTP://wh0810.f3.comsz.com/xjy/music/北京欢迎你.mp3下载",
            "http://www.tjbjss.com/tjxinxing/zww/zywN/pic/no1_15/报告.html链接",
            "http://ditu.google.cn/?ll=25.18788,128.887898,",
            "http://maps.google.com/maps?f=q&q=(32.08831,118.753944),中文",
            "http://maps.google.com/maps?f=q&q=(32.08831,118.753944)中文",

        };

        String[][] expectedResults = new String[][] {
            //0
            {"http://www.baidu.com"},
            {"http://www.baidu.com", "http://www.baidu.com"},
            {"http://www.baidu.com", "http://www.baidu.com"},
            {"http://www.baidu.com", "http://WWW.BAIDU.COM"},
            //{"http://www.baidu.com"},
            //5
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            //10
            {"http://www.baidu.com", "http://www.baidu.com", "http://www.baidu.com",
             "http://www.baidu.com"},
            {"http://www.baidu.com", "http://www.baidu.com", "http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://123.com:3000/mp3"},
            //15
            {"http://www.baidu.com"},
            {"http://www.baidu.com"},
            {"http://www.baidu.COM"},
            {"http://www.baidu.COM"},
            {"http://www.baidu.COM"},
            //20
            {"http://192.168.100.11/index.htm"},
            {"http://192.168.100.11/index.htm"},
            {"http://wh0810.f3.comsz.com/xjy/music/北京欢迎你.mp3"},
            {"http://www.baidu.COM"},
            {"http://WWW.baidu.com"},
            //25
            {"http://wh0810.f3.comsz.com/xjy/music/北京欢迎你.mp3"},
            {"http://www.tjbjss.com/tjxinxing/zww/zywN/pic/no1_15/报告.html"},
            {"http://ditu.google.cn/?ll=25.18788,128.887898"},
            {"http://maps.google.com/maps?f=q&q=(32.08831,118.753944)"},
            {"http://maps.google.com/maps?f=q&q=(32.08831,118.753944)"},

        };

        boolean allPass = true;
        for (int i = 0; i < testStrings.length; i++) {
            boolean pass = true;
            SpannableString spannable = new SpannableString(testStrings[i]);

            Linkify.addLinks(spannable, Linkify.WEB_URLS);
            URLSpan[] spans = spannable.getSpans(0, spannable.length(), URLSpan.class);

            if (expectedResults[i].length != spans.length) {
                pass = false;
                allPass = false;
            } else {
                for (int j = 0; j < expectedResults[i].length; j++) {
                    if (!expectedResults[i][j].equals(spans[j].getURL())) {
                        pass = false;
                        allPass = false;
                    }
                }
            }

            if (!pass) {
                Log.d(TAG, "case " + i + " failed, string = " + testStrings[i]);
                for (int j = 0; j < spans.length; j++) {
                    Log.d(TAG, spans[j].getURL());
                }
            }
        }
        assertTrue(allPass);
    }
}
